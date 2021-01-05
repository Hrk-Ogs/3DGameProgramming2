#include "inc_PostProcess.hlsli"

// 入力テクスチャ
Texture2D g_inputTex : register(t0);
// サンプラ(Linear補間 Clampモード)
SamplerState g_ss : register(s3);

// ACES Filmic ToneMapping
// HDRの色をLDRに変換する
// ・x … HDRカラー(RGB)
// ・戻り値 … ToneMap後の色
float3 ACESFilmicToneMapping(float3 x)
{
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;

	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

// トーンマッピング ピクセルシェーダー
float4 main(VSOutput In) : SV_Target0
{
	// 調整用
	const float linearExposure = 0.7;

	// テクスチャから色を取得し圧縮する
	float4 texColor = g_inputTex.Sample(g_ss, In.UV);
	texColor.rgb = ACESFilmicToneMapping(texColor.rgb * linearExposure);

	return texColor;
}