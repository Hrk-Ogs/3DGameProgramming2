<<<<<<< HEAD
// ’è”ƒoƒbƒtƒ@
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;		// ƒ[ƒ‹ƒhs—ñ
	int g_lightEnable;				// ƒ‰ƒCƒg—LŒø
	int g_FogEnable;				// ƒtƒHƒO—LŒø

	float4 g_mulMaterialColor; // æZF

	// UV‘€ì
	float2 g_UVOffset; // UVƒIƒtƒZƒbƒg’l
	float2 g_UVTiling; // UV”{—¦

	float g_refractiveIndex; // ‹üÜ—¦
};

// ƒ}ƒeƒŠƒAƒ‹ƒf[ƒ^‚ğ\‘¢‘Ì‚É“Z‚ß‚é
struct Material
{
	float4 BaseColor;		// ƒx[ƒXE
	float3 Emissive;		// ©ŒÈ”­ŒõF
	float Metallic;			// ‹à‘®“x
	float Roughness;		// ‘e‚³
};

// ƒ}ƒeƒŠƒAƒ‹’è”ƒoƒbƒtƒ@
cbuffer cbMaterial:register(b1)
{
	Material g_material;
}

// ’¸“_ƒVƒF[ƒ_‚©‚ço—Í‚·‚éƒf[ƒ^
struct VSOutput
{
	float4 Pos : SV_Position;	// Ë‰eÀ•W
	float2 UV : TEXCOORD0;		// ƒeƒNƒXƒ`ƒƒUVÀ•W
	float4 Color : TEXCOORD1;	// ’¸“_F
	float3 wN : TEXCOORD2;	// ƒ[ƒ‹ƒh–@ü
	float3 wPos : TEXCOORD3;	// ƒ[ƒ‹ƒhÀ•W

	float3 wT : TEXCOORD4;	// ƒ[ƒ‹ƒhÚü
	float3 wB : TEXCOORD5;	// ƒ[ƒ‹ƒh]–@ü
};
=======
>>>>>>> parent of fcd683a... ã€3Dpro2ã€‘
