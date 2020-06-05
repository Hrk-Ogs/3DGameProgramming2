//=====================================================
//
// 全シェーダ共通
//
//=====================================================

//------------------------------
// 定数バッファ(カメラ)
//------------------------------
cbuffer cbCamera : register(b7)
{
    // カメラ情報
    row_major float4x4  g_mV; // ビュー変換行列
    row_major float4x4  g_mP; // 射影変換行列
    float3              g_CamPos; // カメラ座標

	// フォグ
    int     g_DistanceFogEnable; // フォグ有効/無効フラグ
    float3  g_DistanceFogColor; // フォグ色
    float   g_DistanceFogDensity; // フォグ減衰率
};

//------------------------------
// 定数バッファ(ライト)
//------------------------------

// 平行光のデータ
struct DirectionalLight
{
	float3 Dir; // 光の方向
	float3 Color; // 光の色
    float tmp;
};

// ポイントライトのデータ
struct PointLight
{
	float3 Color; // 色
	float Radius; // 半径
	float3 Pos; // 座標
    float tmp;
};

// スポットライトのデータ
struct SpotLight
{
    float3 Dir; // 光の方向
    float Range; // 範囲
    float3 Color; // 色
    float InnerCorn; // 内側の角度
    float3 Pos; // 座標
    float OuterCorn; // 外側の角度
};

cbuffer cbLight : register(b8)
{
	int g_DL_Cnt = 0; // 平行光の数
	int g_PL_Cnt = 0; // 点光の数
    int g_SL_Cnt = 0; // 集中光の数
    
    //-------------------------
    // 環境光
    //-------------------------
    float3  g_AmbientLight;

    //-------------------------
    // 平行光
    //-------------------------

    DirectionalLight    g_DL[3];    // 全ての平行光データ

    //-------------------------
    // 点光
    //-------------------------

	PointLight g_PL[100];           // 全ての点光データ

    //-------------------------
    // 集中光
    //-------------------------

    SpotLight g_SL[30];             // 全ての集中光データ
    
};
