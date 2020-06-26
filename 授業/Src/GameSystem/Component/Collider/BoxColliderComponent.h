#pragma once

#include "ColliderComponent.h"

//========================================================
//
// ���R���C�_�[�R���|�[�l���g
//
//========================================================
class BoxColliderComponent : public ColliderComponent
{
public:

	// ���ϊ�BOX���擾
	const DirectX::BoundingOrientedBox& GetLocalBox() const { return m_localBox; }
	// �ϊ��ς�BOX���擾
	const DirectX::BoundingOrientedBox& GetWorldBox() const { return m_worldBox; }

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const KdVec3& pos, const KdVec3& size, const KdMatrix& rotaMat, bool isDoJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = isDoJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_localBox.Center = pos;
		m_localBox.Extents = size;

		m_localBox.Transform(m_worldBox, rotaMat);
	}

	// 
	void MoveBox(const KdVec3& v)
	{
		(KdVec3&)m_worldBox.Center += v;
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

		KdJsonGet(jsonData["Center"], (KdVec3&)m_localBox.Center);
		KdJsonGet(jsonData["Extents"], (KdVec3&)m_localBox.Extents);
	}

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["Center"] = KdVec3(m_localBox.Center).ToArray();
		outJsonObj["Extents"] = KdVec3(m_localBox.Extents).ToArray();
	}

private:

	// ���ݒ�f�[�^
	DirectX::BoundingOrientedBox	m_localBox;

	// ���ۂɔ���Ŏg�p���鋅�f�[�^(m_localPos��m_radius��Transform�̍s��ŕϊ���������)
	// �Ȃ̂Ń��A���^�C���ɕω�����
	DirectX::BoundingOrientedBox	m_worldBox;

};
