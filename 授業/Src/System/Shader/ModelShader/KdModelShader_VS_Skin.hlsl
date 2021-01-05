// �J�����⃉�C�g�̃f�[�^���g����悤�ɂȂ�
#include "../inc_KdCommon.hlsli"
// ���ʃf�[�^
#include "inc_KdModelShader.hlsli"

// �{�[���s��o�b�t�@(�\�����o�b�t�@)
struct BoneData
{
	row_major float4x4 mat;
};
StructuredBuffer<BoneData> g_sbBones : register(t100);

//==================================
// ���_�V�F�[�_�[
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV���W�������Ă���
	float4 color : COLOR, // ���_�F�������Ă���
	float3 tangent : TANGENT, // �ڐ��������Ă���
	float3 normal : NORMAL, // �@���������Ă���
	uint4 skinIndex : SKININDEX, // ���Ԗڂ̃{�[���ɉe�����Ă��邩�H�̃f�[�^(�ő�S�Ԃ�)
	float4 skinWeight : SKINWEIGHT // ���̃{�[���ɁA�ǂꂾ���e�����Ă��邩�H�̃f�[�^(�ő�S�Ԃ�)
)
{
	VSOutput Out;

	//------------------------------
	// �X�L�j���O����
	// ���̒��_�ɉe�����Ă��镡���̃{�[���̍s����g�p����
	// pos��normal�Ȃǂ�ϊ�����
	//------------------------------
	row_major float4x4 mBones = 0; // �ŏI�I�ȕϊ��s��
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		mBones += g_sbBones[skinIndex[i]].mat * skinWeight[i];
	}
	// �ϊ��s��ŕϊ�(�܂����[�J�����W�n)
	pos = mul(pos, mBones);
	normal = mul(normal, (float3x3) mBones);
	//------------------------------

	// 3D���_���W��2D�֕ϊ�
	Out.Pos = mul(pos, g_mW); // ���[���h�s��ŕϊ����Ă���A
	Out.wPos = Out.Pos.xyz; // ���[���h���W������Ă���
	Out.Pos = mul(Out.Pos, g_mV); // �J�����̋t�s��(�r���[�s��)�ŕϊ����āA
	Out.Pos = mul(Out.Pos, g_mP); // �ˉe�s��ŕϊ�

	// UV�����H���s�N�Z���V�F�[�_�[��
	Out.UV = uv * g_UVTiling + g_UVOffset;

	// ���_�F�͂��̂܂܃s�N�Z���V�F�[�_�[��
	Out.Color = color;

	//	�@�������[���h���W�n�֕ϊ�
	Out.wN = normalize(mul(normal, (float3x3)g_mW));
	// �ڐ������[���h���W�n�֕ϊ�
	Out.wT = normalize(mul(tangent, (float3x3)g_mW));
	// �]�@����@���Ɛڐ�����Z�o
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}