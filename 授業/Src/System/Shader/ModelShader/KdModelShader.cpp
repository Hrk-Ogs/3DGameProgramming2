#include "System/KdSystem.h"

#include "KdModelShader.h"

bool KdModelShader::Init()
{
	//--------------------------------
	// 頂点シェーダー
	//--------------------------------
	{
		#include "KdModelShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// 1頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点レイアウト作成
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

	//--------------------------------
	// ピクセルシェーダー
	//--------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdModelShader_PS.inc"
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//--------------------------------
	// 定数バッファ作成
	//--------------------------------
	m_cb0.Create();
	m_cb1_Material.Create();

	return true;
}

void KdModelShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_PS);

	m_cb0.Release();
	m_cb1_Material.Release();
}

void KdModelShader::SetToDevice()
{
	// 頂点シェーダをセット
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// ピクセルシェーダをセット
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	//--------------------------
	// 定数バッファをセット
	//--------------------------
	// オブジェクト定数バッファ（0番目にセット）
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	// マテリアル定数バッファ
	D3D.GetDevContext()->VSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
}

void KdModelShader::DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials)
{
	if (mesh == nullptr)return;

	// メッシュ情報をセット
	mesh->SetToDevice();

	// 全サブセット
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// 面が１枚も無い場合はスキップ
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		// マテリアル
		const KdMaterial& material = *materials[mesh->GetSubsets()[subi].MaterialNo];

		//---------------------------
		// マテリアル情報を定数バッファへ書き込む
		//---------------------------
		// 基本色
		m_cb1_Material.Work().BaseColor = material.BaseColor;
		// 発光色
		m_cb1_Material.Work().Emissive = material.Emissive;
		// 金属性
		m_cb1_Material.Work().Matallic = material.Metallic;
		// 粗さ
		m_cb1_Material.Work().Roughness = material.Roughness;
		// GPUへ転送
		m_cb1_Material.Write();

		//------------------------------
		// テクスチャセット
		//------------------------------
		// 複数毎のテクスチャを一気に渡すように、配列を用意
		std::array<ID3D11ShaderResourceView*, 4> srvs = {};

		// BaseColorテクスチャ
		srvs[0] = material.BaseColorTex->GetSRView();
		// Emissiveテクスチャ
		srvs[1] = material.EmissiveTex->GetSRView();
		// Metallic Roughness
		srvs[2] = material.MetallicRoughnessTex->GetSRView();
		// NormalMap
		srvs[3] = material.NormalTex->GetSRView();

		// セット 0番目のスロットから4枚分セットする
		D3D.GetDevContext()->PSSetShaderResources(0, srvs.size(), &srvs[0]);

		// サブセット描画
		mesh->DrawSubset(subi);
	}
}

void KdModelShader::Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform)
{
	if (nodeOL == nullptr)return;

	//--------------------------
	// 全ノード(メッシュ)を描画
	//--------------------------
	// メッシュが入っているノードのIndex全て
	for (auto&& meshIdx : nodeOL->GetMeshNodeIndices())
	{
		// 実際のメッシュの入っているノード
		auto& node = nodeOL->GetAllNodes()[meshIdx];

		// 定数バッファにデータを書き込む
		m_cb0.Work().mW = node.Transform * worldTransform;
		// コレで実際にGPU側のバッファへ書き込まれ、シェーダーで使用出来るようになる
		m_cb0.Write();

		// メッシュ描画
		DrawMesh(node.Mesh.get(), materials);
	}
}
