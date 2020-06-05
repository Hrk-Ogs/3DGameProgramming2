#include "System/KdSystem.h"

#include "KdMesh.h"

#include "KdGLTFLoader.h"

//=============================================================
//
// KdNodeOutliner
//
//=============================================================


// ノードをセット

void KdNodeOutliner::SetNodes(const std::vector<Node>& nodes)
{
	Release();

	// 全ノードコピー
	m_allNodes = nodes;

	// 
	for (UINT ni = 0; ni < m_allNodes.size(); ni++)
	{
		// ルートノードのIndexリスト
		if (m_allNodes[ni].Parent == -1)m_rootNodeIndices.push_back(ni);

		// ボーンノードのIndexリスト
		int boneIdx = m_allNodes[ni].BoneIndex;
		if (boneIdx >= 0)
		{
			if (boneIdx >= (int)m_boneNodeIndices.size())m_boneNodeIndices.resize(boneIdx + 1);
			m_boneNodeIndices[boneIdx] = ni;
		}

		// メッシュをもってるノードのIndexリスト
		if (m_allNodes[ni].Mesh)
		{
			m_meshNodeIndices.push_back(ni);
		}

	}

	// ボーン行列バッファ作成
	m_sbBones.Create(m_boneNodeIndices.size(), nullptr);

}

void KdNodeOutliner::CalcNodeMatrices(Node* node)
{
	// 親が存在する
	if (node->Parent >= 0)
	{
		node->Transform = node->LocalTransform * m_allNodes[node->Parent].Transform;
	}
	// 親が存在しない
	else
	{
		node->Transform = node->LocalTransform;
	}

	// ボーンの場合は、描画用の行列を計算しておく
	if (node->BoneIndex >= 0)
	{
		m_sbBones.Work()[node->BoneIndex] = node->BoneInverseBindMatrix * node->Transform;
	}

	// 子再帰
	for (auto&& childIndex : node->Children)
	{
		CalcNodeMatrices(&m_allNodes[childIndex]);
	}
}

void KdNodeOutliner::CalcNodeMatrices()
{
	if (m_allNodes.size() == 0)return;

	for (int nodeIdx : m_rootNodeIndices)
	{
		CalcNodeMatrices(&m_allNodes[nodeIdx]);
	}
}

//=============================================================
//
// KdModel
//
//=============================================================

bool KdModel::Load(const std::string& filename)
{
	Release();

	// モデルファイルのあるフォルダ名取得
	std::string nowDir = KdGetDirFromPath(filename);
	nowDir += "/";

	//-------------------------------
	// glTFモデル読み込み
	//-------------------------------
	std::shared_ptr<KdGLTFModel> model = KdLoadGLTFModel(filename);
	if (model == nullptr)return false;

	//-------------------------------
	// 全マテリアル情報作成
	//-------------------------------
	m_materials.resize(model->Materials.size());
	for (UINT mti = 0; mti < m_materials.size(); mti++)
	{
		// GLTFマテリアルデータから必要なデータをコピーなどをする
		const KdGLTFMaterial& gltfMaterial = model->Materials[mti];

		// メモリ確保
		m_materials[mti] = std::make_shared<KdMaterial>();

		// 名前
		m_materials[mti]->Name = gltfMaterial.Name;

		// 基本色
		m_materials[mti]->BaseColor = gltfMaterial.BaseColor;
		m_materials[mti]->BaseColorTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->BaseColorTex->Load(nowDir + gltfMaterial.BaseColorTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			m_materials[mti]->BaseColorTex = D3D.GetWhiteTex();
		}

		// 金属性・粗さ
		m_materials[mti]->Metallic = gltfMaterial.Metallic;
		m_materials[mti]->Roughness = gltfMaterial.Roughness;
		m_materials[mti]->MetallicRoughnessTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->MetallicRoughnessTex->Load(nowDir + gltfMaterial.MetallicRoughnessTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			m_materials[mti]->MetallicRoughnessTex = D3D.GetWhiteTex();
		}

		// 自己発光
		m_materials[mti]->Emissive = gltfMaterial.Emissive;
		m_materials[mti]->EmissiveTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->EmissiveTex->Load(nowDir + gltfMaterial.EmissiveTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			m_materials[mti]->EmissiveTex = D3D.GetWhiteTex();
		}

		// 法線マップ
		m_materials[mti]->NormalTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->NormalTex->Load(nowDir + gltfMaterial.NormalTexture) == false)
		{
			// 読み込めなかった場合は、代わりに白画像を使用
			m_materials[mti]->NormalTex = D3D.GetDefNormalTex();
		}

	}

	//-------------------------------
	// ノードアウトライナー作成
	//-------------------------------

	// ノード作成
	std::vector<KdNodeOutliner::Node>	nodes;
	nodes.resize(model->Nodes.size());

	// ノードデータセット
	for (UINT ni = 0; ni < model->Nodes.size(); ni++)
	{
		// glTFノードデータ
		const auto* srcNode = &model->Nodes[ni];

		// ノード情報セット
		nodes[ni].Name = srcNode->Name;
		nodes[ni].NodeIndex = ni;
		nodes[ni].LocalTransform = srcNode->LocalTransform;
		nodes[ni].Transform = srcNode->WorldTransform;
		nodes[ni].BoneInverseBindMatrix = srcNode->InverseBindMatrix;
		nodes[ni].Parent = srcNode->Parent;
		nodes[ni].Children = srcNode->Children;
		
		// メッシュ？
		if (srcNode->IsMesh)
		{
			// メッシュ作成
			KdSptr<KdMesh> mesh = std::make_shared<KdMesh>();

			mesh->Create(srcNode->Mesh.VertexData, srcNode->Mesh.Faces, srcNode->Mesh.Subsets);

			nodes[ni].Mesh = mesh;
		}

	}

	//-------------------------------
	// ボーンノードのみの処理
	//-------------------------------
	for (UINT bi = 0; bi < model->BoneNodeIndices.size(); bi++)
	{
		int nodeIdx = model->BoneNodeIndices[bi];
		// glTFノードデータ
//		const auto* srcNode = &model->Nodes[nodeIdx];

		// ボーンIndex
		nodes[nodeIdx].BoneIndex = bi;
	}

	//-------------------------------
	// セット
	//-------------------------------
	m_nodeOutliner.SetNodes(nodes);

	//-------------------------------
	// アニメーションデータ
	//-------------------------------
	m_animations = model->Animations;

	// 
	m_filepath = filename;

	return true;
}
