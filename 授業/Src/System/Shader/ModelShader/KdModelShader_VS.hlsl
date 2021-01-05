// ƒJƒƒ‰‚âƒ‰ƒCƒg‚Ìƒf[ƒ^‚ªg‚¦‚é‚æ‚¤‚É‚È‚é
#include "../inc_KdCommon.hlsli"

// ’è”ƒoƒbƒtƒ@
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;	// ƒ[ƒ‹ƒhs—ñ
}

// ’¸“_ƒVƒF[ƒ_[‚©‚ço—Í‚·‚éƒf[ƒ^
struct VSOutput
{
	float4 Pos : SV_Position;		// Ë‰eÀ•W
};

//======================================
// ’¸“_ƒVƒF[ƒ_[
//======================================
<<<<<<< HEAD
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR,float3 tangent : TANGENT, float3 normal : NORMAL)
=======
VSOutput main(float4 pos : POSITION)
>>>>>>> parent of fcd683a... ã€3Dpro2ã€‘
{
	VSOutput Out;

	// 3DÀ•W‚ğ2DiË‰ejÀ•W‚Ö•ÏŠ·
<<<<<<< HEAD
	Out.Pos = mul(pos, g_mW);		// ƒ[ƒ‹ƒhs—ñ‚Å•ÏŠ·‚µ‚Ä‚©‚çA
	Out.wPos = Out.Pos.xyz;			// ƒ[ƒ‹ƒhÀ•W‚ğæ‚Á‚Ä‚¨‚­
	Out.Pos = mul(Out.Pos, g_mV);	// ƒJƒƒ‰‚Ì‹ts—ñiƒrƒ…[s—ñj‚Å•ÏŠ·‚µ‚ÄA
	Out.Pos = mul(Out.Pos, g_mP);	// Ë‰e•ÏŠ·

	// UV‚ğ‰ÁH‚µƒsƒNƒZƒ‹ƒVƒF[ƒ_[‚Ö
	Out.UV = uv * g_UVTiling + g_UVOffset;

	// ’¸“_F‚Í‚»‚Ì‚Ü‚ÜƒsƒNƒZƒ‹ƒVƒF[ƒ_[‚Ö
	Out.Color = color;
=======
	Out.Pos = mul(pos, g_mW);	// ƒ[ƒ‹ƒhs—ñ‚Å•ÏŠ·‚µ‚Ä‚©‚çA
	Out.Pos = mul(pos, g_mP);	// Ë‰e•ÏŠ·
>>>>>>> parent of fcd683a... ã€3Dpro2ã€‘

	// –@ü‚ğƒ[ƒ‹ƒhÀ•WŒn‚Ö•ÏŠ·
	Out.wN = normalize(mul(normal, (float3x3)g_mW));
	// Úü‚ğƒ[ƒ‹ƒhÀ•WŒn‚Ö•ÏŠ·
	Out.wT = normalize(mul(tangent, (float3x3)g_mW));
	// ]–@ü‚ğA–@ü‚ÆÚü‚©‚çZo
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}
