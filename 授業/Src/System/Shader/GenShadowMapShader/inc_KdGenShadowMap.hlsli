// �萔�o�b�t�@
cbuffer cbObject : register(b0)
{
	row_major float4x4 g_mW; // ���[���h�s��
	row_major float4x4 g_mLightVP; // ���C�g�J�����̃r���[�s�� * �ˉe�s��
};

// VS�o�͗p�\����
struct VSOutput
{
	float4 Pos : SV_Position; // �ˉe���W
	float2 UV : TEXCOORD0; // UV
	float4 wvpPos : TEXCOORD1; // �ˉe���W
};