#include "../inc_KdCommon.hlsli"
#include "inc_KdStandardShader.hlsli"

// �e�N�X�`��
Texture2D g_baseTex : register(t0);         // �x�[�X�J���[�e�N�X�`��
Texture2D g_emissiveTex : register(t1);     // �G�~�b�V�u�e�N�X�`��
Texture2D g_mrTex : register(t2);           // ���^���b�N/���t�l�X�e�N�X�`��
Texture2D g_normalTex : register(t3);       // �@���}�b�v


// �T���v��
SamplerState g_ss : register(s0);

/*
// Unity��Build-In Shader�ł̎�@
// Roughness����SpecularPower���Z�o
float RoughnessToSpecPower(float roughness)
{
    float trueRoughness = roughness * roughness; // �w�p�I�ɐ��������t�l�X�����߂�
    float sq = max(0.0001, trueRoughness * trueRoughness);
    float n = (2.0 / sq) - 2.0;
    // Roughness��1.0��NdotH���[���̏ꍇ�ɔ�������\���̂���pow(0,0)�̃P�[�X��h�~
    n = max(n, 0.0001);
    return n;
}
*/

// BlinnPhong NDF
// �ElightDir    �c ���C�g�̕���
// �EvCam        �c �s�N�Z������J�����ւ̕���
// �Enormal      �c �@��
// �EspecPower   �c ���˂̉s��
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
    float3 H = normalize(-lightDir + vCam);
    float NdotH = saturate(dot(normal, H)); // �J�����̊p�x��(0�`1)
    float spec = pow(NdotH, specPower);

    // ���K��Blinn-Phong
    return spec * ((specPower + 2) / (2 * 3.1415926535));
}

