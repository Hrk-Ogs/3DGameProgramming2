//=====================================================
//
// �S�V�F�[�_����
//
//=====================================================

//------------------------------
// �萔�o�b�t�@(�J����)
//------------------------------
cbuffer cbCamera : register(b7)
{
    // �J�������
    row_major float4x4  g_mV; // �r���[�ϊ��s��
    row_major float4x4  g_mP; // �ˉe�ϊ��s��
    float3              g_CamPos; // �J�������W

	// �t�H�O
    int     g_DistanceFogEnable; // �t�H�O�L��/�����t���O
    float3  g_DistanceFogColor; // �t�H�O�F
    float   g_DistanceFogDensity; // �t�H�O������
};

//------------------------------
// �萔�o�b�t�@(���C�g)
//------------------------------

// ���s���̃f�[�^
struct DirectionalLight
{
	float3 Dir; // ���̕���
	float3 Color; // ���̐F
    float tmp;
};

// �|�C���g���C�g�̃f�[�^
struct PointLight
{
	float3 Color; // �F
	float Radius; // ���a
	float3 Pos; // ���W
    float tmp;
};

// �X�|�b�g���C�g�̃f�[�^
struct SpotLight
{
    float3 Dir; // ���̕���
    float Range; // �͈�
    float3 Color; // �F
    float InnerCorn; // �����̊p�x
    float3 Pos; // ���W
    float OuterCorn; // �O���̊p�x
};

cbuffer cbLight : register(b8)
{
	int g_DL_Cnt = 0; // ���s���̐�
	int g_PL_Cnt = 0; // �_���̐�
    int g_SL_Cnt = 0; // �W�����̐�
    
    //-------------------------
    // ����
    //-------------------------
    float3  g_AmbientLight;

    //-------------------------
    // ���s��
    //-------------------------

    DirectionalLight    g_DL[3];    // �S�Ă̕��s���f�[�^

    //-------------------------
    // �_��
    //-------------------------

	PointLight g_PL[100];           // �S�Ă̓_���f�[�^

    //-------------------------
    // �W����
    //-------------------------

    SpotLight g_SL[30];             // �S�Ă̏W�����f�[�^
    
};
