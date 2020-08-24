// 定数バッファ
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;		// ワールド行列
};

// マテリアルデータを構造体に纏める
struct Material
{
	float4 BaseColor;		// ベース職
	float3 Emissive;		// 自己発光色
	float Matallic;			// 金属度
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
};
