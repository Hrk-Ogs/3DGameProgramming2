// カメラやライトのデータが使えるようになる
#include "../inc_KdCommon.hlsli"
// 共通データ
#include "inc_KdModelShader.hlsli"
//==================================
// ピクセルシェーダー １つ１つのピクセルがやってくる
//==================================
float4 main(VSOutput In) : SV_Target0
{
	// 単色で返す
	return float4(0,0,0,1);
}