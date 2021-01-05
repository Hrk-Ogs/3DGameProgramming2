// 頂点シェーダ出力構造体
// (ピクセルシェーダの入力データにもなる)
struct VSOutput
{
	float4 Pos : SV_Position;
	float2 UV : TEXCOORD0; // UV座標
};