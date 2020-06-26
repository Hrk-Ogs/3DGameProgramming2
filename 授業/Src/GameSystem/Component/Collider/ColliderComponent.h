#pragma once

#include "../BaseComponent.h"

class ColliderComponent;
class CollisionManager;

class SphereColliderComponent;
class RayColliderComponent;
class BoxColliderComponent;
class MeshColliderComponent;

//============================================
//
// ���茋�ʃf�[�^
//
//============================================
struct CollisionResult
{
	KdSptr<ColliderComponent> Collider;//�q�b�g����̃R���C�_�[�R���|�[�l���g

	KdVec3 Push;	// �����x�N�g��(�����݁A�ꕔ�̌`��ł̂ݐ��m�Ȓl�ɂȂ��Ă��܂�)
	float Dist = 0;	// �ŒZ����(�����݁A�ꕔ�̌`��ł̐��m�Ȓl�ɂȂ��Ă��܂�)

	// �q�b�g�ʒu���X�g
	std::list<KdVec3>	HitPosList;


	// ���C vs ���b�V����p
	KdWptr<KdMesh>	Mesh;				// ���b�V��
	UINT			Mesh_FaceIdx = -1;	//�ʔԍ�
};

//============================================
//
// �R���C�_�[��{�N���X
//
//============================================
class ColliderComponent : public BaseComponent
{
	friend class CollisionManager;
public:

	//------------------------------------------------
	// ���菈���֐�
	//------------------------------------------------
	// �����蔻����s�@���� vs obj
	// Vister�p�^�[��
	virtual bool HitTest(ColliderComponent* obj) = 0;

	virtual bool HitTest(SphereColliderComponent* sph) = 0;
	virtual bool HitTest(RayColliderComponent* ray) = 0;
	virtual bool HitTest(BoxColliderComponent* box) = 0;
	virtual bool HitTest(MeshColliderComponent* mesh) = 0;

	// ����f�[�^�X�V
	virtual void UpdateCollisionData() = 0;

	//------------------------------------------------
	// �ݒ�f�[�^
	//------------------------------------------------

	// �^�O�擾
	const std::string& GetStrTag() const { return m_strTag; }
	// �^�O�ݒ�
	void SetStrTag(std::string_view tag) { m_strTag = tag; }

	// ����̂ݍs���t���O�Z�b�g
	void SetTrigger(bool enable) { m_isTrigger = enable; }
	// ����̂ݍs���H(�Ԃ��菈���͂��Ȃ���)
	bool IsTrigger() const { return m_isTrigger; }

	// CollisionManager�ɓo�^�ς݁H
	bool IsRegistered() const { return m_isRegistered; }
	// ���݂�Active��CollisionManager�ɓo�^
	void Register();

	// UpdateCollisionData�֐��ōX�V���������s���邩�ǂ���
	void SetUpdateCollisionData(bool enable)
	{
		m_updateCollisionData = enable;
	}

	// �o�^����
	void Unregister()
	{
		m_isRegistered = false;
		m_refMgr.reset();
	}

	// ���肷�鑤�H
	bool IsDoJudge() const { return m_isDoJudge; }

	// 
	float CalcMassRatio(float mass);

	//------------------------------------------------
	// ���茋�ʃf�[�^
	//------------------------------------------------
	// ���茋�ʃf�[�^�擾
	const std::vector<CollisionResult>& GetResults() const { return m_resultList; }
	// ���茋�ʃf�[�^�ǉ�
	void AddResult(const CollisionResult& resultData)
	{
		m_resultList.push_back(resultData);
	}

	//------------------------------------------------
	// �q�b�g�ʒm�֐��I�u�W�F�N�g
	//------------------------------------------------
	// ������̓v���O�������Ղ��d���Ŋ֐��I�u�W�F�N�g�ɂ��܂������AUnity�̂悤��virtual�֐��Œʒm������@������܂��B
	// �����肪�I����null�ɂȂ�̂ŁA����֐��I�u�W�F�N�g������K�v������܂��B
	std::function<void(ColliderComponent*)>	m_onHitEnter;	// �q�b�g���̏���̂ݎ��s�����
	std::function<void(ColliderComponent*)>	m_onHitStay;	// �q�b�g�������Ǝ��s�����
	std::function<void(ColliderComponent*)>	m_onHitExit;	// �q�b�g���Ȃ��Ȃ�������̂ݎ��s�����

	//------------------------------------------------
	// Serialize / Deserialize
	//------------------------------------------------
	virtual void Deserialize(const json11::Json& jsonData) override
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["Tag"], m_strTag);
		KdJsonGet(jsonData["IsDoJudge"], m_isDoJudge);
		KdJsonGet(jsonData["GroupFilter"], m_groupFilter);
		KdJsonGet(jsonData["MaskFilter"], m_maskFilter);
		KdJsonGet(jsonData["IsTrigger"], m_isTrigger);
	}

	// JSON�f�[�^����A�����ݒ���s��
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["Tag"] = m_strTag;
		outJsonObj["IsDoJudge"] = m_isDoJudge;
		outJsonObj["GroupFilter"] = m_groupFilter;
		outJsonObj["MaskFilter"] = m_maskFilter;
		outJsonObj["IsTrigger"] = m_isTrigger;
	}

	// (Editor)ImGui
	virtual void Editor_ImGuiUpdate() override;


	// Debug�p�ݒ�
	static bool s_showDebugLine;
	static KdVec4 s_debugLineColor;
	static KdVec4 s_debugSelectLineColor;

protected:

	//------------------------------------------------
	// �ݒ�f�[�^
	//------------------------------------------------

	// �ėp������^�O
	std::string			m_strTag;

	// ���肷�鑤��(�ʒm�擾�\)
	bool				m_isDoJudge = false;

	// ����t�B���^
	int					m_groupFilter = 0;	// �O���[�v�t�B���^�@
	int					m_maskFilter = 0;	// �}�X�N�t�B���^

	// ����̂ݍs���A�Ԃ��菈���͂��Ȃ�
	bool				m_isTrigger = false;

	// UpdateCollisionData�֐��ōX�V�����s����
	bool				m_updateCollisionData = true;

	//------------------------------------------------
	// ����������s���̂ݎg�p����f�[�^
	//------------------------------------------------

	// �q�b�g��ԃt���O(����q�b�g���A�q�b�g���A�q�b�g�������̔���p)
	int								m_hitState = 0;

	// ���ʃf�[�^�@m_isDoJudge == true �̏ꍇ�A�����ɂ����蔻��̌��ʃf�[�^�������Ă���
	std::vector<CollisionResult>	m_resultList;

	// CollisionManager�ɓo�^�ς݁H
	bool							m_isRegistered = false;

	// �o�^�}�l�[�W���ւ̎Q��
	KdWptr<CollisionManager>		m_refMgr;


};
