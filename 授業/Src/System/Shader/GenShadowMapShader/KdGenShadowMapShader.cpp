#include "KdGenShadowMapShader.h"

bool KdGenShadowMapShader::Init()
{
	Release();

	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdGenShadowMap_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0], // 入力エレメント先頭アドレス
			layout.size(), // 入力エレメント数
			&compiledBuffer[0], // 頂点バッファのバイナリデータ
			sizeof(compiledBuffer), // 上記のバッファサイズ
			&m_inputLayout)) //
			) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// 頂点シェーダ(スキンメッシュ)
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdGenShadowMap_VS_Skin.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS_Skin))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SKININDEX", 0, DXGI_FORMAT_R16G16B16A16_UINT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0], // 入力エレメント先頭アドレス
			layout.size(), // 入力エレメント数
			&compiledBuffer[0], // 頂点バッファのバイナリデータ
			sizeof(compiledBuffer), // 上記のバッファサイズ
			&m_inputLayout_Skin)) //
			) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdGenShadowMap_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// 定数バッファ作成
	//-------------------------------------
	m_cb0.Create();

	//-------------------------------------
	// 深度マップ作製
	//-------------------------------------
	m_spotLightShadowMap = std::make_shared<KdTexture>();
	m_spotLightShadowMap->CreateRenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, SHADER.m_cb8_Light.GetWork().PL.size());
	m_spotLightZBuffer = std::make_shared<KdTexture>();
	m_spotLightZBuffer->CreateDepthStencil(1024, 1024, DXGI_FORMAT_R32_TYPELESS, SHADER.m_cb8_Light.GetWork().PL.size());

	m_dirLightShadowMap = std::make_shared<KdTexture>();
	m_dirLightShadowMap->CreateRenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, SHADER.m_cb8_Light.GetWork().DL.size() * 4);
	m_dirLightZBuffer = std::make_shared<KdTexture>();
	m_dirLightZBuffer->CreateDepthStencil(1024, 1024, DXGI_FORMAT_R32_TYPELESS, SHADER.m_cb8_Light.GetWork().DL.size() * 4);

	return true;
}

void KdGenShadowMapShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_VS_Skin);
	KdSafeRelease(m_inputLayout_Skin);

	KdSafeRelease(m_PS);
	
	m_cb0.Release();
}

void KdGenShadowMapShader::SetToDevice()
{
	// 頂点シェーダをセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	
	// ピクセルシェーダをセット
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);
	
	//---------------------
	// 定数バッファをセット
	//---------------------
	// オブジェクト定数バッファ
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
}

void KdGenShadowMapShader::DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials, bool isSkinMesh)
{
	if (mesh == nullptr)return;

	// 頂点シェーダセット
	if (isSkinMesh) // スキンメッシュ
	{
		D3D.GetDevContext()->VSSetShader(m_VS_Skin, 0, 0);
		// 頂点レイアウトをセット
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout_Skin);
	}
	else // スタティックメッシュ
	{
		// 頂点シェーダをセット
		D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
		// 頂点レイアウトをセット
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	}

	// 定数バッファ書き込み
	m_cb0.Write();

	// メッシュ情報をセット
	mesh->SetToDevice();

	// 全サブセット
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// 面が１枚も無い場合はスキップ
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		//マテリアル
			const KdMaterial* material = nullptr;
		material = materials[mesh->GetSubsets()[subi].MaterialNo].get();

		//-----------------------
		// テクスチャセット
		//-----------------------
		ID3D11ShaderResourceView* srvs[1] = {};

		// BaseColor Texture
		srvs[0] = material->BaseColorTex->GetSRView();

		// セット
		D3D.GetDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);

		//-----------------------
		// サブセット描画
		//-----------------------
		mesh->DrawSubset(subi);
	}
}

void KdGenShadowMapShader::Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform)
{
	if (nodeOL == nullptr)return;

	//--------------------------
	// ボーンスキニング行列用のバッファをセットする
	//--------------------------
	if (nodeOL->GetBoneStructuredBuffer().GetWork().size() >= 1)
	{
		D3D.GetDevContext()->VSSetShaderResources(100, 1, nodeOL->GetBoneStructuredBuffer().GetSRViewAddress());
	}

	//--------------------------
	// 全ノード(メッシュ)を描画
	//--------------------------
	for (auto&& meshIdx : nodeOL->GetMeshNodeIndices())
	{
		auto& node = nodeOL->GetAllNodes()[meshIdx];

		// 行列セット
		m_cb0.Work().mW = node.Transform * worldTransform;
		m_cb0.Write();

		// メッシュ描画
		DrawMesh(node.Mesh.get(), materials, node.Mesh->IsSkinMesh());
	}
}