#include "inc_KdGenShadowMap.hlsli"

Texture2D g_baseTex : register(t0);
SamplerState g_ss : register(s0);

// �s�N�Z���V�F�[�_
float4 main(VSOutput In) : SV_Target0
{
	// �e�N�X�`������F�擾
	float4 texCol = g_baseTex.Sample(g_ss, In.UV);
	// �A���t�@�e�X�g
	if (texCol.a < 0.1)
	{
	discard;
	}

	// �[�x�����߂�R�v�f�Ƃ��ďo��(R�ȊO��GBA�͎g�p���Ȃ��̂ŁA0,0,1�Ƃł����Ă����܂��B)
	return float4(In.wvpPos.z / In.wvpPos.w, 0, 0, 1);
}