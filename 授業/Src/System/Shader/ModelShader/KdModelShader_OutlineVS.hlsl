// �J�����⃉�C�g�̃f�[�^���g����悤�ɂȂ�
#include "../inc_KdCommon.hlsli"
// ���ʃf�[�^
#include "inc_KdModelShader.hlsli"
//==================================
// ���_�V�F�[�_�[
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV���W�������Ă���
	float4 color : COLOR, // ���_�F�������Ă���
	float3 normal : NORMAL // �@���������Ă���
)
{
	VSOutput Out;

	// ���_���W��@�������ɁA�������炷
	pos.xyz = pos.xyz + normal * 0.03f;

	// 3D���_���W��2D�֕ϊ�
	Out.Pos = mul(pos, g_mW); // ���[���h�s��ŕϊ����Ă���A
	Out.wPos = Out.Pos.xyz; // ���[���h���W������Ă���
	Out.Pos = mul(Out.Pos, g_mV); // �J�����̋t�s��(�r���[�s��)�ŕϊ����āA
	Out.Pos = mul(Out.Pos, g_mP); // �ˉe�s��ŕϊ�

	// UV�͂��̂܂܃s�N�Z���V�F�[�_�[��(����͖��g�p)
	Out.UV = uv;

	// ���_�F�͂��̂܂܃s�N�Z���V�F�[�_�[��(����͖��g�p)
	Out.Color = color;

	// �@�������[���h���W�n�֕ϊ�(����͖��g�p)
	Out.wN = normalize(mul(normal, (float3x3)g_mW));

	return Out;
}