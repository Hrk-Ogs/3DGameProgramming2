#pragma once

#include "ColliderComponent.h"

//========================================================
//
// ���C�R���C�_�[�R���|�[�l���g
//
//========================================================
class RayColliderComponent : public ColliderComponent
{
public:

	// ���C���W�擾
	const KdVec3& GetLocalRayPos() const { return m_localPos; }
	// ���C�����擾
	const KdVec3& GetLocalRayDir() const { return m_localDir; }

	// ����Ɏg�p����郌�C���W�擾
	const KdVec3& GetRayPos() const { return m_rayPos; }
	// ����Ɏg�p����郌�C�����擾
	const KdVec3& GetRayDir() const { return m_rayDir; }
	// ����Ɏg�p����郌�C�̒����擾
	float GetRayLen() const { return m_rayLen; }

	// ���Z�b�g(GameObject�ɃA�^�b�`�����A�R���C�_�[�P�i�Ŏg�p����ꍇ�Ɏg�p)
	void Set(const KdVec3& pos, const KdVec3& dir, bool isDoJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = isDoJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_rayPos = m_localPos = pos;
		m_rayDir = m_localDir = dir;
		m_rayLen = dir.Length();

		m_rayDir.Normalize();
	}

	// 
	void MoveRay(const KdVec3& v)
	{
		m_rayPos += v;
	}

	//------------------------------------------------
	// ���菈���֐�
	//------------------------------------------------

	// �����蔻��@���� vs obj
	//Vister�p�^�[��
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

		KdJsonGet(jsonData["RayPos"], m_localPos);
		KdJsonGet(jsonData["RayDir"], m_localDir);
	}

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["RayPos"] = m_localPos.ToArray();
		outJsonObj["RayDir"] = m_localDir.ToArray();
	}

private:

	// ���C�ݒ�p�f�[�^
	KdVec3	m_localPos;		// ���[�J�����W
	KdVec3	m_localDir;		// ���[�J���x�N�g��(�������g�p)

	// ���ۂɔ���Ŏg�p���郌�C�f�[�^(m_localPos��m_localDir��Transform�̍s��ŕϊ���������)
	// �Ȃ̂Ń��A���^�C���ɕω�����
	KdVec3	m_rayPos;		// �ʒu
	KdVec3	m_rayDir;		// ����(���K��)
	float	m_rayLen;		// ���C�̒���

};
