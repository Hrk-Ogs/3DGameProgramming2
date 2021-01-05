#include "inc_PostProcess.hlsli"

// 頂点シェーダ
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0
)
{
	VSOutput Out;

	// 座標をそのまま出力
	Out.Pos = pos;
	// UVもそのまま出力
	Out.UV = uv;
	return Out;
}