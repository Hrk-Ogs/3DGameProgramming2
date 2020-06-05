#include "../inc_KdCommon.hlsli"
#include "inc_KdStandardShader.hlsli"


// �{�[���s��o�b�t�@
struct BoneData
{
    row_major float4x4 mat;
};
StructuredBuffer<BoneData> g_sbBones : register(t100);


//================================
// �X�L�����b�V���p ���_�V�F�[�_
//================================
VSOutput main(  float4 pos : POSITION,  // ���_���W
                float2 uv : TEXCOORD0,  // �e�N�X�`��UV���W
                float3 tangent : TANGENT,// �ڐ��x�N�g��
                float3 normal : NORMAL,  // �@���x�N�g��
                float4 color : COLOR,
                uint4 skinIndex : SKININDEX,
                float4 skinWeight : SKINWEIGHT
){

    //------------------------------
    // �X�L�j���O����
    // ���̒��_�ɉe�����Ă��镡���̃{�[���̍s����g�p����
    // pos��normal�Ȃǂ�ϊ�����
    //------------------------------
    row_major float4x4 mBones = 0; // �ŏI�I�ȃ��[���h�s��
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        mBones += g_sbBones[skinIndex[i]].mat * skinWeight[i];
    }
    // �{�[���s��ŕϊ�(�܂����[�J�����W�n)
    pos = mul(pos, mBones);
    tangent = mul(tangent, (float3x3) mBones);
    normal = mul(normal, (float3x3) mBones);
    //------------------------------

    VSOutput Out;

    // ���W�ϊ�
    Out.Pos = mul(pos, g_mW);       // ���[�J�����W�n -> ���[���h���W�n�֕ϊ�
        Out.wPos = Out.Pos.xyz;
    Out.Pos = mul(Out.Pos, g_mV);   // ���[���h���W�n -> �r���[���W�n�֕ϊ�
    Out.Pos = mul(Out.Pos, g_mP);   // �r���[���W�n -> �ˉe���W�n�֕ϊ�

    // �@��
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));
    Out.wN = normalize(mul(normal, (float3x3)g_mW));
    Out.wB = normalize(cross(Out.wN, Out.wT));

    // ���_�F
    Out.Color = color;

    // UV���W
    Out.UV = uv * g_UVTiling + g_UVOffset;

    // �o��
    return Out;
}
