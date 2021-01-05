#pragma once

//=======================================
// ブラーテクスチャ
//=======================================
struct KdBlurTexture
{
	// テクスチャ作成
	void Create(int w, int h)
	{
		// レンダーターゲットテクスチャ(RT)として作成
		int divideValue = 2;
		for (int i = 0; i < 5; i++)
		{
			m_rt[i][0] = std::make_shared<KdTexture>();
			m_rt[i][0]->CreateRenderTarget(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			m_rt[i][1] = std::make_shared<KdTexture>();
			m_rt[i][1]->CreateRenderTarget(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			divideValue *= 2;
		}
	}

	// 作業用レンダーターゲットテクスチャ
	KdSptr<KdTexture> m_rt[5][2];
};

//=======================================
// 波テクスチャ
//=======================================
struct KdWaveTexture
{
	// 高さマップ
	KdSptr<KdTexture> m_rtHeight[2];
	// 法線マップ
	KdSptr<KdTexture> m_rtNormal;

	// 作成
	void Create(int w, int h)
	{
		m_rtHeight[0] = std::make_shared<KdTexture>();
		m_rtHeight[1] = std::make_shared<KdTexture>();
		m_rtNormal = std::make_shared<KdTexture>();

		m_rtHeight[0]->CreateRenderTarget(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtHeight[1]->CreateRenderTarget(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtNormal->CreateRenderTarget(w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
		// 高さマップクリア
		D3D.GetDevContext()->ClearRenderTargetView(m_rtHeight[0]->GetRTView(), KdVec4(0, 0, 0, 0));
		D3D.GetDevContext()->ClearRenderTargetView(m_rtHeight[1]->GetRTView(), KdVec4(0, 0, 0, 0));
	}
};

//=======================================================
//
// ポストプロセスシェーダー
//
//=======================================================
class KdPostProcessShader
{
public:

	// 頂点形式
	struct Vertex
	{
		Math::Vector3 Pos;
		Math::Vector2 UV;
	};

	// 色描画
	void ColorDraw(const KdTexture* tex, const KdVec4& color = { 1,1,1,1 });

	// ぼかし描画
	// ・tex … ソーステクスチャ
	// ・dir … ぼかす方向ベクトル( {1,0}で左右方向のぼかす {0,1}で上下方向にぼかす)
	void BlurDraw(const KdTexture* tex, const KdVec2& dir);

	// ぼかし画像を生成する
	// ・blurTex … 出力先のテクスチャ
	// ・srcTex … ソーステクスチャ
	void GenerateBlur(KdBlurTexture& blurTex, const KdTexture* srcTex);

	// 高輝度抽出
	// ・destRT … 高輝度色を入れるテクスチャ
	// ・srcTex … 抽出元テクスチャ
	void BrightFiltering(const KdTexture* destRT, const KdTexture* srcTex);

	// トーンマッピング描画
	void ToneMappingDraw(const KdTexture* tex);

	// 波進行
	// ・wave … 更新する波テクスチャ
	void AdvanceWave(KdWaveTexture& wave);

	//================================================
	// 初期化・解放
	//================================================
	// 初期化
	bool Init();
	// 解放
	void Release();
	//
	~KdPostProcessShader()
	{
		Release();
	}

private:
	// 頂点シェーダ
	ID3D11VertexShader* m_VS = nullptr; // 頂点シェーダー
	ID3D11InputLayout* m_inputLayout = nullptr; // 頂点入力レイアウト

	// ピクセルシェーダ(色描画)
	ID3D11PixelShader* m_colorPS = nullptr;
	// ピクセルシェーダ(ぼかし)
	ID3D11PixelShader* m_blurPS = nullptr;
	// ピクセルシェーダ(高輝度抽出)
	ID3D11PixelShader* m_brightFilteringPS = nullptr;
	// ピクセルシェーダ(トーンマッピング)
	ID3D11PixelShader* m_toneMappingPS = nullptr;
	// ピクセルシェーダ(波)
	ID3D11PixelShader* m_waveHeightPS = nullptr;
	ID3D11PixelShader* m_waveNormalPS = nullptr;

	// 定数バッファ(Color描画用)
	struct cbCoolor {
		KdVec4 Color = { 1,1,1,1 };
	};
	KdConstantBuffer<cbCoolor> m_cb0_Color;

	// 定数バッファ(Blur描画用)
	struct cbBlur {
		KdVec4 Offset[31];
	};
	KdConstantBuffer<cbBlur> m_cb0_Blur;

	// 定数バッファ(Wave用)
	struct cbWave {
		KdVec2 AddPowerUV; // 力を加えるUV座標
		float AddPowerRadius; // その半径
		float AddPower; // 威力
	};
	KdConstantBuffer<cbWave> m_cb0_Wave;
};