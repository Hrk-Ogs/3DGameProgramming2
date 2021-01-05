#include "System/KdSystem.h"
#include "KdPostProcessShader.h"

void KdPostProcessShader::ColorDraw(const KdTexture* tex, const KdVec4& color)
{
	if (tex == nullptr)return;

	// 現在セットされているDSを記憶しておく
	ComPtr<ID3D11DepthStencilState> saveDS;
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	// 定数バッファ
	m_cb0_Color.Work().Color = color; // 値セット
	m_cb0_Color.Write(); // 書き込み(GPU側へ転送)
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0_Color.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Color.GetAddress());

	// テクスチャセット
	D3D.GetDevContext()->PSSetShaderResources(0, 1, tex->GetSRViewAddress());

	// 頂点シェーダーセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 入力レイアウト
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// ピクセルシェーダーをセット
	D3D.GetDevContext()->PSSetShader(m_colorPS, 0, 0);

	// 頂点データ
	Vertex v[4] = {
		{ {-1,-1,0}, {0, 1} },// 0
		{ {-1, 1,0}, {0, 0} },// 1
		{ { 1,-1,0}, {1, 1} },// 2
		{ { 1, 1,0}, {1, 0} } // 3
	};

	// 頂点描画
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // 頂点をどういう感じに使用するか？のフラグ
		4, // 使用する頂点数
		&v[0], // 頂点配列の先頭アドレス
		sizeof(Vertex) // １頂点のバイトサイズ
	);

	// テクスチャを解除
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// DSを元に戻す
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}

bool KdPostProcessShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdPostProcess_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) 
		{
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点インプットレイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0],
			layout.size(),
			compiledBuffer,
			sizeof(compiledBuffer),
			&m_inputLayout))
			) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	// 色描画
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdPostProcess_Color_PS.inc"
		// ピクセルシェーダ作成
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_colorPS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}
	// 定数バッファ作成
	m_cb0_Color.Create();

	// ぼかし
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdPostProcess_Blue_PS.inc"
		// ピクセルシェーダ作成
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_blurPS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}
	// 定数バッファ作成
	m_cb0_Blur.Create();

	// 高輝度抽出
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdPostProcess_BrightFiltering_PS.inc"
		// ピクセルシェーダ作成
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_brightFilteringPS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	// トゥーンマッピング
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdPostProcess_ToneMapping_PS.inc"
		// ピクセルシェーダ作成
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_toneMappingPS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	// 波
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdPostProcess_WaveHeight_PS.inc"
		// ピクセルシェーダ作成
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_waveHeightPS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdPostProcess_WaveNormal_PS.inc"

		// ピクセルシェーダ作成
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_waveNormalPS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}
	// 定数バッファ作成
	m_cb0_Wave.Create();

	return true;
}

void KdPostProcessShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);

	KdSafeRelease(m_colorPS);
	m_cb0_Color.Release();

	KdSafeRelease(m_blurPS);
	m_cb0_Blur.Release();

	KdSafeRelease(m_brightFilteringPS);

	KdSafeRelease(m_waveHeightPS);
	KdSafeRelease(m_waveNormalPS);
	m_cb0_Wave.Release();
}

