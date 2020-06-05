#include "System/KdSystem.h"

#include "KdTexture.h"

bool KdTexture::Load(const std::string& filename, bool renderTarget, bool depthStencil, bool generateMipmap)
{
	Release();
	if (filename.empty())return false;

	// ファイル名をWideCharへ変換
	std::wstring wFilename = sjis_to_wide(filename);

	//------------------------------------
	// 画像読み込み
	//------------------------------------

	// Bind Flags
	UINT bindFlags = 0;
	bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (renderTarget)bindFlags |= D3D11_BIND_RENDER_TARGET;
	if (depthStencil)bindFlags |= D3D11_BIND_DEPTH_STENCIL;


	// ※DirectX Texライブラリを使用して画像を読み込む

	DirectX::TexMetadata meta;
	DirectX::ScratchImage image;

	bool bLoaded = false;

	// WIC画像読み込み
	//  WIC_FLAGS_ALL_FRAMES … gifアニメなどの複数フレームを読み込んでくれる
	if (SUCCEEDED(DirectX::LoadFromWICFile(wFilename.c_str(), DirectX::WIC_FLAGS_ALL_FRAMES, &meta, image)))
	{
		bLoaded = true;
	}

	// DDS画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromDDSFile(wFilename.c_str(), DirectX::DDS_FLAGS_NONE, &meta, image)))
		{
			bLoaded = true;
		}
	}

	// TGA画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromTGAFile(wFilename.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// HDR画像読み込み
	if (bLoaded == false) {
		if (SUCCEEDED(DirectX::LoadFromHDRFile(wFilename.c_str(), &meta, image)))
		{
			bLoaded = true;
		}
	}

	// 読み込み失敗
	if (bLoaded == false)
	{
		return false;
	}

	// ミップマップ生成
	if (meta.mipLevels == 1 && generateMipmap)
	{
		DirectX::ScratchImage mipChain;
		if (SUCCEEDED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain)))
		{
			image.Release();
			image = std::move(mipChain);
		}
	}

	//------------------------------------
	// テクスチャリソース作成
	//------------------------------------
	if (FAILED(DirectX::CreateTextureEx(
		D3D.GetDev(),						// Direct3D Device
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		D3D11_USAGE_DEFAULT,								// Usage
		bindFlags,							// Bind Flags
		0,									// CPU Access Flags
		0,									// MiscFlags
		false,								// ForceSRGB
		(ID3D11Resource**)&m_resource)
	)) {

		return false;
	}

	//------------------------------------
	// テクスチャリソース(m_resource)から、各ビューを作成する
	//------------------------------------
	if (CreateViewsFromTexture2D() == false)
	{
		Release();
		return false;
	}

	// 画像情報取得
	m_resource->GetDesc(&m_desc);

	m_filepath = filename;

	return true;
}

bool KdTexture::Create(ID3D11Texture2D* pTexture2D)
{
	Release();
	if (pTexture2D == nullptr)return false;

	// アドレスコピーと参照カウンター増加
	m_resource = pTexture2D;
	m_resource->AddRef();

	//---------------------------------------------
	// 画像リソースから、各ビューを作成する
	//---------------------------------------------
	if (CreateViewsFromTexture2D() == false)
	{
		Release();
		return false;
	}

	// ファイル名記憶
	m_filepath = "";

	// 画像情報取得
	m_resource->GetDesc(&m_desc);

	return true;

}

bool KdTexture::Create(const D3D11_TEXTURE2D_DESC & desc, const D3D11_SUBRESOURCE_DATA * fillData)
{
	Release();

	//--------------------------------------------
	// 2Dテクスチャの生成
	//--------------------------------------------
	HRESULT hr = D3D.GetDev()->CreateTexture2D(&desc, fillData, &m_resource);
	if (FAILED(hr)) {
		Release();
		return false;
	}

	//--------------------------------------------
	// m_pTexture2Dから、各ビューを作成する
	//--------------------------------------------
	if (CreateViewsFromTexture2D() == false)
	{
		Release();
		return false;
	}

	// 画像情報取得
	m_resource->GetDesc(&m_desc);

	return true;
}

bool KdTexture::Create(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA * fillData)
{
	Release();

	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Width = (UINT)w;
	desc.Height = (UINT)h;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = arrayCnt;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// 作成
	if (Create(desc, fillData) == false)return false;

	return true;
}

