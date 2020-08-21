// カメラやライトのデータが使えるようになる
#include "../inc_KdCommon.hlsli"

// 定数バッファ
cbuffer cb : register(b0)
{
	row_major float4x4 g_mW;	// ワールド行列
}

// 頂点シェーダーから出力するデータ
struct VSOutput
{
	float4 Pos : SV_Position;		// 射影座標
};

//======================================
// 頂点シェーダー
//======================================
VSOutput main(float4 pos : POSITION)
{
	VSOutput Out;

	// 3D座標を2D（射影）座標へ変換
	Out.Pos = mul(pos, g_mW);	// ワールド行列で変換してから、
	Out.Pos = mul(pos, g_mP);	// 射影変換

	return Out;
}
