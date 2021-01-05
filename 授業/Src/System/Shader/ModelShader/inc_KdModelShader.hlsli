// �萔�o�b�t�@
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;		// ���[���h�s��
	int g_lightEnable;				// ���C�g�L��
	int g_FogEnable;				// �t�H�O�L��

	float4 g_mulMaterialColor; // ��Z�F

	// UV����
	float2 g_UVOffset; // UV�I�t�Z�b�g�l
	float2 g_UVTiling; // UV�{��

	float g_refractiveIndex; // ���ܗ�
};

// �}�e���A���f�[�^���\���̂ɓZ�߂�
struct Material
{
	float4 BaseColor;		// �x�[�X�E
	float3 Emissive;		// ���Ȕ����F
	float Metallic;			// �����x
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
	float3 wN : TEXCOORD2;	// ���[���h�@��
	float3 wPos : TEXCOORD3;	// ���[���h���W

	float3 wT : TEXCOORD4;	// ���[���h�ڐ�
	float3 wB : TEXCOORD5;	// ���[���h�]�@��
};
