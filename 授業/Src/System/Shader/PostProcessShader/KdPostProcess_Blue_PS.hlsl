#include "inc_PostProcess.hlsli"

// �萔�o�b�t�@
cbuffer cb : register(b0)
{
	float4 g_Offset[31]; // xy:�I�t�Z�b�g���W z:�d��
};

// ���̓e�N�X�`��
Texture2D g_InputTex : register(t0);
// �T���v��(Linear��� Clamp���[�h)
SamplerState g_ss : register(s3);

// �ڂ����s�N�Z���V�F�[�_�[
float4 main(VSOutput In) : SV_Target0
{
	float3 color = 0;
	// 31�s�N�Z���Ԃ���u�����h����
	for (int i = 0; i < 31; i++)
	{
		// UV�̈ʒu����Offset��xy�������炵���ʒu�̐F���擾���A�d�݂��|���đ�������
		color += g_InputTex.Sample(g_ss, In.UV + g_Offset[i].xy).rgb * g_Offset[i].z;
	}

	return float4(color, 1);
}