#pragma once

#include "ColliderComponent.h"

//========================================================
//
// ���R���C�_�[�R���|�[�l���g
//
//========================================================
class SphereColliderComponent : public ColliderComponent
{
public:

	// ���ϊ������擾
	const DirectX::BoundingSphere& GetLocalSphere() const { return m_localSphere; }
	// �ϊ��ς݋����擾
	const DirectX::BoundingSphere& GetWorldSphere() const { return m_worldSphere; }

	// ���b�V���Ƃ̔��莞�A�ڍׂȃq�b�g�ʒu�����擾���邩
	bool IsMeshDetailHitPos() const { return m_meshDetailHitPos; }
	void MeshDetailHitPos(bool enable) { m_meshDetailHitPos = enable; }

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const KdVec3& pos, float radius, bool doJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = doJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		// �����
		m_localSphere.Center = pos;
		m_worldSphere.Center = pos;
		m_localSphere.Radius = radius;
		m_worldSphere.Radius = radius;
	}

	// 
	void MoveSphere(const KdVec3& v)
	{
		(KdVec3&)m_worldSphere.Center += v;
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

		KdJsonGet( jsonData["Center"], (KdVec3&)m_localSphere.Center);
		KdJsonGet(jsonData["Radius"], m_localSphere.Radius);
		KdJsonGet(jsonData["MeshDetailHitPos"], m_meshDetailHitPos);
	}

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["Center"] = KdVec3(m_localSphere.Center).ToArray();
		outJsonObj["Radius"] = m_localSphere.Radius;
		outJsonObj["MeshDetailHitPos"] = m_meshDetailHitPos;
	}

private:

	// ���ݒ�p�f�[�^
	DirectX::BoundingSphere m_localSphere;

	// ���ۂɔ���Ŏg�p���鋅�f�[�^(m_localPos��m_radius��Transform�̍s��ŕϊ���������)
	// �Ȃ̂Ń��A���^�C���ɕω�����
	DirectX::BoundingSphere m_worldSphere;

	// ���b�V���R���C�_�[�Ɣ��莞�A�S�q�b�g�ʒu���擾���邩
	bool					m_meshDetailHitPos = false;
};
