#include "inc_PostProcess.hlsli"

// 定数バッファ
cbuffer cb : register(b0)
{
	float4 g_Offset[31]; // xy:オフセット座標 z:重み
};

// 入力テクスチャ
Texture2D g_InputTex : register(t0);
// サンプラ(Linear補間 Clampモード)
SamplerState g_ss : register(s3);

// ぼかしピクセルシェーダー
float4 main(VSOutput In) : SV_Target0
{
	float3 color = 0;
	// 31ピクセルぶんをブレンドする
	for (int i = 0; i < 31; i++)
	{
		// UVの位置からOffsetのxyだけずらした位置の色を取得し、重みを掛けて足しこむ
		color += g_InputTex.Sample(g_ss, In.UV + g_Offset[i].xy).rgb * g_Offset[i].z;
	}

	return float4(color, 1);
}