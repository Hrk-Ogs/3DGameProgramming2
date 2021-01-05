#include "inc_PostProcess.hlsli"

// �萔�o�b�t�@
cbuffer cb : register(b0)
{
	float4 g_Color; // �����F
};

// ���̓e�N�X�`��
Texture2D g_inputTex : register(t0);
// �T���v��(Linear��� Clamp���[�h)
SamplerState g_ss : register(s3);

// �s�N�Z���V�F�[�_�[
float4 main(VSOutput In) : SV_Target0
{
	// �e�N�X�`������F���擾
	float4 texColor = g_inputTex.Sample(g_ss, In.UV);
	// �萔�o�b�t�@�̐F���������o��
	return texColor * g_Color;
}