void KdPostProcessShader::AdvanceWave(KdWaveTexture& wave)
{
	ComPtr<ID3D11RenderTargetView> saveRTV;
	ComPtr<ID3D11DepthStencilView> saveDSV;
	D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

	//---------------------------
	// 定数バッファ更新
	//---------------------------
	// 水面にランダムな力を加える
	m_cb0_Wave.Work().AddPowerUV.x = KdMTRand::s_rnd.GetFloat(0, 1); // U座標
	m_cb0_Wave.Work().AddPowerUV.y = KdMTRand::s_rnd.GetFloat(0, 1); // V座標
	m_cb0_Wave.Work().AddPowerRadius = KdMTRand::s_rnd.GetFloat(1.0f / 512, 3.0f / 512); // 半径
	m_cb0_Wave.Work().AddPower = KdMTRand::s_rnd.GetFloat(-0.3f, 0.3f); // 力
	m_cb0_Wave.Write();
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Wave.GetAddress());

	//---------------------------
	// 高さ・速さマップ更新
	//---------------------------
	// RT変更
	D3D.GetDevContext()->OMSetRenderTargets(1, wave.m_rtHeight[0]->GetRTViewAddress(), nullptr);
	D3D.SetViewportFromRTV(wave.m_rtHeight[0]->GetRTView());

	// テクスチャセット
	D3D.GetDevContext()->PSSetShaderResources(0, 1, wave.m_rtHeight[1]->GetSRViewAddress());

	// 各シェーダをセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	D3D.GetDevContext()->PSSetShader(m_waveHeightPS, 0, 0);

	// 頂点データ
	Vertex v[4] = {
{ {-1,-1,0}, {0, 1} },// 0
{ {-1, 1,0}, {0, 0} },// 1
{ { 1,-1,0}, {1, 1} },// 2
{ { 1, 1,0}, {1, 0} } // 3
	};
	// 頂点描画
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4, // 使用する頂点数
		&v[0], // 頂点配列の先頭アドレス
		sizeof(Vertex) // １頂点のバイトサイズ
	);

	// [0]と[1]を入れ替える
	std::swap(wave.m_rtHeight[0], wave.m_rtHeight[1]);

	//---------------------------
	// 法線マップ更新
	//---------------------------
	// RT変更
	D3D.GetDevContext()->OMSetRenderTargets(1, wave.m_rtNormal->GetRTViewAddress(), nullptr);
	D3D.SetViewportFromRTV(wave.m_rtNormal->GetRTView());

	// 高さマップテクスチャセット
		D3D.GetDevContext()->PSSetShaderResources(0, 1, wave.m_rtHeight[1]->GetSRViewAddress());

	// シェーダをセット
	D3D.GetDevContext()->PSSetShader(m_waveNormalPS, 0, 0);

	// 頂点描画
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4, // 使用する頂点数
		&v[0], // 頂点配列の先頭アドレス
		sizeof(Vertex) // １頂点のバイトサイズ
	);

	//---------------------------
	// 後始末
	//---------------------------
	// テクスチャを解除
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// RTとZバッファを元に戻す
	D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get());
	D3D.SetViewportFromRTV(saveRTV.Get());
}

void KdPostProcessShader::BrightFiltering(const KdTexture* destRT, const KdTexture* srcTex)
{
	if (destRT == nullptr || srcTex == nullptr)return;

	// 現在のRTとZバッファを記憶しておく
	ComPtr<ID3D11RenderTargetView> saveRTV[8] = {};
	ComPtr<ID3D11DepthStencilView> saveDSV;
	D3D.GetDevContext()->OMGetRenderTargets(8, saveRTV[0].GetAddressOf(), &saveDSV);

	// RT変更
	D3D.GetDevContext()->OMSetRenderTargets(1, destRT->GetRTViewAddress(), nullptr);

	// シェーダー
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	D3D.GetDevContext()->PSSetShader(m_brightFilteringPS, 0, 0);
	// 入力レイアウト
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);

	// テクスチャセット
	D3D.GetDevContext()->PSSetShaderResources(0, 1, srcTex->GetSRViewAddress());

	// 頂点データ
	Vertex v[4] = {
	{ {-1,-1,0}, {0, 1} },// 0
	{ {-1, 1,0}, {0, 0} },// 1
	{ { 1,-1,0}, {1, 1} },// 2
	{ { 1, 1,0}, {1, 0} } // 3
	};

	// 頂点描画
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // 頂点をどういう感じに使用するか？のフラグ
		4, // 使用する頂点数
		&v[0], // 頂点配列の先頭アドレス
		sizeof(Vertex) // １頂点のバイトサイズ
	);

	// テクスチャを解除
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// RTとZバッファをもとに戻す
	D3D.GetDevContext()->OMSetRenderTargets(8, saveRTV->GetAddressOf(), saveDSV.Get());
}

