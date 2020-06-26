#pragma once

#include "ColliderComponent.h"
#include "SphereColliderComponent.h"
#include "RayColliderComponent.h"
#include "BoxColliderComponent.h"
#include "MeshColliderComponent.h"

#include "CollisionFunctions.h"

// �q�b�g��ԃt���O
namespace HitStates
{
	enum
	{
		ENTER = 0x00000001,	// HIT����1���
		STAY = 0x00000002,	// HIT��������
		EXIT = 0x00000004,	// HIT���Ȃ��Ȃ���1���
	};
}

//===============================================
// 
// �R���W�����Ǘ��N���X
// �EColliderComponent�n�̓o�^�E����E���ʒʒm�̂��ׂĂ��Ǘ�����B
//
//===============================================
class CollisionManager : public std::enable_shared_from_this<CollisionManager>
{
public:

	//-------------------------------------------------------
	// ����I�u�W�F�N�g��o�^
	//-------------------------------------------------------
	void Register(const KdSptr<ColliderComponent>& obj)
	{
		if (obj == nullptr)return;

		// �o�^�ς݁H
		if (obj->IsRegistered())return;

		//���肷�鑤�H
		if (obj->IsDoJudge())
		{
			m_doJudgeList.push_back(obj);	// �o�^
		}
		else
		{
			m_passiveList.push_back(obj);	// �o�^
		}

		// �o�^�ς݂ɂ���
		obj->m_isRegistered = true;
		obj->m_refMgr = shared_from_this();
	}

	//-------------------------------------------------------
	// �R���C�_�[�̍X�V�┻��̎��s
	//-------------------------------------------------------
	// onlyUpdateCollider	�c �R���C�_�[�̍X�V�̂ݎ��s���A����͎��s���Ȃ��B
	void Run(bool onlyUpdateCollider);

	// comp vs m_passiveList�̔���
	void HitTestToPassiveList(ColliderComponent& comp);

	// �S����
	void Release();

	//-------------------------------------------------------
	// ����ݒ�E�擾
	//-------------------------------------------------------
	// �}���`�X���b�h�ł̔��� On/Off
	void SetMultiThread(bool enable)
	{
		m_isMultiThread = enable;
	}

	// �}���`�X���b�h���삩�H
	bool IsMultiThread() const { return m_isMultiThread; }

	// ���̃}�l�[�W�����f�t�H���g�ɂ���
	void SetManagerToDefault()
	{
		s_nowManager = shared_from_this();
	}

	// (static�֐�)���݃Z�b�g����Ă���}�l�[�W����Ԃ�
	static KdSptr<CollisionManager> sGetNowManager()
	{
		return s_nowManager.lock();
	}

	// �t�B���^�[���X�g�̂P���ڂ̃f�[�^
	struct FilterNode
	{
		std::string Name;	// �t�B���^�[��
		int Flag = 0;		// �r�b�g�t���O
	};


	// ����t�B���^�ݒ�
	static std::vector<FilterNode>	s_filterType1;
	static std::vector<FilterNode>	s_filterType2;


	// ImGui
	void ImGuiUpdate();

private:

	// ���肷�鑤���X�g
	std::list< KdSptr<ColliderComponent> >	m_doJudgeList;
	// ���肳��鑤���X�g
	std::list< KdSptr<ColliderComponent> >	m_passiveList;

	// Run�֐��̂����蔻����}���`�X���b�h�ōs��
	bool m_isMultiThread = true;

	// ���ݎg�p���Ă���}�l�[�W��
	// �R���C�_�[�R���|�[�l���g�́A�����ɃZ�b�g����Ă���CollisionManager�ɓo�^���邱�ƂɂȂ�B
	static KdWptr<CollisionManager>	s_nowManager;

	// (�f�o�b�O�p)����ɂ�����������
	long long m_debug_ElapsedTime = 0;

};
