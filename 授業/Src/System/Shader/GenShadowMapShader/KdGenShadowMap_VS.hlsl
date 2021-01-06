#include "../inc_KdCommon.hlsli"
#include "inc_KdGenShadowMap.hlsli"

// 頂点シェーダ
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0)
{
	VSOutput Out;

	// 射影変換
	Out.Pos = mul(pos, g_mW);
	Out.Pos = mul(Out.Pos, g_mLightVP);

	// 別に射影座標を持っていく(Out.Posは、PSに来た時にはスクリーン座標系になっているため)
	Out.wvpPos = Out.Pos;
	// UV
	Out.UV = uv;

	return Out;
}