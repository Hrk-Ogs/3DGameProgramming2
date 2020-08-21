// �J�����⃉�C�g�̃f�[�^���g����悤�ɂȂ�
#include "../inc_KdCommon.hlsli"

// �萔�o�b�t�@
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;	// ���[���h�s��
}

// ���_�V�F�[�_�[����o�͂���f�[�^
struct VSOutput
{
	float4 Pos : SV_Position;		// �ˉe���W
};

//======================================
// ���_�V�F�[�_�[
//======================================
VSOutput main(float4 pos : POSITION)
{
	VSOutput Out;

	// 3D���W��2D�i�ˉe�j���W�֕ϊ�
	Out.Pos = mul(pos, g_mW);	// ���[���h�s��ŕϊ����Ă���A
	Out.Pos = mul(pos, g_mP);	// �ˉe�ϊ�

	return Out;
}
