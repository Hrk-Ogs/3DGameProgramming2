#include "System/KdSystem.h"

#include "KdMesh.h"

#include "KdGLTFLoader.h"

//=============================================================
//
// KdNodeOutliner
//
//=============================================================


// �m�[�h���Z�b�g

void KdNodeOutliner::SetNodes(const std::vector<Node>& nodes)
{
	Release();

	// �S�m�[�h�R�s�[
	m_allNodes = nodes;

	// 
	for (UINT ni = 0; ni < m_allNodes.size(); ni++)
	{
		// ���[�g�m�[�h��Index���X�g
		if (m_allNodes[ni].Parent == -1)m_rootNodeIndices.push_back(ni);

		// �{�[���m�[�h��Index���X�g
		int boneIdx = m_allNodes[ni].BoneIndex;
		if (boneIdx >= 0)
		{
			if (boneIdx >= (int)m_boneNodeIndices.size())m_boneNodeIndices.resize(boneIdx + 1);
			m_boneNodeIndices[boneIdx] = ni;
		}

		// ���b�V���������Ă�m�[�h��Index���X�g
		if (m_allNodes[ni].Mesh)
		{
			m_meshNodeIndices.push_back(ni);
		}

	}

	// �{�[���s��o�b�t�@�쐬
	m_sbBones.Create(m_boneNodeIndices.size(), nullptr);

}

void KdNodeOutliner::CalcNodeMatrices(Node* node)
{
	// �e�����݂���
	if (node->Parent >= 0)
	{
		node->Transform = node->LocalTransform * m_allNodes[node->Parent].Transform;
	}
	// �e�����݂��Ȃ�
	else
	{
		node->Transform = node->LocalTransform;
	}

	// �{�[���̏ꍇ�́A�`��p�̍s����v�Z���Ă���
	if (node->BoneIndex >= 0)
	{
		m_sbBones.Work()[node->BoneIndex] = node->BoneInverseBindMatrix * node->Transform;
	}

	// �q�ċA
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

	// ���f���t�@�C���̂���t�H���_���擾
	std::string nowDir = KdGetDirFromPath(filename);
	nowDir += "/";

	//-------------------------------
	// glTF���f���ǂݍ���
	//-------------------------------
	std::shared_ptr<KdGLTFModel> model = KdLoadGLTFModel(filename);
	if (model == nullptr)return false;

	//-------------------------------
	// �S�}�e���A�����쐬
	//-------------------------------
	m_materials.resize(model->Materials.size());
	for (UINT mti = 0; mti < m_materials.size(); mti++)
	{
		// GLTF�}�e���A���f�[�^����K�v�ȃf�[�^���R�s�[�Ȃǂ�����
		const KdGLTFMaterial& gltfMaterial = model->Materials[mti];

		// �������m��
		m_materials[mti] = std::make_shared<KdMaterial>();

		// ���O
		m_materials[mti]->Name = gltfMaterial.Name;

		// ��{�F
		m_materials[mti]->BaseColor = gltfMaterial.BaseColor;
		m_materials[mti]->BaseColorTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->BaseColorTex->Load(nowDir + gltfMaterial.BaseColorTexture) == false)
		{
			// �ǂݍ��߂Ȃ������ꍇ�́A����ɔ��摜���g�p
			m_materials[mti]->BaseColorTex = D3D.GetWhiteTex();
		}

		// �������E�e��
		m_materials[mti]->Metallic = gltfMaterial.Metallic;
		m_materials[mti]->Roughness = gltfMaterial.Roughness;
		m_materials[mti]->MetallicRoughnessTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->MetallicRoughnessTex->Load(nowDir + gltfMaterial.MetallicRoughnessTexture) == false)
		{
			// �ǂݍ��߂Ȃ������ꍇ�́A����ɔ��摜���g�p
			m_materials[mti]->MetallicRoughnessTex = D3D.GetWhiteTex();
		}

		// ���Ȕ���
		m_materials[mti]->Emissive = gltfMaterial.Emissive;
		m_materials[mti]->EmissiveTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->EmissiveTex->Load(nowDir + gltfMaterial.EmissiveTexture) == false)
		{
			// �ǂݍ��߂Ȃ������ꍇ�́A����ɔ��摜���g�p
			m_materials[mti]->EmissiveTex = D3D.GetWhiteTex();
		}

		// �@���}�b�v
		m_materials[mti]->NormalTex = std::make_shared<KdTexture>();
		if (m_materials[mti]->NormalTex->Load(nowDir + gltfMaterial.NormalTexture) == false)
		{
			// �ǂݍ��߂Ȃ������ꍇ�́A����ɔ��摜���g�p
			m_materials[mti]->NormalTex = D3D.GetDefNormalTex();
		}

	}

	//-------------------------------
	// �m�[�h�A�E�g���C�i�[�쐬
	//-------------------------------

	// �m�[�h�쐬
	std::vector<KdNodeOutliner::Node>	nodes;
	nodes.resize(model->Nodes.size());

	// �m�[�h�f�[�^�Z�b�g
	for (UINT ni = 0; ni < model->Nodes.size(); ni++)
	{
		// glTF�m�[�h�f�[�^
		const auto* srcNode = &model->Nodes[ni];

		// �m�[�h���Z�b�g
		nodes[ni].Name = srcNode->Name;
		nodes[ni].NodeIndex = ni;
		nodes[ni].LocalTransform = srcNode->LocalTransform;
		nodes[ni].Transform = srcNode->WorldTransform;
		nodes[ni].BoneInverseBindMatrix = srcNode->InverseBindMatrix;
		nodes[ni].Parent = srcNode->Parent;
		nodes[ni].Children = srcNode->Children;
		
		// ���b�V���H
		if (srcNode->IsMesh)
		{
			// ���b�V���쐬
			KdSptr<KdMesh> mesh = std::make_shared<KdMesh>();

			mesh->Create(srcNode->Mesh.VertexData, srcNode->Mesh.Faces, srcNode->Mesh.Subsets);

			nodes[ni].Mesh = mesh;
		}

	}

	//-------------------------------
	// �{�[���m�[�h�݂̂̏���
	//-------------------------------
	for (UINT bi = 0; bi < model->BoneNodeIndices.size(); bi++)
	{
		int nodeIdx = model->BoneNodeIndices[bi];
		// glTF�m�[�h�f�[�^
//		const auto* srcNode = &model->Nodes[nodeIdx];

		// �{�[��Index
		nodes[nodeIdx].BoneIndex = bi;
	}

	//-------------------------------
	// �Z�b�g
	//-------------------------------
	m_nodeOutliner.SetNodes(nodes);

	//-------------------------------
	// �A�j���[�V�����f�[�^
	//-------------------------------
	m_animations = model->Animations;

	// 
	m_filepath = filename;

	return true;
}
