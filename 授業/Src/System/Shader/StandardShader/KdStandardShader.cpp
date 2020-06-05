#include "System/KdSystem.h"

#include "KdStandardShader.h"


bool KdStandardShader::Init()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "KdStandardShader_VS.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,		2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0],			// 入力エレメント先頭アドレス
			layout.size(),		// 入力エレメント数
			&compiledBuffer[0],				// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),			// 上記のバッファサイズ
			&m_inputLayout))					// 
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
		#include "KdStandardShader_VS_Skin.inc"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS_Skin))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,			0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,				1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,			2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,			3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,			4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKININDEX",    0, DXGI_FORMAT_R16G16B16A16_UINT,		5,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKINWEIGHT",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	6,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0],			// 入力エレメント先頭アドレス
			layout.size(),		// 入力エレメント数
			&compiledBuffer[0],				// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),			// 上記のバッファサイズ
			&m_inputLayout_Skin))					// 
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
		#include "KdStandardShader_PS.inc"

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
	m_cb1_Material.Create();
	m_cb2_Bone.Create();

	return true;
}

void KdStandardShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);

	KdSafeRelease(m_VS_Skin);
	KdSafeRelease(m_inputLayout_Skin);

	KdSafeRelease(m_PS);
	m_cb0.Release();
	m_cb1_Material.Release();
	m_cb2_Bone.Release();
}

void KdStandardShader::SetToDevice()
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
	// マテリアル定数バッファ
	D3D.GetDevContext()->VSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	// ボーン定数バッファ
	D3D.GetDevContext()->VSSetConstantBuffers(2, 1, m_cb2_Bone.GetAddress());
//	D3D.GetDevContext()->PSSetConstantBuffers(2, 1, m_cb2_Bone.GetAddress());
}

void KdStandardShader::DrawMesh(const KdMesh* mesh, const std::vector< KdSptr<KdMaterial>>& materials, bool isSkinMesh)
{
	if (mesh == nullptr)return;

	// 頂点シェーダセット
	if (isSkinMesh)	// スキンメッシュ
	{
		D3D.GetDevContext()->VSSetShader(m_VS_Skin, 0, 0);
		// 頂点レイアウトをセット
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout_Skin);

		// ボーン定数バッファ書き込み
		m_cb2_Bone.Write();
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

		// マテリアル
		const KdMaterial* material = nullptr;

		// マテリアル範囲チェック
		if (mesh->GetSubsets()[subi].MaterialNo < materials.size())
		{
			material = materials[mesh->GetSubsets()[subi].MaterialNo].get();
		}
		else // 範囲外
		{
			material = &m_nullMaterial;
		}


		//-----------------------
		// マテリアル情報を定数バッファへ書き込む
		//-----------------------
		m_cb1_Material.Work().BaseColor = material->BaseColor;
		m_cb1_Material.Work().Emissive = material->Emissive;
		m_cb1_Material.Work().Metallic = material->Metallic;
		m_cb1_Material.Work().Roughness = material->Roughness;
		m_cb1_Material.Write();

		//-----------------------
		// テクスチャセット
		//-----------------------

		ID3D11ShaderResourceView* srvs[4] = {};

		// BaseColor
		srvs[0] = material->BaseColorTex->GetSRView();
		// Emissive
		srvs[1] = material->EmissiveTex->GetSRView();
		// Metallic Roughness
		srvs[2] = material->MetallicRoughnessTex->GetSRView();
		// NormalMap
		srvs[3] = material->NormalTex->GetSRView();

		// セット
		D3D.GetDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);

		//-----------------------
		// サブセット描画
		//-----------------------
		mesh->DrawSubset(subi);
	}
}

void KdStandardShader::Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform)
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
		SetWorldMatrix(node.Transform * worldTransform);

		// メッシュ描画
		DrawMesh(node.Mesh.get(), materials, node.Mesh->IsSkinMesh());
	}
}