//================================
// �s�N�Z���V�F�[�_
//================================
float4 main(VSOutput In) : SV_Target0
{
    // �J�����ւ̕���
    float3 vCam = g_CamPos - In.wPos;
    float camDist = length(vCam);       // �J���� - �s�N�Z������
    vCam = normalize(vCam);
    
    // �@�����K��
//    float3 wN = normalize(In.wN);
    // �@���}�b�v
    float3x3 mTBN = {
        normalize(In.wT),
        normalize(In.wB),
        normalize(In.wN)
    };

    float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;
    wN = wN * 2.0 - 1.0;
    wN = mul(wN, mTBN);

    //------------------------------------------
    // �ގ��F
    //------------------------------------------

    float4 mr = g_mrTex.Sample(g_ss, In.UV);
    // ������
    float metallic = mr.b * g_Material.Metallic;
    // �e��
    float roughness = mr.g * g_Material.Roughness;


    // �ގ��̐F
    float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_Material.BaseColor * In.Color * g_mulMaterialColor;

    // �A���t�@�e�X�g
    if (baseColor.a <= g_AlphaTestThreshold)discard;

    // ���t�l�X����ABlinn-Phong�p��SpecularPower�����߂�
    // Call of Duty : Black Ops��FarCry3�ł̂ł̎�@���g�p
    // specPower = 2^(13*g)�@�O���X�lg = 0�`1��1�`8192�̃X�y�L�������x�����߂�
    // �Q�l�Fhttps://hanecci.hatenadiary.org/entry/20130511/p1
    float smoothness = 1.0 - roughness; // ���t�l�X���t�]�����u���炩�v���ɂ���
    float specPower = pow(2, 11 * smoothness); // 1�`2048
    
    //------------------------------------------
    //
    // ���C�e�B���O
    //
    //------------------------------------------
    // �ŏI�I�ȐF
    float3 color = 0;

    // ���C�g�L����
    if(g_LightEnable)
    {
        // �ގ��̊g�U�F�@������قǍގ��̐F�ɂȂ�A�����قǊg�U�F�͖����Ȃ�
        const float3 baseDiffuse = lerp(baseColor.rgb, float3(0, 0, 0), metallic);
        // �ގ��̔��ːF�@������قǌ��̐F�����̂܂ܔ��˂��A�����قǍގ��̐F�����
        const float3 baseSpecular = lerp(0.04, baseColor.rgb, metallic);

        //------------------
        // ���s��
        //------------------
        for (int di = 0; di < g_DL_Cnt; di++)
        {
            // Diffuse(�g�U��) ���K��Lambert���g�p
            {
                // ���̕����Ɩ@���̕����Ƃ̊p�x�������̋����ɂȂ�
                float lightDiffuse = dot(-g_DL[di].Dir, wN);
                lightDiffuse = saturate(lightDiffuse);  // �}�C�i�X�l��0�ɂ���@0(��)�`1(��)�ɂȂ�

                // ���K��Lambert
                lightDiffuse /= 3.1415926535;

                // ���̐F * �ގ��̊g�U�F * ������
                color += (g_DL[di].Color * lightDiffuse) * baseDiffuse * baseColor.a;
            }

            // Specular(���ːF) ���K��Blinn-Phong NDF���g�p
            {


                // Unity��Build-In Shader�ł̎�@
                //float spec = RoughnessToSpecPower(roughness);

                // Blinn-Phong NDF
                float spec = BlinnPhong(g_DL[di].Dir, vCam, wN, specPower);

                // ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ������
                color += (g_DL[di].Color * spec) * baseSpecular * 0.5 * baseColor.a;
            }
        }

        //------------------
        // �_��
        //------------------
		for (int pi = 0; pi < g_PL_Cnt; pi++)
		{
			float3 dir = g_PL[pi].Pos - In.wPos;
			float dist = length(dir);
			dir /= dist;
            
			if (dist < g_PL[pi].Radius)
			{
                // ���a�����ƂɁA�����̔䗦�����߂�
				float atte = 1.0 - saturate(dist / g_PL[pi].Radius);
                // �t�Q��̖@��
				atte *= atte;
                
                // Diffuse(�g�U��) ���K��Lambert���g�p
			    {
                    // ���̕����Ɩ@���̕����Ƃ̊p�x�������̋����ɂȂ�
					float lightDiffuse = dot(dir, wN);
				    lightDiffuse = saturate(lightDiffuse); // �}�C�i�X�l��0�ɂ���@0(��)�`1(��)�ɂȂ�
                    
					lightDiffuse *= atte;   // ����

                    // ���K��Lambert
				    lightDiffuse /= 3.1415926535;

                    // ���̐F * �ގ��̊g�U�F * ������
                    color += (g_PL[pi].Color * lightDiffuse) * baseDiffuse * baseColor.a;
                }
                
                // Specular(���ːF) ���K��Blinn-Phong NDF���g�p
                {

                    // Blinn-Phong NDF
                    float spec = BlinnPhong(-dir, vCam, wN, specPower);

                    spec *= atte; // ����
                    
                    // ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ������
                    color += (g_PL[pi].Color * spec) * baseSpecular * 0.5 * baseColor.a;
                }
			}
		}

        //------------------
        // �W����
        //------------------
        for (int si = 0; si < g_SL_Cnt; si++)
        {
            float3 dir = g_SL[si].Pos - In.wPos;
            float dist = length(dir);
            dir /= dist;
            
            // �����͈͓�����
            if (dist < g_SL[si].Range)
            {
                float cosA = dot(-g_SL[si].Dir, dir);
                float cosOutner = cos(g_SL[si].OuterCorn / 2.0);

                // �~���͈͓�����
                if (cosA > cosOutner)
                {
                    // ���a�����ƂɁA�����̔䗦�����߂�
                    float atte = 1.0 - saturate(dist / g_SL[si].Range);
                    // �t�Q��̖@��
                    atte *= atte;
                    
                    // ���a�ƊO�a�̌���
                    float cosInner = cos(g_SL[si].InnerCorn / 2.0);
                    atte *= (cosA - cosOutner) / (cosInner - cosOutner);

                    // Diffuse(�g�U��) ���K��Lambert���g�p
			        {
                        // ���̕����Ɩ@���̕����Ƃ̊p�x�������̋����ɂȂ�
                        float lightDiffuse = dot(dir, wN);
                        lightDiffuse = saturate(lightDiffuse); // �}�C�i�X�l��0�ɂ���@0(��)�`1(��)�ɂȂ�
                    
                        lightDiffuse *= atte; // ����

                        // ���K��Lambert
                        lightDiffuse /= 3.1415926535;

                        // ���̐F * �ގ��̊g�U�F * ������
                        color += (g_SL[si].Color * lightDiffuse) * baseDiffuse * baseColor.a;
                    }
                
                    // Specular(���ːF) ���K��Blinn-Phong NDF���g�p
                    {

                        // Blinn-Phong NDF
                        float spec = BlinnPhong(-dir, vCam, wN, specPower);

                        spec *= atte; // ����
                    
                        // ���̐F * ���ˌ��̋��� * �ގ��̔��ːF * ������
                        color += (g_SL[si].Color * spec) * baseSpecular * 0.5 * baseColor.a;
                    }
                }
            }
        }
        
        //------------------
        // ����
        //------------------
        color += g_AmbientLight * baseColor.rgb * baseColor.a;

        //------------------
        // �G�~�b�V�u
        //------------------
        color += g_emissiveTex.Sample(g_ss, In.UV).rgb * g_Material.Emissive;
    }
    // ���C�g������
    else
    {
        // �ގ��̐F�����̂܂܎g�p
        color = baseColor.rgb;
    }

    //------------------------------------------
    // �����t�H�O
    //------------------------------------------
    if (g_DistanceFogEnable && g_FogEnable)
    {
		// �t�H�O 1(�߂�)�`0(����)
        float f = saturate(1.0 / exp(camDist * g_DistanceFogDensity));
        // �K�p
        color.rgb = lerp(g_DistanceFogColor, color.rgb, f);
    }

    
    //------------------------------------------
    // �o��
    //------------------------------------------
    return float4(color, baseColor.a);

}