bool KdTexture::CreateRenderTarget(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData)
{
	Release();

	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Width = (UINT)w;
	desc.Height = (UINT)h;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = arrayCnt;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// 作成
	if (Create(desc, fillData) == false)return false;

	return true;
}

bool KdTexture::CreateDepthStencil(int w, int h, DXGI_FORMAT format, UINT arrayCnt, const D3D11_SUBRESOURCE_DATA* fillData)
{
	Release();

	// 作成する2Dテクスチャ設定
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.Width = (UINT)w;
	desc.Height = (UINT)h;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = arrayCnt;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// 作成
	if (Create(desc, fillData) == false)return false;

	return true;
}

void KdTexture::SetSRView(ID3D11ShaderResourceView* srv)
{
	if (srv == nullptr)return;

	Release();

	m_srv = srv;	// セット
	m_srv->AddRef();// 参照カウンタを増やしておく

	// リソース取得
	{
		ID3D11Resource* res;
		m_srv->GetResource(&res);
		if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&m_resource)))
		{
			res->Release();
			return;
		}
		res->Release();
	}

	// 画像情報取得
	m_resource->GetDesc(&m_desc);
}

void KdTexture::Release()
{
	KdSafeRelease(m_resource);
	KdSafeRelease(m_srv);
	KdSafeRelease(m_rtv);
	KdSafeRelease(m_dsv);

	for (auto&& node : m_srvArray) KdSafeRelease(node);
	for (auto&& node : m_rtvArray) KdSafeRelease(node);
	for (auto&& node : m_dsvArray) KdSafeRelease(node);
	m_srvArray.clear();
	m_rtvArray.clear();
	m_dsvArray.clear();

	m_filepath = "";
}

