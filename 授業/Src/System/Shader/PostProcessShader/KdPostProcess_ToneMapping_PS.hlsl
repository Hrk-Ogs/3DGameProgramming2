#include "inc_PostProcess.hlsli"

// ���̓e�N�X�`��
Texture2D g_inputTex : register(t0);
// �T���v��(Linear��� Clamp���[�h)
SamplerState g_ss : register(s3);

// ACES Filmic ToneMapping
// HDR�̐F��LDR�ɕϊ�����
// �Ex �c HDR�J���[(RGB)
// �E�߂�l �c ToneMap��̐F
float3 ACESFilmicToneMapping(float3 x)
{
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;

	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

// �g�[���}�b�s���O �s�N�Z���V�F�[�_�[
float4 main(VSOutput In) : SV_Target0
{
	// �����p
	const float linearExposure = 0.7;

	// �e�N�X�`������F���擾�����k����
	float4 texColor = g_inputTex.Sample(g_ss, In.UV);
	texColor.rgb = ACESFilmicToneMapping(texColor.rgb * linearExposure);

	return texColor;
}