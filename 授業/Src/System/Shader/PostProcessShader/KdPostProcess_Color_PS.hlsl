#include "inc_PostProcess.hlsli"

// 定数バッファ
cbuffer cb : register(b0)
{
	float4 g_Color; // 合成色
};

// 入力テクスチャ
Texture2D g_inputTex : register(t0);
// サンプラ(Linear補間 Clampモード)
SamplerState g_ss : register(s3);

// ピクセルシェーダー
float4 main(VSOutput In) : SV_Target0
{
	// テクスチャから色を取得
	float4 texColor = g_inputTex.Sample(g_ss, In.UV);
	// 定数バッファの色を合成し出力
	return texColor * g_Color;
}