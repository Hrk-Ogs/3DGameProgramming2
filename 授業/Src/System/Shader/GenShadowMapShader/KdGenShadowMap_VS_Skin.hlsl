#include "../inc_KdCommon.hlsli"
#include "inc_KdGenShadowMap.hlsli"

// �{�[���s��o�b�t�@
struct BoneData
{
	row_major float4x4 mat;
};
StructuredBuffer<BoneData> g_sbBones : register(t100);

// ���_�V�F�[�_
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0,
	uint4 skinIndex : SKININDEX,
	float4 skinWeight : SKINWEIGHT
)
{
	//------------------------------
	// �X�L�j���O����
	// ���̒��_�ɉe�����Ă��镡���̃{�[���̍s����g�p����
	// pos��normal�Ȃǂ�ϊ�����
	//------------------------------
	row_major float4x4 mBones = 0; // �ŏI�I�ȃ��[���h�s��
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		mBones += g_sbBones[skinIndex[i]].mat * skinWeight[i];
	}
	// �{�[���s��ŕϊ�(�܂����[�J�����W�n)
	pos = mul(pos, mBones);
	//------------------------------

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