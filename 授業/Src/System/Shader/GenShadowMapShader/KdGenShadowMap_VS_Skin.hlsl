#include "../inc_KdCommon.hlsli"
#include "inc_KdGenShadowMap.hlsli"

// ボーン行列バッファ
struct BoneData
{
	row_major float4x4 mat;
};
StructuredBuffer<BoneData> g_sbBones : register(t100);

// 頂点シェーダ
VSOutput main(float4 pos : POSITION,
	float2 uv : TEXCOORD0,
	uint4 skinIndex : SKININDEX,
	float4 skinWeight : SKINWEIGHT
)
{
	//------------------------------
	// スキニング処理
	// この頂点に影響している複数のボーンの行列を使用して
	// posやnormalなどを変換する
	//------------------------------
	row_major float4x4 mBones = 0; // 最終的なワールド行列
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		mBones += g_sbBones[skinIndex[i]].mat * skinWeight[i];
	}
	// ボーン行列で変換(まだローカル座標系)
	pos = mul(pos, mBones);
	//------------------------------

	VSOutput Out;

	// 射影変換
	Out.Pos = mul(pos, g_mW);
	Out.Pos = mul(Out.Pos, g_mLightVP);

	// 別に射影座標を持っていく(Out.Posは、PSに来た時にはスクリーン座標系になっているため)
	Out.wvpPos = Out.Pos;
	// UV
	Out.UV = uv;

	return Out;
}