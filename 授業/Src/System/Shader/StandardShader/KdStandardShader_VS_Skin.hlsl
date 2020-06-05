#include "../inc_KdCommon.hlsli"
#include "inc_KdStandardShader.hlsli"


// ボーン行列バッファ
struct BoneData
{
    row_major float4x4 mat;
};
StructuredBuffer<BoneData> g_sbBones : register(t100);


//================================
// スキンメッシュ用 頂点シェーダ
//================================
VSOutput main(  float4 pos : POSITION,  // 頂点座標
                float2 uv : TEXCOORD0,  // テクスチャUV座標
                float3 tangent : TANGENT,// 接線ベクトル
                float3 normal : NORMAL,  // 法線ベクトル
                float4 color : COLOR,
                uint4 skinIndex : SKININDEX,
                float4 skinWeight : SKINWEIGHT
){

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
    tangent = mul(tangent, (float3x3) mBones);
    normal = mul(normal, (float3x3) mBones);
    //------------------------------

    VSOutput Out;

    // 座標変換
    Out.Pos = mul(pos, g_mW);       // ローカル座標系 -> ワールド座標系へ変換
        Out.wPos = Out.Pos.xyz;
    Out.Pos = mul(Out.Pos, g_mV);   // ワールド座標系 -> ビュー座標系へ変換
    Out.Pos = mul(Out.Pos, g_mP);   // ビュー座標系 -> 射影座標系へ変換

    // 法線
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));
    Out.wN = normalize(mul(normal, (float3x3)g_mW));
    Out.wB = normalize(cross(Out.wN, Out.wT));

    // 頂点色
    Out.Color = color;

    // UV座標
    Out.UV = uv * g_UVTiling + g_UVOffset;

    // 出力
    return Out;
}
