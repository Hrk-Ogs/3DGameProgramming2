#pragma once

#include "ColliderComponent.h"

//========================================================
//
// ���b�V���R���C�_�[�R���|�[�l���g
//
//========================================================
class MeshColliderComponent : public ColliderComponent
{
public:

	// ���f���f�[�^�擾
	const KdSptr<KdModel>& GetModel() const { return m_model; }
	// Transform�擾
	const KdMatrix& GetMeshTransform() const { return m_transform; }

	// ���f���ǂݍ���
	void LoadModel(const std::string& filename);

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const std::string& filename, const KdMatrix& mat, bool isDoJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = isDoJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_transform = mat;

		LoadModel(filename);
	}

	// 
	void Move(const KdVec3& v)
	{
		m_transform.Move(v);
	}

	//------------------------------------------------
	// ���菈���֐�
	//------------------------------------------------

	// �����蔻��@���� vs obj
	// Vister�p�^�[��
	virtual bool HitTest(ColliderComponent* obj) override
	{
		return obj->HitTest(this);
	}
	virtual bool HitTest(SphereColliderComponent* sph) override;
	virtual bool HitTest(RayColliderComponent* ray) override;
	virtual bool HitTest(BoxColliderComponent* box) override;
	virtual bool HitTest(MeshColliderComponent* mesh) override;

	// ����f�[�^�X�V
	virtual void UpdateCollisionData() override;

	//------------------------------------------------
	// �R���|�[�l���g�N���X�̊֐�����
	//------------------------------------------------

	// �X�V����
	virtual void Update() override;
	// �`�揀������
	virtual void PrepareDraw(RenderingData& rdata) override;

	// (Editor)
	virtual void Editor_ImGuiUpdate() override;

	//------------------------------------------------
	// Serialize / Deserialize
	//------------------------------------------------
	virtual void Deserialize(const json11::Json& jsonData) override
	{
		ColliderComponent::Deserialize(jsonData);

		std::string filename;
		KdJsonGet(jsonData["ModelFilename"], filename);

		LoadModel(filename);
	}

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["ModelFilename"] = m_model->GetFilepath();
	}

private:

	// ���胂�f���f�[�^
	KdSptr<KdModel>		m_model = std::make_shared<KdModel>();
	KdMatrix			m_transform;	// Transform�̍s��
};
