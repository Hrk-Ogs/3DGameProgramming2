// ƒJƒƒ‰‚âƒ‰ƒCƒg‚Ìƒf[ƒ^‚ªg‚¦‚é‚æ‚¤‚É‚È‚é
#include "../inc_KdCommon.hlsli"

<<<<<<< HEAD
// ƒeƒNƒXƒ`ƒƒit0‚Æ‚©t1‚Æ‚©‚ªAPSS‚¦‚”ShaderResources‚ÅƒZƒbƒg‚µ‚½‚Æ‚«‚ÌƒXƒƒbƒg”Ô†j
Texture2D g_baseTex : register(t0);		// ƒx[ƒXƒJƒ‰[ƒeƒNƒXƒ`ƒƒ[
Texture2D g_emissiveTex : register(t1);	// ƒGƒ~ƒbƒVƒuƒeƒNƒXƒ`ƒƒ
Texture2D g_mrTex : register(t2);		// ƒƒ^ƒŠƒbƒN/ƒ‰ƒtƒlƒXƒeƒNƒXƒ`ƒƒ
Texture2D g_normalTex : register(t3);	// –@üƒ}ƒbƒv
Texture2D g_toonTex : register(t4); // ƒgƒD[ƒ“ƒeƒNƒXƒ`ƒƒ

// ƒŒƒ“ƒ_ƒŠƒ“ƒO‚³‚ê‚½ƒV[ƒ“‰æ–Ê
Texture2D g_sceneTex : register(t10);

// IBL
TextureCube g_IBLTex : register(t11); // IBLƒeƒNƒXƒ`ƒƒ

// ƒTƒ“ƒvƒ‰(ƒeƒNƒXƒ`ƒƒ‚©‚çF‚ğæ“¾‚·‚é‚Æ‚«‚Ég‚¤j
SamplerState g_ss : register(s0);
SamplerState g_clampSS : register(s1); // Clampİ’è‚ÌƒTƒ“ƒvƒ‰(KdShaderManager‚Ås1‚ÉƒZƒbƒg‚µ‚Ä‚¢‚Ü‚·)
=======
>>>>>>> parent of fcd683a... ã€3Dpro2ã€‘

//======================================
// ƒsƒNƒZƒ‹ƒVƒF[ƒ_[
//======================================
<<<<<<< HEAD
// BlinnPhong NDF
// ElightDir c ƒ‰ƒCƒg‚Ì•ûŒü
// EvCam c ƒsƒNƒZƒ‹‚©‚çƒJƒƒ‰‚Ö‚Ì•ûŒü
// Enormal c –@ü
// EspecPower c ”½Ë‚Ì‰s‚³
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // ƒJƒƒ‰‚ÌŠp“x·(0`1)
	float spec = pow(NdotH, specPower);
	// ³‹K‰»Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

