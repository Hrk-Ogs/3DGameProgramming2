#include "inc_PostProcess.hlsli"

// �萔�o�b�t�@
cbuffer cbWave : register(b0)
{
	float2 g_AddPowerUV; // �Ӑ}�I�ɗ͂���������W
	float g_AddPowerRadius;// ���̔��a
	float g_AddPower; // �З�
};

// ���̓e�N�X�`��(�����}�b�v)
Texture2D g_heightTex : register(t0);

// �T���v��(Linear��� Wrap���[�h)
SamplerState g_ss : register(s0);

//�g �����}�b�v�X�V�s�N�Z���V�F�[�_�[
float4 main(VSOutput In) : SV_Target0
{
	// �摜�̕��ƍ������Q�b�g(�s�N�Z��)
	float w, h;
	g_heightTex.GetDimensions(w, h);
	// �s�N�Z�� -> �e�N�Z���ϊ�
	float tw = 1.0 / w;
	float th = 1.0 / h;

	// ���S�_�Ɨאڂ���4�_���T���v�����O
	float4 t0 = g_heightTex.Sample(g_ss, In.UV);
	float4 t1 = g_heightTex.Sample(g_ss, In.UV + float2(-tw, 0));
	float4 t2 = g_heightTex.Sample(g_ss, In.UV + float2(tw, 0));
	float4 t3 = g_heightTex.Sample(g_ss, In.UV + float2(0, th));
	float4 t4 = g_heightTex.Sample(g_ss, In.UV + float2(0, -th));

	// ���̃s�N�Z���ɂ������Ă����
	// ��0.03�̓o�l�̋���
	float F = (t1.r + t2.r + t3.r + t4.r - t0.r * 4) * 0.03;

	// �͂����ɁA�������X�V
	t0.g += F;

	// ���������ɍ������X�V
	t0.r += t0.g;

	// �Ӑ}�I�ɗ͂�������
	// ��distance(p1, p2) �c p1�`p2�̋�����Ԃ�
	if (distance(In.UV, g_AddPowerUV) < g_AddPowerRadius)
	{
	t0.g += g_AddPower;
	}

	// R:���� G:����
	return float4(t0.r, t0.g, 0, 1);
}