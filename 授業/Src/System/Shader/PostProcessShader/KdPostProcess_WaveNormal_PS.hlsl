#include "inc_PostProcess.hlsli"

// ���̓e�N�X�`��(�����}�b�v)
Texture2D g_heightTex : register(t0);

// �T���v��(Linear��� Clamp���[�h)
SamplerState g_ss : register(s0);

// �g �@���}�b�v�X�V�s�N�Z���V�F�[�_�[
float4 main(VSOutput In) : SV_Target0
{
	// �摜�̕��ƍ������Q�b�g(�s�N�Z��)
	float w, h;
	g_heightTex.GetDimensions(w, h);
	// �s�N�Z�� -> �e�N�Z���ϊ�
	float tw = 1.0 / w;
	float th = 1.0 / h;

	// ���S�_�Ɨאڂ���4�_���T���v�����O
	float4 t1 = g_heightTex.Sample(g_ss, In.UV + float2(-tw, 0));
	float4 t2 = g_heightTex.Sample(g_ss, In.UV + float2(tw, 0));
	float4 t3 = g_heightTex.Sample(g_ss, In.UV + float2(0, -th));
	float4 t4 = g_heightTex.Sample(g_ss, In.UV + float2(0, th));

	// ���E�A�㉺�̍�����@���x�N�g�������߂�
	float3 normal;
	normal.x = (t1.r - t2.r) * 0.5f;
	normal.y = (t3.r - t4.r) * 0.5f;
	normal.z = 1;
	normal = normalize(normal);

	// -1�`1����0�`1�֕ϊ����o��
	normal = normal * 0.5 + 0.5;

	return float4(normal, 1);
}