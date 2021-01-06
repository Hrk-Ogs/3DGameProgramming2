// �J�����⃉�C�g�̃f�[�^���g����悤�ɂȂ�
#include "../inc_KdCommon.hlsli"
// ���ʃf�[�^
#include "inc_KdModelShader.hlsli"

//======================================
// ���_�V�F�[�_�[
//======================================
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR,float3 tangent : TANGENT, float3 normal : NORMAL)
{
	VSOutput Out;

	// 3D���W��2D�i�ˉe�j���W�֕ϊ�
	Out.Pos = mul(pos, g_mW);		// ���[���h�s��ŕϊ����Ă���A
	Out.wPos = Out.Pos.xyz;			// ���[���h���W������Ă���
	Out.Pos = mul(Out.Pos, g_mV);	// �J�����̋t�s��i�r���[�s��j�ŕϊ����āA
	Out.Pos = mul(Out.Pos, g_mP);	// �ˉe�ϊ�

	// UV�����H���s�N�Z���V�F�[�_�[��
	Out.UV = uv * g_UVTiling + g_UVOffset;

	// ���_�F�͂��̂܂܃s�N�Z���V�F�[�_�[��
	Out.Color = color;

	// �@�������[���h���W�n�֕ϊ�
	Out.wN = normalize(mul(normal, (float3x3)g_mW));
	// �ڐ������[���h���W�n�֕ϊ�
	Out.wT = normalize(mul(tangent, (float3x3)g_mW));
	// �]�@�����A�@���Ɛڐ�����Z�o
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}
