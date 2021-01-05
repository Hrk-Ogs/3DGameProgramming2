// カメラやライトのデータが使えるようになる
#include "../inc_KdCommon.hlsli"
// 共通データ
#include "inc_KdModelShader.hlsli"
//==================================
// 頂点シェーダー
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV座標が入ってくる
	float4 color : COLOR, // 頂点色が入ってくる
	float3 normal : NORMAL // 法線が入ってくる
)
{
	VSOutput Out;

	// 頂点座標を法線方向に、少しずらす
	pos.xyz = pos.xyz + normal * 0.03f;

	// 3D頂点座標を2Dへ変換
	Out.Pos = mul(pos, g_mW); // ワールド行列で変換してから、
	Out.wPos = Out.Pos.xyz; // ワールド座標を取っておく
	Out.Pos = mul(Out.Pos, g_mV); // カメラの逆行列(ビュー行列)で変換して、
	Out.Pos = mul(Out.Pos, g_mP); // 射影行列で変換

	// UVはそのままピクセルシェーダーへ(今回は未使用)
	Out.UV = uv;

	// 頂点色はそのままピクセルシェーダーへ(今回は未使用)
	Out.Color = color;

	// 法線をワールド座標系へ変換(今回は未使用)
	Out.wN = normalize(mul(normal, (float3x3)g_mW));

	return Out;
}