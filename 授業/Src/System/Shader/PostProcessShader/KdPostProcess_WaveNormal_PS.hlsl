#include "inc_PostProcess.hlsli"

// 入力テクスチャ(高さマップ)
Texture2D g_heightTex : register(t0);

// サンプラ(Linear補間 Clampモード)
SamplerState g_ss : register(s0);

// 波 法線マップ更新ピクセルシェーダー
float4 main(VSOutput In) : SV_Target0
{
	// 画像の幅と高さをゲット(ピクセル)
	float w, h;
	g_heightTex.GetDimensions(w, h);
	// ピクセル -> テクセル変換
	float tw = 1.0 / w;
	float th = 1.0 / h;

	// 中心点と隣接する4点をサンプリング
	float4 t1 = g_heightTex.Sample(g_ss, In.UV + float2(-tw, 0));
	float4 t2 = g_heightTex.Sample(g_ss, In.UV + float2(tw, 0));
	float4 t3 = g_heightTex.Sample(g_ss, In.UV + float2(0, -th));
	float4 t4 = g_heightTex.Sample(g_ss, In.UV + float2(0, th));

	// 左右、上下の差から法線ベクトルを求める
	float3 normal;
	normal.x = (t1.r - t2.r) * 0.5f;
	normal.y = (t3.r - t4.r) * 0.5f;
	normal.z = 1;
	normal = normalize(normal);

	// -1～1から0～1へ変換し出力
	normal = normal * 0.5 + 0.5;

	return float4(normal, 1);
}