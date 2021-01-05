// �J�����⃉�C�g�̃f�[�^���g����悤�ɂȂ�
#include "../inc_KdCommon.hlsli"
// ���ʃf�[�^
#include "inc_KdModelShader.hlsli"

// �e�N�X�`���it0�Ƃ�t1�Ƃ����APSS����ShaderResources�ŃZ�b�g�����Ƃ��̃X���b�g�ԍ��j
Texture2D g_baseTex : register(t0);		// �x�[�X�J���[�e�N�X�`���[
Texture2D g_emissiveTex : register(t1);	// �G�~�b�V�u�e�N�X�`��
Texture2D g_mrTex : register(t2);		// ���^���b�N/���t�l�X�e�N�X�`��
Texture2D g_normalTex : register(t3);	// �@���}�b�v
Texture2D g_toonTex : register(t4); // �g�D�[���e�N�X�`��

// �����_�����O���ꂽ�V�[�����
Texture2D g_sceneTex : register(t10);

// IBL
TextureCube g_IBLTex : register(t11); // IBL�e�N�X�`��

// �T���v��(�e�N�X�`������F���擾����Ƃ��Ɏg���j
SamplerState g_ss : register(s0);
SamplerState g_clampSS : register(s1); // Clamp�ݒ�̃T���v��(KdShaderManager��s1�ɃZ�b�g���Ă��܂�)

//======================================
// �s�N�Z���V�F�[�_�[
//======================================
// BlinnPhong NDF
// �ElightDir �c ���C�g�̕���
// �EvCam �c �s�N�Z������J�����ւ̕���
// �Enormal �c �@��
// �EspecPower �c ���˂̉s��
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // �J�����̊p�x��(0�`1)
	float spec = pow(NdotH, specPower);
	// ���K��Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

