#include "inc_PostProcess.hlsli"

// 入力テクスチャ
Texture2D g_inputTex : register(t0);
// サンプラ(Linear補間 Clampモード)
SamplerState g_ss : register(s3);

// 高輝度抽出ピクセルシェーダー
float4 main(VSOutput In) : SV_Target0
{
	// テクスチャの色取得
	float4 texColor = g_inputTex.Sample(g_ss, In.UV);

	// テクスチャの色から１を引く
	texColor.rgb = max(0, texColor.rgb - 1);
	// max(a ,b) … aかb、大きい方を返す

	// 残った色＝高輝度色なので、それを出力する
	return texColor;
}