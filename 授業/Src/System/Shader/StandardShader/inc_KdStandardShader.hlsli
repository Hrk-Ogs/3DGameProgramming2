
//===========================================
//
// �萔�o�b�t�@
//  �Q�[���v���O���������玝���Ă���f�[�^
//
//===========================================

// �萔�o�b�t�@(�I�u�W�F�N�g�P�ʍX�V)
cbuffer cbObject : register(b0)
{
    // �I�u�W�F�N�g���
    row_major float4x4 g_mW; // ���[���h�ϊ��s��

    // UV����
    float2  g_UVOffset;
    float2  g_UVTiling;

    // ���C�g�L��
    int     g_LightEnable;
    // �t�H�O�L��
    int     g_FogEnable;
    // �A���t�@�e�X�g��臒l
    float   g_AlphaTestThreshold;
    // ��Z�F
    float4  g_mulMaterialColor;
};

//------------------------------
// �萔�o�b�t�@(�}�e���A��)
//------------------------------

// �}�e���A���\����
struct Material
{
    float4 BaseColor;   // �x�[�X�F
    float3 Emissive;    // ���Ȕ����F
    float Metallic;     // �����x
    float Roughness;    // �e��
};

cbuffer cbMaterial : register(b1)
{
    Material g_Material;
};

//------------------------------
// ���_�V�F�[�_����o�͂���f�[�^
//------------------------------
struct VSOutput
{
    float4 Pos : SV_Position;   // �ˉe���W
    float2 UV : TEXCOORD0;      // UV���W
    float3 wT : TEXCOORD1;      // ���[���h�ڐ�
    float3 wB : TEXCOORD2;      // ���[���h�]�@��
    float3 wN : TEXCOORD3;      // ���[���h�@��
    float4 Color : TEXCOORD4;   // �F
    float3 wPos : TEXCOORD5;    // ���[���h3D���W
};
