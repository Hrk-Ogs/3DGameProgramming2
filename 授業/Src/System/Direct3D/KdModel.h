#pragma once

//==========================================================
//
// �m�[�h�A�E�g���C�i�[
// �EBlender�̃A�E�g���C�i�[�E�B���h�E�̂悤�ɁA�����̃m�[�h��ێ�����N���X�B�K�w�\���ɂ��Ȃ��Ă���B
// �EBlender�Ń��b�V�����t���Ă���m�[�h�ɂ́A������KdMesh���t���Ă���B
//
//==========================================================
class KdNodeOutliner
{
public:

	//---------------------
	// �P�̃m�[�h
	//---------------------
	struct Node
	{
		// �m�[�h��(�{�[����)
		std::string				Name;

		// �擪���牽�Ԗڂ̃m�[�h���H
		int						NodeIndex = -1;
		// �{�[���m�[�h�̎��A�擪���牽�Ԗڂ̃{�[�����H
		int						BoneIndex = -1;

		// �ϊ��s��
		KdMatrix				LocalTransform;
		KdMatrix				Transform;
		KdMatrix				BoneInverseBindMatrix;	// �{�[���I�t�Z�b�g�s��

		// ���b�V��
		KdSptr<KdMesh>			Mesh;

		// �qIndex���X�g
		std::vector<int>		Children;
		// �eIndex
		int						Parent = -1;
	};

	// �S�m�[�h�̔z��擾
	const std::vector<Node>&	GetAllNodes() const { return m_allNodes; }
	std::vector<Node>&			GetAllNodes() { return m_allNodes; }

	// Root�m�[�h�݂̂̃m�[�hIndex�z��擾
	const std::vector<int>&		GetRootNodeIndices() const { return m_rootNodeIndices; }

	// �{�[���݂̂̃m�[�hIndex�z��擾
	const std::vector<int>&		GetBoneNodeIndices() const { return m_boneNodeIndices; }

	// ���b�V�������݂���m�[�hIndex�z��擾
	const std::vector<int>&		GetMeshNodeIndices() const { return m_meshNodeIndices; }

	// �m�[�h���Z�b�g
	void SetNodes(const std::vector<Node>& nodes);

	// 
	const KdStructuredBuffer<KdMatrix>& GetBoneStructuredBuffer() const { return m_sbBones; }

	// ���
	void Release()
	{
		m_allNodes.clear();
		m_rootNodeIndices.clear();
		m_boneNodeIndices.clear();
		m_meshNodeIndices.clear();

		m_sbBones.Release();
	}

	// �w�薼�̃m�[�h������
	Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_allNodes)
		{
			if (node.Name == name)return &node;
		}
		return nullptr;
	}

	// �m�[�h�s����v�Z(�w��node����v�Z)
	void CalcNodeMatrices(Node* node);
	// �m�[�h�s����v�Z(Root����v�Z)
	// �E�e����q�ւƁA�ǂ�ǂ�v�Z���Ă���
	void CalcNodeMatrices();

	// �{�[���m�[�h�̑S�`��p�s����o�b�t�@�ɏ�������
	void WriteToBoneBuffer()
	{
		m_sbBones.Write();
	}

	// 
	KdNodeOutliner() = default;

	//=================================
	// �R�s�[
	//=================================
	void operator=(const KdNodeOutliner& src)
	{
		SetNodes(src.GetAllNodes());
	}
	KdNodeOutliner(const KdNodeOutliner& src)
	{
		SetNodes(src.GetAllNodes());
	}

private:

	//----------------------------
	// �S�Ẵm�[�h�z��
	//----------------------------
	std::vector<Node>				m_allNodes;

		// �S�m�[�h���ARoot�m�[�h�݂̂�Index�z��
		std::vector<int>				m_rootNodeIndices;
		// �S�m�[�h���A�{�[���m�[�h�݂̂�Index�z��
		std::vector<int>				m_boneNodeIndices;
		// �S�m�[�h���A���b�V�������݂���m�[�h�݂̂�Indexn�z��
		std::vector<int>				m_meshNodeIndices;

	// �{�[���ϊ��s��o�b�t�@
	KdStructuredBuffer<KdMatrix>		m_sbBones;
};

