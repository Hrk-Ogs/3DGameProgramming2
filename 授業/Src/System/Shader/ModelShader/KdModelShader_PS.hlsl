// �J�����⃉�C�g�̃f�[�^���g����悤�ɂȂ�
#include "../inc_KdCommon.hlsli"
// ���ʃf�[�^
#include "inc_KdModelShader.hlsli"

// �e�N�X�`���it0�Ƃ�t1�Ƃ����APSS����ShaderResources�ŃZ�b�g�����Ƃ��̃X���b�g�ԍ��j
Texture2D g_baseTex : register(t0);		// �x�[�X�J���[�e�N�X�`���[
Texture2D g_emissiveTex : register(t1);	// �G�~�b�V�u�e�N�X�`��
Texture2D g_mrTex : register(t2);		// ���^���b�N/���t�l�X�e�N�X�`��
Texture2D g_normalTex : register(t3);	// �@���}�b�v

// �T���v��(�e�N�X�`������F���擾����Ƃ��Ɏg���j
SamplerState g_ss : register(s0);

//======================================
// �s�N�Z���V�F�[�_�[
//======================================
float4 main(VSOutput In) :SV_Target0
{
	//----------------------------------------
	// �ގ��F
	//----------------------------------------
	// �e�N�X�`���F
	float4 texColor = g_baseTex.Sample(g_ss,In.UV);
	// �}�e���A���̐F�ƒ��_�F�ƍ���
	float4 baseColor = texColor * g_material.BaseColor * In.Color;

	return baseColor;
}