bool KdTexture::CreateViewsFromTexture2D()
{
	// リソースが無い
	if (m_resource == nullptr)return false;

	// テクスチャ本体の情報取得
	m_resource->GetDesc(&m_desc);

	// 既に作成済みのビューを解放
	KdSafeRelease(m_srv);
	KdSafeRelease(m_rtv);
	KdSafeRelease(m_dsv);
	for (auto&& node : m_srvArray) KdSafeRelease(node);
	for (auto&& node : m_rtvArray) KdSafeRelease(node);
	for (auto&& node : m_dsvArray) KdSafeRelease(node);
	m_srvArray.clear();
	m_rtvArray.clear();
	m_dsvArray.clear();

	//===========================================================
	//
	// RenderTargetViewを作成する
	//
	//===========================================================

	// レンダーターゲットフラグがついてる時のみ
	if (m_desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		// 作成するビューの設定
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_desc.Format;	// Format
		// 単品のテクスチャ(通常テクスチャ)の場合
		if (m_desc.ArraySize == 1) {
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;			// 単品テクスチャ
		}
		// テクスチャ配列の場合
		else {
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;		// テクスチャ配列
			rtvDesc.Texture2DArray.ArraySize = m_desc.ArraySize;			// 要素数
			rtvDesc.Texture2DArray.FirstArraySlice = 0;
			rtvDesc.Texture2DArray.MipSlice = 0;
		}

		// レンダーターゲットビュー作成
		HRESULT hr = D3D.GetDev()->CreateRenderTargetView(m_resource, &rtvDesc, &m_rtv);
		if (FAILED(hr))
		{
			assert(0 && "RenderTargetViewの作成に失敗");
			return false;
		}

		// 「テクスチャ配列」の場合は、各要素の画像へのビューもつくっておくと便利
		if (m_desc.ArraySize > 1)
		{
			m_rtvArray.resize(m_desc.ArraySize);
			for (UINT i = 0; i < m_desc.ArraySize; i++) {

				rtvDesc.Texture2DArray.ArraySize = 1;			// 単品用なので1
				rtvDesc.Texture2DArray.FirstArraySlice = i;		// ここが配列のインデックスになる

				//-------------------------------
				// i番目の画像のレンダーターゲットビュー作成
				//-------------------------------
				HRESULT hr = D3D.GetDev()->CreateRenderTargetView(m_resource, &rtvDesc, &m_rtvArray[i]);
				if (FAILED(hr)) {
					Release();
					return false;
				}
			}
		}
	}

	//===========================================================
	//
	// ShaderResourceViewの情報を作成する
	//
	//===========================================================
	// シェーダリソースビューフラグがついてる時のみ
	if (m_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// 作成するビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		// テクスチャがZバッファの場合は、最適なフォーマットにする
		if (m_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (m_desc.Format) {
				// 16ビット
			case DXGI_FORMAT_R16_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R16_UNORM;
				break;
				// 32ビット
			case DXGI_FORMAT_R32_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
				// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			case DXGI_FORMAT_R24G8_TYPELESS:
				srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			default:
				assert(0 && "[ShaderResource] 対応していないフォーマットです");
				break;
			}
		}
		// Zバッファでない場合は、そのまま同じフォーマットを使用
		else
		{
			srvDesc.Format = m_desc.Format;
		}

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (m_desc.ArraySize == 1)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = m_desc.MipLevels;
			if (srvDesc.Texture2D.MipLevels <= 0)srvDesc.Texture2D.MipLevels = -1;
		}
		// テクスチャ配列の場合
		else {
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = m_desc.MipLevels;
			srvDesc.Texture2DArray.ArraySize = m_desc.ArraySize;	// 要素数
			srvDesc.Texture2DArray.FirstArraySlice = 0;

			// さらにキューブマップの場合
			if (m_desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) {
				if (m_desc.ArraySize == 6)
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				}
				else
				{
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
					srvDesc.TextureCubeArray.NumCubes = m_desc.ArraySize / 6;
				}
			}
			// 通常テクスチャ配列
			else {
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			}
		}

		// シェーダリソースビュー作成
		HRESULT hr = D3D.GetDev()->CreateShaderResourceView(m_resource, &srvDesc, &m_srv);
		if (FAILED(hr))
		{
			assert(0 && "ShaderResourceViewの作成に失敗");
			return false;
		}

		// 「テクスチャ配列」の場合は、各要素の画像へのビューもつくっておくと便利
		if (m_desc.ArraySize > 1) {
			m_srvArray.resize(m_desc.ArraySize);
			for (UINT i = 0; i < m_desc.ArraySize; i++) {

				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.ArraySize = 1;			// 単品用なので1
				srvDesc.Texture2DArray.FirstArraySlice = i;		// ここが配列のインデックスになる

				//-------------------------------
				// i番目の画像のシェーダリソースビュー作成
				//-------------------------------
				HRESULT hr = D3D.GetDev()->CreateShaderResourceView(m_resource, &srvDesc, &m_srvArray[i]);
				if (FAILED(hr)) {
					Release();
					return false;
				}
			}
		}
	}

	//===========================================================
	//
	// DepthStencilViewを作成する
	//
	//===========================================================
	// Zバッファフラグがついてる時のみ
	if (m_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
		// 作成するビューの設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

		// テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
		switch (m_desc.Format) {
			// 16ビット
		case DXGI_FORMAT_R16_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D16_UNORM;
			break;
			// 32ビット
		case DXGI_FORMAT_R32_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			break;
			// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
		case DXGI_FORMAT_R24G8_TYPELESS:
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			assert(0 && "[DepthStencil] 対応していないフォーマットです");
			break;
		}

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (m_desc.ArraySize == 1) {
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		// テクスチャ配列の場合
		else {
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = m_desc.ArraySize;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		//-------------------------------
		// デプスステンシルビュー作成
		//-------------------------------
		HRESULT hr = D3D.GetDev()->CreateDepthStencilView(m_resource, &dsvDesc, &m_dsv);
		if (FAILED(hr)) {
			assert(0 && "DepthStencilViewの作成に失敗");
			return false;
		}

		// 「テクスチャ配列」の場合は、各要素の画像へのビューもつくっておくと便利
		if (m_desc.ArraySize > 1) {
			m_dsvArray.resize(m_desc.ArraySize);
			for (UINT i = 0; i < m_desc.ArraySize; i++) {

				dsvDesc.Texture2DArray.ArraySize = 1;			// 単品用なので1
				dsvDesc.Texture2DArray.FirstArraySlice = i;		// ここが配列のインデックスになる

				//-------------------------------
				// i番目の画像のデプスステンシルビュー作成
				//-------------------------------
				HRESULT hr = D3D.GetDev()->CreateDepthStencilView(m_resource, &dsvDesc, &m_dsvArray[i]);
				if (FAILED(hr)) {
					Release();
					return false;
				}
			}
		}

	}

	return true;
}
