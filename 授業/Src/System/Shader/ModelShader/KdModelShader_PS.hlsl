// カメラやライトのデータが使えるようになる
#include "../inc_KdCommon.hlsli"
// 共通データ
#include "inc_KdModelShader.hlsli"

// テクスチャ（t0とかt1とかが、PSSえｔShaderResourcesでセットしたときのスロット番号）
Texture2D g_baseTex : register(t0);		// ベースカラーテクスチャー
Texture2D g_emissiveTex : register(t1);	// エミッシブテクスチャ
Texture2D g_mrTex : register(t2);		// メタリック/ラフネステクスチャ
Texture2D g_normalTex : register(t3);	// 法線マップ
Texture2D g_toonTex : register(t4); // トゥーンテクスチャ

// レンダリングされたシーン画面
Texture2D g_sceneTex : register(t10);

// IBL
TextureCube g_IBLTex : register(t11); // IBLテクスチャ

// サンプラ(テクスチャから色を取得するときに使う）
SamplerState g_ss : register(s0);
SamplerState g_clampSS : register(s1); // Clamp設定のサンプラ(KdShaderManagerでs1にセットしています)

//======================================
// ピクセルシェーダー
//======================================
// BlinnPhong NDF
// ・lightDir … ライトの方向
// ・vCam … ピクセルからカメラへの方向
// ・normal … 法線
// ・specPower … 反射の鋭さ
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // カメラの角度差(0～1)
	float spec = pow(NdotH, specPower);
	// 正規化Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

float4 main(VSOutput In) :SV_Target0
{
	// カメラへの方向
	//float3 vCam = normalize(g_CamPos - In.wPos);
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam);			// カメラへの距離
	vCam = normalize(vCam);

	// 法線マップから正規化
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;
	// 0-1から-1～1へ復元する
	wN = wN * 2.0 - 1.0;

	// 法線行列（回転行列）で変換する
	float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};
	wN = mul(wN, mTBN);		// TBNで変換

	wN = normalize(wN);		// 正規化

	//----------------------------------------
	// 材質色
	//----------------------------------------
	// テクスチャ色
	float4 texColor = g_baseTex.Sample(g_ss,In.UV);
	// マテリアルの色と頂点色と合成
	float4 baseColor = texColor * g_material.BaseColor * In.Color * g_mulMaterialColor;

	// RoughnessMetallicMapから色取得
	float4 mr = g_mrTex.Sample(g_ss, In.UV);

	// 粗さ
	float roughness = g_material.Roughness * mr.g;
	// 金属性
	float metallic = g_material.Metallic * mr.b;

	// roughnessを逆転させ「滑らか」さにする
	// roughnessを２乗しているのは、その方が感覚的にリアルになるらしいので
	float smoothness = 1.0 - roughness * roughness;
	float specPower = pow(2, 11 * smoothness); // 1～2048


	//------------------------------------------
	// ライティング
	//------------------------------------------
	// 最終的な色
	float3 color = 0;

	// ライト有効時
	if (g_lightEnable)
	{
		// 材質の拡散色 非金属ほど材質の色になり、金属ほど拡散色は無くなる
		float3 baseDiffuse = lerp(baseColor.rgb, float3(0, 0, 0), metallic);
		//材質の反射色 非金属ほど光の色をそのまま反射し、金属ほど材質の色が乗る
		float3 baseSpecular = lerp(0.04, baseColor.rgb, metallic);

		// 平行光は複数あるため、使用しているはずだけ計算する
		for (int di = 0; di < g_DL_Cnt; di++)
		{
			//--------------------------------------
			// 拡散光
			//--------------------------------------
			// Lambertモデル
			float lightDiffuse = dot(-g_DL[di].Dir, wN);	// 内積で角度の差を求める
			lightDiffuse = saturate(lightDiffuse); // マイナスはまずいので、0～1内に切り詰める

			// 「明るさ」をU座標として、トゥーンテクスチャから最終的な明るさデータを取得
			float3 toonColor = g_toonTex.Sample(g_clampSS, float2(lightDiffuse, 0.5)).rgb;

			// 物理的に正しくするため、正規化する
			toonColor /= 3.1415926535;

			// 飛び出る光を計算し、結果に加算する
			// (光の色) * 材質の色 * 透明力
			color += (g_DL[di].Color * toonColor) * baseDiffuse * baseColor.a;

			//--------------------------------------
			// 反射光
			//--------------------------------------
			// Blinn-Phong
			float spec = BlinnPhong(g_DL[di].Dir, vCam, wN, specPower);

			// 反射する光を計算し、結果に加算する
			// (光の色) * 非金属は4%反射らしい * 透明力
			color += (g_DL[di].Color * spec) * baseSpecular * baseColor.a;

		}

		//----------------------------
		// 環境光
		//----------------------------
		color += g_AmbientLight * baseColor.rgb * baseColor.a;

		//------------------
		// IBL
		//------------------
		// 拡散光
		float3 envDiff = g_IBLTex.SampleLevel(g_ss, wN, 8).rgb; // 拡散表現するため、超低解像度の画像を使用する
		color += envDiff * baseDiffuse.rgb / 3.141592;

		// 反射光
		float3 vRef = reflect(-vCam, wN); // 反射ベクトルを求める
		float3 envSpec = g_IBLTex.SampleLevel(g_ss, vRef, roughness * 8).rgb; // 粗いほど低解像度の画像を使用する
		color += envSpec * baseSpecular;
		}
	// ライト無効時
	else
	{
		// 材質の色をそのまま使用
		color = baseColor.rgb;
	}

	// 屈折表現
	if (g_refractiveIndex < 1.0)
	{
		// カメラ方向を使用して屈折ベクトルを求める
		float3 vRefract = refract(-vCam, wN, g_refractiveIndex);
		float3 v = vRefract - (-vCam);
		// このピクセルの画面上でのUV座標を求める
		// 屈折方向にすこし移動させた3D座標を、射影座標へ変換
		float4 posProj = mul(float4(In.wPos + v * 0.5f, 1), g_mV);
		posProj = mul(posProj, g_mP);
		posProj.xyz /= posProj.w;
		// 射影座標からUV座標へ変換
		float2 uv = posProj.xy * float2(1, -1);
		uv = uv * 0.5 + 0.5;
		// シーン画像からuvの位置の色を取得
		float4 sceneColor = g_sceneTex.Sample(g_clampSS, uv);
		// 物質の色と背景の色をアルファブレンド
		// ※ n = lerp(a, b, t) … a～b間のtの位置の値を求める
		color = lerp(sceneColor.rgb, color.rgb, baseColor.a);
		baseColor.a = 1; // 不透明にする
	}

	//------------------------------------
	// 距離フォグ
	//------------------------------------
	if (g_DistanceFogEnable && g_FogEnable)
	{
		// 指数フォグ1（近い）～0（遠い）
		float f = saturate(1.0 / exp(camDist * g_DistanceFogDensity));
		// 適用
		color.rgb = lerp(g_DistanceFogColor, color.rgb, f);
	}

	return float4(color, baseColor.a);
}
