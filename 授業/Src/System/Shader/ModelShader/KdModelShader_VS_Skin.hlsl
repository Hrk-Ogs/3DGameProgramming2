// カメラやライトのデータが使えるようになる
#include "../inc_KdCommon.hlsli"
// 共通データ
#include "inc_KdModelShader.hlsli"

// ボーン行列バッファ(構造化バッファ)
struct BoneData
{
	row_major float4x4 mat;
};
StructuredBuffer<BoneData> g_sbBones : register(t100);

//==================================
// 頂点シェーダー
//==================================
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0, // UV座標が入ってくる
	float4 color : COLOR, // 頂点色が入ってくる
	float3 tangent : TANGENT, // 接線が入ってくる
	float3 normal : NORMAL, // 法線が入ってくる
	uint4 skinIndex : SKININDEX, // 何番目のボーンに影響しているか？のデータ(最大４つぶん)
	float4 skinWeight : SKINWEIGHT // そのボーンに、どれだけ影響しているか？のデータ(最大４つぶん)
)
{
	VSOutput Out;

	//------------------------------
	// スキニング処理
	// この頂点に影響している複数のボーンの行列を使用して
	// posやnormalなどを変換する
	//------------------------------
	row_major float4x4 mBones = 0; // 最終的な変換行列
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		mBones += g_sbBones[skinIndex[i]].mat * skinWeight[i];
	}
	// 変換行列で変換(まだローカル座標系)
	pos = mul(pos, mBones);
	normal = mul(normal, (float3x3) mBones);
	//------------------------------

	// 3D頂点座標を2Dへ変換
	Out.Pos = mul(pos, g_mW); // ワールド行列で変換してから、
	Out.wPos = Out.Pos.xyz; // ワールド座標を取っておく
	Out.Pos = mul(Out.Pos, g_mV); // カメラの逆行列(ビュー行列)で変換して、
	Out.Pos = mul(Out.Pos, g_mP); // 射影行列で変換

	// UVを加工しピクセルシェーダーへ
	Out.UV = uv * g_UVTiling + g_UVOffset;

	// 頂点色はそのままピクセルシェーダーへ
	Out.Color = color;

	//	法線をワールド座標系へ変換
	Out.wN = normalize(mul(normal, (float3x3)g_mW));
	// 接線をワールド座標系へ変換
	Out.wT = normalize(mul(tangent, (float3x3)g_mW));
	// 従法線を法線と接線から算出
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}