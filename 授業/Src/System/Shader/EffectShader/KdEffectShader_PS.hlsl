#include "../inc_KdCommon.hlsli"
#include "inc_KdEffectShader.hlsli"

// テクスチャ
Texture2D g_tex : register(t0);

// サンプラ
SamplerState g_ss : register(s0);

//================================
// エフェクト用ピクセルシェーダ
//================================
float4 main(VSOutput In) : SV_Target0
{
    float4 texColor = g_tex.Sample(g_ss, In.UV);
    texColor.rgb = pow(texColor.rgb, 2.2);
    return texColor * In.Color * g_Color;
}
