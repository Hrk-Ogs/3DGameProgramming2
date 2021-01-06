#include "../inc_KdCommon.hlsli"
#include "inc_KdGenShadowMap.hlsli"

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0)
{
	VSOutput Out;

	// �ˉe�ϊ�
	Out.Pos = mul(pos, g_mW);
	Out.Pos = mul(Out.Pos, g_mLightVP);

	// �ʂɎˉe���W�������Ă���(Out.Pos�́APS�ɗ������ɂ̓X�N���[�����W�n�ɂȂ��Ă��邽��)
	Out.wvpPos = Out.Pos;
	// UV
	Out.UV = uv;

	return Out;
}