void KdPostProcessShader::BlurDraw(const KdTexture* tex, const KdVec2& dir)
{
	if (tex == nullptr)return;

	// 現在セットされているDSを記憶しておく
	ComPtr<ID3D11DepthStencilState> saveDS;
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	//-------------------------
	// ガウスぼかしの設定
	//-------------------------
	// テクセルサイズ(１ピクセルのUV座標系でのサイズ)
	KdVec2 ts;
	ts.x = 1.0f / tex->GetWidth();
	ts.y = 1.0f / tex->GetHeight();
	// オフセット座標と重みの設定
	float totalWeight = 0;
	const float dispersion = 3; // 分散
	for (int i = 0; i < 31; i++)
	{
		int pt = i - 15; // -15 ～ +15
		// オフセット座標
		m_cb0_Blur.Work().Offset[i].x = dir.x * (pt * ts.x);
		m_cb0_Blur.Work().Offset[i].y = dir.y * (pt * ts.y);
		// 重み(ガウス関数)
		float w = exp(-(pt * pt) / (2 * dispersion * dispersion));
		m_cb0_Blur.Work().Offset[i].z = w;
		totalWeight += w;
	}
	// 重み正規化
	for (int i = 0; i < 31; i++)
	{
		m_cb0_Blur.Work().Offset[i].z /= totalWeight;
	}

	// 書き込み(GPU側へ転送)
	m_cb0_Blur.Write();

	// 定数バッファセット
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0_Blur.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Blur.GetAddress());

	// テクスチャセット
	D3D.GetDevContext()->PSSetShaderResources(0, 1, tex->GetSRViewAddress());

	// 頂点シェーダーセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 入力レイアウト
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// ピクセルシェーダーをセット
	D3D.GetDevContext()->PSSetShader(m_blurPS, 0, 0);

	// 頂点データ
	Vertex v[4] = {
	{ {-1,-1,0}, {0, 1} },// 0
	{ {-1, 1,0}, {0, 0} },// 1
	{ { 1,-1,0}, {1, 1} },// 2
	{ { 1, 1,0}, {1, 0} } // 3
	};

	// 頂点描画
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // 頂点をどういう感じに使用するか？のフラグ
		4, // 使用する頂点数
		&v[0], // 頂点配列の先頭アドレス
		sizeof(Vertex) // １頂点のバイトサイズ
	);

	// テクスチャを解除
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// DSを元に戻す
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}

void KdPostProcessShader::GenerateBlur(KdBlurTexture& blurTex, const KdTexture* srcTex)
{
	//---------------------
	// 現在のRTとZバッファを記憶しておく
	//---------------------
	ComPtr<ID3D11RenderTargetView> saveRTV;
	ComPtr<ID3D11DepthStencilView> saveDSV;
	D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

	// MGF
	for (int i = 0; i < 5; i++)
	{
		//---------------------
		// 縮小コピー
		//---------------------
		// RT変更
		D3D.GetDevContext()->OMSetRenderTargets(1, blurTex.m_rt[i][0]->GetRTViewAddress(), nullptr);
		// ビューポートを設定
		D3D.SetViewportFromRTV(blurTex.m_rt[i][0]->GetRTView());
		if (i == 0) {
			ColorDraw(srcTex);
		}
		else {
			ColorDraw(blurTex.m_rt[i - 1][0].get());
		}


		//---------------------
		// 横ぼかし
		//---------------------
		// RT変更
		D3D.GetDevContext()->OMSetRenderTargets(1, blurTex.m_rt[i][1]->GetRTViewAddress(), nullptr);
		// ビューポートを設定
		D3D.SetViewportFromRTV(blurTex.m_rt[i][1]->GetRTView());

		BlurDraw(blurTex.m_rt[i][0].get(), { 1, 0 });

		//---------------------
		// 縦ぼかし
		//---------------------
		// RT変更
		D3D.GetDevContext()->OMSetRenderTargets(1, blurTex.m_rt[i][0]->GetRTViewAddress(), nullptr);
		// ビューポートを設定
		D3D.SetViewportFromRTV(blurTex.m_rt[i][0]->GetRTView());

		BlurDraw(blurTex.m_rt[i][1].get(), { 0, 1 });
	}

	//---------------------
	// RTとZバッファをもとに戻す
	//---------------------
	D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get());
	D3D.SetViewportFromRTV(saveRTV.Get());
}

void KdPostProcessShader::ToneMappingDraw(const KdTexture* tex)
{
	if (tex == nullptr)return;

	// 現在セットされているDSを記憶しておく
	ComPtr<ID3D11DepthStencilState> saveDS;
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	// テクスチャセット
	D3D.GetDevContext()->PSSetShaderResources(0, 1, tex->GetSRViewAddress());

	// 頂点シェーダーセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 入力レイアウト
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// ピクセルシェーダーをセット
	D3D.GetDevContext()->PSSetShader(m_toneMappingPS, 0, 0);

	// 頂点データ
	Vertex v[4] = {
	{ {-1,-1,0}, {0, 1} },// 0
	{ {-1, 1,0}, {0, 0} },// 1
	{ { 1,-1,0}, {1, 1} },// 2
	{ { 1, 1,0}, {1, 0} } // 3
	};

	// 頂点描画
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // 頂点をどういう感じに使用するか？のフラグ
		4, // 使用する頂点数
		&v[0], // 頂点配列の先頭アドレス
		sizeof(Vertex) // １頂点のバイトサイズ
	);

	// テクスチャを解除
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// DSを元に戻す
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}