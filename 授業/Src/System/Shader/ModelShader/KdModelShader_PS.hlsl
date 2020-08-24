// カメラやライトのデータが使えるようになる
#include "../inc_KdCommon.hlsli"
// 共通データ
#include "inc_KdModelShader.hlsli"

// テクスチャ（t0とかt1とかが、PSSえｔShaderResourcesでセットしたときのスロット番号）
Texture2D g_baseTex : register(t0);		// ベースカラーテクスチャー
Texture2D g_emissiveTex : register(t1);	// エミッシブテクスチャ
Texture2D g_mrTex : register(t2);		// メタリック/ラフネステクスチャ
Texture2D g_normalTex : register(t3);	// 法線マップ

// サンプラ(テクスチャから色を取得するときに使う）
SamplerState g_ss : register(s0);

//======================================
// ピクセルシェーダー
//======================================
float4 main(VSOutput In) :SV_Target0
{
	//----------------------------------------
	// 材質色
	//----------------------------------------
	// テクスチャ色
	float4 texColor = g_baseTex.Sample(g_ss,In.UV);
	// マテリアルの色と頂点色と合成
	float4 baseColor = texColor * g_material.BaseColor * In.Color;

	return baseColor;
}
