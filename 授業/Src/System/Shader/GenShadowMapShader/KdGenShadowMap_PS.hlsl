#include "inc_KdGenShadowMap.hlsli"

Texture2D g_baseTex : register(t0);
SamplerState g_ss : register(s0);

// ピクセルシェーダ
float4 main(VSOutput In) : SV_Target0
{
	// テクスチャから色取得
	float4 texCol = g_baseTex.Sample(g_ss, In.UV);
	// アルファテスト
	if (texCol.a < 0.1)
	{
	discard;
	}

	// 深度を求めてR要素として出力(R以外のGBAは使用しないので、0,0,1とでもしておきます。)
	return float4(In.wvpPos.z / In.wvpPos.w, 0, 0, 1);
}