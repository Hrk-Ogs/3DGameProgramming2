// �J�����⃉�C�g�̃f�[�^���g����悤�ɂȂ�
#include "../inc_KdCommon.hlsli"
// ���ʃf�[�^
#include "inc_KdModelShader.hlsli"

//======================================
// ���_�V�F�[�_�[
//======================================
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR)
{
	VSOutput Out;

	// 3D���W��2D�i�ˉe�j���W�֕ϊ�
	Out.Pos = mul(pos, g_mW);		// ���[���h�s��ŕϊ����Ă���A
	Out.Pos = mul(Out.Pos, g_mV);	// �J�����̋t�s��i�r���[�s��j�ŕϊ����āA
	Out.Pos = mul(Out.Pos, g_mP);	// �ˉe�ϊ�

	// UV�͂��̂܂܃s�N�Z���V�F�[�_�[��
	Out.UV = uv;

	// ���_�F�͂��̂܂܃s�N�Z���V�F�[�_�[��
	Out.Color = color;

	return Out;
}
