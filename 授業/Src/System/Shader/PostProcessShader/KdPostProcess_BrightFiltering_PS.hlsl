#include "inc_PostProcess.hlsli"

// ���̓e�N�X�`��
Texture2D g_inputTex : register(t0);
// �T���v��(Linear��� Clamp���[�h)
SamplerState g_ss : register(s3);

// ���P�x���o�s�N�Z���V�F�[�_�[
float4 main(VSOutput In) : SV_Target0
{
	// �e�N�X�`���̐F�擾
	float4 texColor = g_inputTex.Sample(g_ss, In.UV);

	// �e�N�X�`���̐F����P������
	texColor.rgb = max(0, texColor.rgb - 1);
	// max(a ,b) �c a��b�A�傫������Ԃ�

	// �c�����F�����P�x�F�Ȃ̂ŁA������o�͂���
	return texColor;
}