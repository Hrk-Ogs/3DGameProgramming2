// 定数バッファ
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;		// ワールド行列
	int g_lightEnable;				// ライト有効
	int g_FogEnable;				// フォグ有効

	float4 g_mulMaterialColor; // 乗算色

	// UV操作
	float2 g_UVOffset; // UVオフセット値
	float2 g_UVTiling; // UV倍率

	float g_refractiveIndex; // 屈折率
};

// マテリアルデータを構造体に纏める
struct Material
{
	float4 BaseColor;		// ベース職
	float3 Emissive;		// 自己発光色
	float Metallic;			// 金属度
	float Roughness;		// 粗さ
};

// マテリアル定数バッファ
cbuffer cbMaterial:register(b1)
{
	Material g_material;
}

// 頂点シェーダから出力するデータ
struct VSOutput
{
	float4 Pos : SV_Position;	// 射影座標
	float2 UV : TEXCOORD0;		// テクスチャUV座標
	float4 Color : TEXCOORD1;	// 頂点色
	float3 wN : TEXCOORD2;	// ワールド法線
	float3 wPos : TEXCOORD3;	// ワールド座標

	float3 wT : TEXCOORD4;	// ワールド接線
	float3 wB : TEXCOORD5;	// ワールド従法線
};
