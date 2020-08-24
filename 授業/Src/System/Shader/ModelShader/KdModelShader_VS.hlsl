// カメラやライトのデータが使えるようになる
#include "../inc_KdCommon.hlsli"
// 共通データ
#include "inc_KdModelShader.hlsli"

//======================================
// 頂点シェーダー
//======================================
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR)
{
	VSOutput Out;

	// 3D座標を2D（射影）座標へ変換
	Out.Pos = mul(pos, g_mW);		// ワールド行列で変換してから、
	Out.Pos = mul(Out.Pos, g_mV);	// カメラの逆行列（ビュー行列）で変換して、
	Out.Pos = mul(Out.Pos, g_mP);	// 射影変換

	// UVはそのままピクセルシェーダーへ
	Out.UV = uv;

	// 頂点色はそのままピクセルシェーダーへ
	Out.Color = color;

	return Out;
}
