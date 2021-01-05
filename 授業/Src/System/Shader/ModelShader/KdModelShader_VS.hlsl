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
<<<<<<< HEAD
VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR,float3 tangent : TANGENT, float3 normal : NORMAL)
=======
VSOutput main(float4 pos : POSITION)
>>>>>>> parent of fcd683a... 縲�3Dpro2縲�
{
	VSOutput Out;

	// 3D座標を2D（射影）座標へ変換
<<<<<<< HEAD
	Out.Pos = mul(pos, g_mW);		// ワールド行列で変換してから、
	Out.wPos = Out.Pos.xyz;			// ワールド座標を取っておく
	Out.Pos = mul(Out.Pos, g_mV);	// カメラの逆行列（ビュー行列）で変換して、
	Out.Pos = mul(Out.Pos, g_mP);	// 射影変換

	// UVを加工しピクセルシェーダーへ
	Out.UV = uv * g_UVTiling + g_UVOffset;

	// 頂点色はそのままピクセルシェーダーへ
	Out.Color = color;
=======
	Out.Pos = mul(pos, g_mW);	// ワールド行列で変換してから、
	Out.Pos = mul(pos, g_mP);	// 射影変換
>>>>>>> parent of fcd683a... 縲�3Dpro2縲�

	// 法線をワールド座標系へ変換
	Out.wN = normalize(mul(normal, (float3x3)g_mW));
	// 接線をワールド座標系へ変換
	Out.wT = normalize(mul(tangent, (float3x3)g_mW));
	// 従法線を、法線と接線から算出
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}
