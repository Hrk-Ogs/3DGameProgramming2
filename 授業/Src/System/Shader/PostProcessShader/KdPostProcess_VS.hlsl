#include "inc_PostProcess.hlsli"

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0
)
{
	VSOutput Out;

	// ���W�����̂܂܏o��
	Out.Pos = pos;
	// UV�����̂܂܏o��
	Out.UV = uv;
	return Out;
}