//==========================================================
// �}�e���A��(glTF�x�[�X��PBR�}�e���A��)
//==========================================================
struct KdMaterial
{
	//---------------------------------------
	// �ގ��f�[�^
	//---------------------------------------

	// ���O
	std::string					Name;

	// ��{�F
	std::shared_ptr<KdTexture>	BaseColorTex;				// ��{�F�e�N�X�`��
	KdVec4						BaseColor = { 1,1,1,1 };	// ��{�F�̃X�P�[�����O�W��(RGBA)

	// �������A�e��
	std::shared_ptr<KdTexture>	MetallicRoughnessTex;		// B:������ G:�e��
	float						Metallic = 0.0f;			// �������̃X�P�[�����O�W��
	float						Roughness = 1.0f;			// �e���̃X�P�[�����O�W��

	// ���Ȕ���
	std::shared_ptr<KdTexture>	EmissiveTex;				// ���Ȕ����e�N�X�`��
	KdVec3						Emissive = { 0,0,0 };		// ���Ȕ����̃X�P�[�����O�W��(RGB)

	// �@���}�b�v
	std::shared_ptr<KdTexture>	NormalTex;
	std::shared_ptr<KdTexture>	ForceUseNormalTex;

	KdMaterial()
	{
		BaseColorTex			= D3D.GetWhiteTex();
		MetallicRoughnessTex	= D3D.GetWhiteTex();
		EmissiveTex				= D3D.GetWhiteTex();
		NormalTex				= D3D.GetDefNormalTex();
	}
};

//==========================================================
//
// �m�[�h����}�e���A���z���g�ݍ��킹�A�P�̃��f���f�[�^�Ƃ����N���X
// �EBlender�̂悤�ɁA�m�[�h�͕������݂���B�܂��K�w�\���ɂ��Ȃ��Ă���(m_nodeOutliner)�B
// �EBlender�Ń��b�V�����t���Ă���m�[�h�ɂ́A������KdMesh���t���Ă���B
// �E�}�e���A���̔z��������Ă���B
// �E�A�j���[�V�����̔z��������Ă���B
//
//==========================================================
class KdModel
{
public:

	//==========================================================
	// �擾
	//==========================================================

	// �L���ȃ��b�V�����H
	bool IsValid() const { return m_nodeOutliner.GetAllNodes().empty() == false; }

	// �m�[�h�A�E�g���C�i�[�擾
	const KdNodeOutliner& GetNodeOutlinear() const { return m_nodeOutliner; }

	// �}�e���A���z��擾
	const std::vector<KdSptr<KdMaterial>>& GetMaterials() const { return m_materials; }


	// �A�j���[�V�������X�g�擾
	const std::vector<KdSptr<KdAnimationData>>& GetAnimations() const { return m_animations; }

	// �A�j���[�V�����𖼑O����擾
	const KdSptr<KdAnimationData> GetAnimation(const std::string& animeName) const
	{
		for (auto&& anim : m_animations)
		{
			if (anim->GetName() == animeName)return anim;
		}
		return nullptr;
	}

	// �t�@�C���p�X�擾
	const std::string& GetFilepath() const { return m_filepath; }

	//==========================================================
	// �쐬�E���
	//==========================================================

	// glTF�`����3D���f�������[�h����
	// �Efilename		�c �t�@�C����
	// �߂�l			�c �����Ftrue
	bool Load(const std::string& filename);

	// ���
	void Release()
	{
		m_materials.clear();
		m_filepath = "";
		m_animations.clear();
		m_nodeOutliner.Release();
	}

	~KdModel() {
		Release();
	}

	// 
	KdModel() {}

private:

	// �m�[�h�A�E�g���C�i�[
	// �E�S�m�[�h���Ǘ����Ă���B���b�V�������̒��̂����ꂩ�ɓ����Ă���B
	KdNodeOutliner							m_nodeOutliner;

	// �}�e���A���z��
	std::vector<KdSptr<KdMaterial>>			m_materials;

	// �A�j���[�V�����f�[�^���X�g
	std::vector<KdSptr<KdAnimationData>>	m_animations;

	// �t�@�C���p�X
	std::string								m_filepath;

private:
	// �R�s�[�֎~�p
	KdModel(const KdModel& src) = delete;
	void operator=(const KdModel& src) = delete;
};