float4 main(VSOutput In) :SV_Target0
{
	// �J�����ւ̕���
	//float3 vCam = normalize(g_CamPos - In.wPos);
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam);			// �J�����ւ̋���
	vCam = normalize(vCam);

	// �@���}�b�v���琳�K��
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;
	// 0-1����-1�`1�֕�������
	wN = wN * 2.0 - 1.0;

	// �@���s��i��]�s��j�ŕϊ�����
	float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};
	wN = mul(wN, mTBN);		// TBN�ŕϊ�

	wN = normalize(wN);		// ���K��

	//----------------------------------------
	// �ގ��F
	//----------------------------------------
	// �e�N�X�`���F
	float4 texColor = g_baseTex.Sample(g_ss,In.UV);
	// �}�e���A���̐F�ƒ��_�F�ƍ���
	float4 baseColor = texColor * g_material.BaseColor * In.Color * g_mulMaterialColor;

	// RoughnessMetallicMap����F�擾
	float4 mr = g_mrTex.Sample(g_ss, In.UV);

	// �e��
	float roughness = g_material.Roughness * mr.g;
	// ������
	float metallic = g_material.Metallic * mr.b;

	// roughness���t�]�����u���炩�v���ɂ���
	// roughness���Q�悵�Ă���̂́A���̕������o�I�Ƀ��A���ɂȂ�炵���̂�
	float smoothness = 1.0 - roughness * roughness;
	float specPower = pow(2, 11 * smoothness); // 1�`2048


	//------------------------------------------
	// ���C�e�B���O
	//------------------------------------------
	// �ŏI�I�ȐF
	float3 color = 0;

	// ���C�g�L����
	if (g_lightEnable)
	{
		// �ގ��̊g�U�F ������قǍގ��̐F�ɂȂ�A�����قǊg�U�F�͖����Ȃ�
		float3 baseDiffuse = lerp(baseColor.rgb, float3(0, 0, 0), metallic);
		//�ގ��̔��ːF ������قǌ��̐F�����̂܂ܔ��˂��A�����قǍގ��̐F�����
		float3 baseSpecular = lerp(0.04, baseColor.rgb, metallic);

		// ���s���͕������邽�߁A�g�p���Ă���͂������v�Z����
		for (int di = 0; di < g_DL_Cnt; di++)
		{
			//--------------------------------------
			// �g�U��
			//--------------------------------------
			// Lambert���f��
			float lightDiffuse = dot(-g_DL[di].Dir, wN);	// ���ςŊp�x�̍������߂�
			lightDiffuse = saturate(lightDiffuse); // �}�C�i�X�͂܂����̂ŁA0�`1���ɐ؂�l�߂�

			// �u���邳�v��U���W�Ƃ��āA�g�D�[���e�N�X�`������ŏI�I�Ȗ��邳�f�[�^���擾
			float3 toonColor = g_toonTex.Sample(g_clampSS, float2(lightDiffuse, 0.5)).rgb;

			// �����I�ɐ��������邽�߁A���K������
			toonColor /= 3.1415926535;

			// ��яo������v�Z���A���ʂɉ��Z����
			// (���̐F) * �ގ��̐F * ������
			color += (g_DL[di].Color * toonColor) * baseDiffuse * baseColor.a;

			//--------------------------------------
			// ���ˌ�
			//--------------------------------------
			// Blinn-Phong
			float spec = BlinnPhong(g_DL[di].Dir, vCam, wN, specPower);

			// ���˂�������v�Z���A���ʂɉ��Z����
			// (���̐F) * �������4%���˂炵�� * ������
			color += (g_DL[di].Color * spec) * baseSpecular * baseColor.a;

		}

		//----------------------------
		// ����
		//----------------------------
		color += g_AmbientLight * baseColor.rgb * baseColor.a;

		//------------------
		// IBL
		//------------------
		// �g�U��
		float3 envDiff = g_IBLTex.SampleLevel(g_ss, wN, 8).rgb; // �g�U�\�����邽�߁A����𑜓x�̉摜���g�p����
		color += envDiff * baseDiffuse.rgb / 3.141592;

		// ���ˌ�
		float3 vRef = reflect(-vCam, wN); // ���˃x�N�g�������߂�
		float3 envSpec = g_IBLTex.SampleLevel(g_ss, vRef, roughness * 8).rgb; // �e���قǒ�𑜓x�̉摜���g�p����
		color += envSpec * baseSpecular;
		}
	// ���C�g������
	else
	{
		// �ގ��̐F�����̂܂܎g�p
		color = baseColor.rgb;
	}

	// ���ܕ\��
	if (g_refractiveIndex < 1.0)
	{
		// �J�����������g�p���ċ��܃x�N�g�������߂�
		float3 vRefract = refract(-vCam, wN, g_refractiveIndex);
		float3 v = vRefract - (-vCam);
		// ���̃s�N�Z���̉�ʏ�ł�UV���W�����߂�
		// ���ܕ����ɂ������ړ�������3D���W���A�ˉe���W�֕ϊ�
		float4 posProj = mul(float4(In.wPos + v * 0.5f, 1), g_mV);
		posProj = mul(posProj, g_mP);
		posProj.xyz /= posProj.w;
		// �ˉe���W����UV���W�֕ϊ�
		float2 uv = posProj.xy * float2(1, -1);
		uv = uv * 0.5 + 0.5;
		// �V�[���摜����uv�̈ʒu�̐F���擾
		float4 sceneColor = g_sceneTex.Sample(g_clampSS, uv);
		// �����̐F�Ɣw�i�̐F���A���t�@�u�����h
		// �� n = lerp(a, b, t) �c a�`b�Ԃ�t�̈ʒu�̒l�����߂�
		color = lerp(sceneColor.rgb, color.rgb, baseColor.a);
		baseColor.a = 1; // �s�����ɂ���
	}

	//------------------------------------
	// �����t�H�O
	//------------------------------------
	if (g_DistanceFogEnable && g_FogEnable)
	{
		// �w���t�H�O1�i�߂��j�`0�i�����j
		float f = saturate(1.0 / exp(camDist * g_DistanceFogDensity));
		// �K�p
		color.rgb = lerp(g_DistanceFogColor, color.rgb, f);
	}

	return float4(color, baseColor.a);
}
