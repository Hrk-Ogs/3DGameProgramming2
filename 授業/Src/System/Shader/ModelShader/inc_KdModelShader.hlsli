// �萔�o�b�t�@
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;		// ���[���h�s��
};

// �}�e���A���f�[�^���\���̂ɓZ�߂�
struct Material
{
	float4 BaseColor;		// �x�[�X�E
	float3 Emissive;		// ���Ȕ����F
	float Matallic;			// �����x
	float Roughness;		// �e��
};

// �}�e���A���萔�o�b�t�@
cbuffer cbMaterial:register(b1)
{
	Material g_material;
}

// ���_�V�F�[�_����o�͂���f�[�^
struct VSOutput
{
	float4 Pos : SV_Position;	// �ˉe���W
	float2 UV : TEXCOORD0;		// �e�N�X�`��UV���W
	float4 Color : TEXCOORD1;	// ���_�F
};