float4 main(VSOutput In) :SV_Target0
{
	// ƒJƒƒ‰‚Ö‚Ì•ûŒü
	//float3 vCam = normalize(g_CamPos - In.wPos);
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam);			// ƒJƒƒ‰‚Ö‚Ì‹——£
	vCam = normalize(vCam);

	// –@üƒ}ƒbƒv‚©‚ç³‹K‰»
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;
	// 0-1‚©‚ç-1`1‚Ö•œŒ³‚·‚é
	wN = wN * 2.0 - 1.0;

	// –@üs—ñi‰ñ“]s—ñj‚Å•ÏŠ·‚·‚é
	float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};
	wN = mul(wN, mTBN);		// TBN‚Å•ÏŠ·

	wN = normalize(wN);		// ³‹K‰»

	//----------------------------------------
	// Ş¿F
	//----------------------------------------
	// ƒeƒNƒXƒ`ƒƒF
	float4 texColor = g_baseTex.Sample(g_ss,In.UV);
	// ƒ}ƒeƒŠƒAƒ‹‚ÌF‚Æ’¸“_F‚Æ‡¬
	float4 baseColor = texColor * g_material.BaseColor * In.Color * g_mulMaterialColor;

	// RoughnessMetallicMap‚©‚çFæ“¾
	float4 mr = g_mrTex.Sample(g_ss, In.UV);

	// ‘e‚³
	float roughness = g_material.Roughness * mr.g;
	// ‹à‘®«
	float metallic = g_material.Metallic * mr.b;

	// roughness‚ğ‹t“]‚³‚¹uŠŠ‚ç‚©v‚³‚É‚·‚é
	// roughness‚ğ‚Qæ‚µ‚Ä‚¢‚é‚Ì‚ÍA‚»‚Ì•û‚ªŠ´Šo“I‚ÉƒŠƒAƒ‹‚É‚È‚é‚ç‚µ‚¢‚Ì‚Å
	float smoothness = 1.0 - roughness * roughness;
	float specPower = pow(2, 11 * smoothness); // 1`2048


	//------------------------------------------
	// ƒ‰ƒCƒeƒBƒ“ƒO
	//------------------------------------------
	// ÅI“I‚ÈF
	float3 color = 0;

	// ƒ‰ƒCƒg—LŒø
	if (g_lightEnable)
	{
		// Ş¿‚ÌŠgUF ”ñ‹à‘®‚Ù‚ÇŞ¿‚ÌF‚É‚È‚èA‹à‘®‚Ù‚ÇŠgUF‚Í–³‚­‚È‚é
		float3 baseDiffuse = lerp(baseColor.rgb, float3(0, 0, 0), metallic);
		//Ş¿‚Ì”½ËF ”ñ‹à‘®‚Ù‚ÇŒõ‚ÌF‚ğ‚»‚Ì‚Ü‚Ü”½Ë‚µA‹à‘®‚Ù‚ÇŞ¿‚ÌF‚ªæ‚é
		float3 baseSpecular = lerp(0.04, baseColor.rgb, metallic);

		// •½sŒõ‚Í•¡”‚ ‚é‚½‚ßAg—p‚µ‚Ä‚¢‚é‚Í‚¸‚¾‚¯ŒvZ‚·‚é
		for (int di = 0; di < g_DL_Cnt; di++)
		{
			//--------------------------------------
			// ŠgUŒõ
			//--------------------------------------
			// Lambertƒ‚ƒfƒ‹
			float lightDiffuse = dot(-g_DL[di].Dir, wN);	// “àÏ‚ÅŠp“x‚Ì·‚ğ‹‚ß‚é
			lightDiffuse = saturate(lightDiffuse); // ƒ}ƒCƒiƒX‚Í‚Ü‚¸‚¢‚Ì‚ÅA0`1“à‚ÉØ‚è‹l‚ß‚é

			// u–¾‚é‚³v‚ğUÀ•W‚Æ‚µ‚ÄAƒgƒD[ƒ“ƒeƒNƒXƒ`ƒƒ‚©‚çÅI“I‚È–¾‚é‚³ƒf[ƒ^‚ğæ“¾
			float3 toonColor = g_toonTex.Sample(g_clampSS, float2(lightDiffuse, 0.5)).rgb;

			// •¨—“I‚É³‚µ‚­‚·‚é‚½‚ßA³‹K‰»‚·‚é
			toonColor /= 3.1415926535;

			// ”ò‚Ño‚éŒõ‚ğŒvZ‚µAŒ‹‰Ê‚É‰ÁZ‚·‚é
			// (Œõ‚ÌF) * Ş¿‚ÌF * “§–¾—Í
			color += (g_DL[di].Color * toonColor) * baseDiffuse * baseColor.a;

			//--------------------------------------
			// ”½ËŒõ
			//--------------------------------------
			// Blinn-Phong
			float spec = BlinnPhong(g_DL[di].Dir, vCam, wN, specPower);

			// ”½Ë‚·‚éŒõ‚ğŒvZ‚µAŒ‹‰Ê‚É‰ÁZ‚·‚é
			// (Œõ‚ÌF) * ”ñ‹à‘®‚Í4%”½Ë‚ç‚µ‚¢ * “§–¾—Í
			color += (g_DL[di].Color * spec) * baseSpecular * baseColor.a;

		}

		//----------------------------
		// ŠÂ‹«Œõ
		//----------------------------
		color += g_AmbientLight * baseColor.rgb * baseColor.a;

		//------------------
		// IBL
		//------------------
		// ŠgUŒõ
		float3 envDiff = g_IBLTex.SampleLevel(g_ss, wN, 8).rgb; // ŠgU•\Œ»‚·‚é‚½‚ßA’´’á‰ğ‘œ“x‚Ì‰æ‘œ‚ğg—p‚·‚é
		color += envDiff * baseDiffuse.rgb / 3.141592;

		// ”½ËŒõ
		float3 vRef = reflect(-vCam, wN); // ”½ËƒxƒNƒgƒ‹‚ğ‹‚ß‚é
		float3 envSpec = g_IBLTex.SampleLevel(g_ss, vRef, roughness * 8).rgb; // ‘e‚¢‚Ù‚Ç’á‰ğ‘œ“x‚Ì‰æ‘œ‚ğg—p‚·‚é
		color += envSpec * baseSpecular;
		}
	// ƒ‰ƒCƒg–³Œø
	else
	{
		// Ş¿‚ÌF‚ğ‚»‚Ì‚Ü‚Üg—p
		color = baseColor.rgb;
	}

	// ‹üÜ•\Œ»
	if (g_refractiveIndex < 1.0)
	{
		// ƒJƒƒ‰•ûŒü‚ğg—p‚µ‚Ä‹üÜƒxƒNƒgƒ‹‚ğ‹‚ß‚é
		float3 vRefract = refract(-vCam, wN, g_refractiveIndex);
		float3 v = vRefract - (-vCam);
		// ‚±‚ÌƒsƒNƒZƒ‹‚Ì‰æ–Êã‚Å‚ÌUVÀ•W‚ğ‹‚ß‚é
		// ‹üÜ•ûŒü‚É‚·‚±‚µˆÚ“®‚³‚¹‚½3DÀ•W‚ğAË‰eÀ•W‚Ö•ÏŠ·
		float4 posProj = mul(float4(In.wPos + v * 0.5f, 1), g_mV);
		posProj = mul(posProj, g_mP);
		posProj.xyz /= posProj.w;
		// Ë‰eÀ•W‚©‚çUVÀ•W‚Ö•ÏŠ·
		float2 uv = posProj.xy * float2(1, -1);
		uv = uv * 0.5 + 0.5;
		// ƒV[ƒ“‰æ‘œ‚©‚çuv‚ÌˆÊ’u‚ÌF‚ğæ“¾
		float4 sceneColor = g_sceneTex.Sample(g_clampSS, uv);
		// •¨¿‚ÌF‚Æ”wŒi‚ÌF‚ğƒAƒ‹ƒtƒ@ƒuƒŒƒ“ƒh
		// ¦ n = lerp(a, b, t) c a`bŠÔ‚Ìt‚ÌˆÊ’u‚Ì’l‚ğ‹‚ß‚é
		color = lerp(sceneColor.rgb, color.rgb, baseColor.a);
		baseColor.a = 1; // •s“§–¾‚É‚·‚é
	}

	//------------------------------------
	// ‹——£ƒtƒHƒO
	//------------------------------------
	if (g_DistanceFogEnable && g_FogEnable)
	{
		// w”ƒtƒHƒO1i‹ß‚¢j`0i‰“‚¢j
		float f = saturate(1.0 / exp(camDist * g_DistanceFogDensity));
		// “K—p
		color.rgb = lerp(g_DistanceFogColor, color.rgb, f);
	}

	return float4(color, baseColor.a);
=======
float4 main() :SV_Target0
{
	return float4(1, 0, 0, 1);	//RGBA
>>>>>>> parent of fcd683a... ã€3Dpro2ã€‘
}
