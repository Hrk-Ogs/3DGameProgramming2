#include "../../GameSystem.h"

#include "CollisionManager.h"

std::vector<CollisionManager::FilterNode>	CollisionManager::s_filterType1 =
{
	{u8"�Ԃ���",		0x00000001},
	{u8"�U��",			0x00000002},
	{u8"�M�~�b�N����",	0x00000004},
};

std::vector<CollisionManager::FilterNode>	CollisionManager::s_filterType2 =
{
	{u8"�w�i��",		0x00010000},
	{u8"����",			0x00020000},
	{u8"�G",			0x00040000},
	{u8"�M�~�b�N",		0x00080000},
	{u8"�v���C���[",	0x00100000},
};

KdWptr<CollisionManager> CollisionManager::s_nowManager;

void CollisionManager::Run(bool onlyUpdateCollider)
{
	//-------------------------------
	//
	// �R���C�_�[�̃f�[�^�X�V
	//
	//-------------------------------
	// ���肷�鑤���X�g�̃R���C�_�[�f�[�^���X�V
	std::for_each(
		std::execution::par,						// ����Ŏ��s
		m_doJudgeList.begin(), m_doJudgeList.end(), // �z��̍ŏ�����Ō�܂�
		[this](KdSptr<ColliderComponent>& doJudge)	// ���s���鏈���֐�
		{
			doJudge->UpdateCollisionData();
		}
	);
	// ���肳��鑤���X�g�̃R���C�_�[�f�[�^���X�V
	std::for_each(
		std::execution::par,						// ����Ŏ��s
		m_passiveList.begin(), m_passiveList.end(), // �z��̍ŏ�����Ō�܂�
		[this](KdSptr<ColliderComponent>& passive)	// ���s���鏈���֐�
		{
			passive->UpdateCollisionData();
		}
	);

	//-------------------------------
	//
	// �Փ˔���E�ʒm
	//
	//-------------------------------
	// �R���C�_�[�X�V�����̏ꍇ�́A�Փ˔���͎��s���Ȃ�
	if (onlyUpdateCollider == false)
	{
		//-------------------------------
		// �Փ˔��菈��
		//-------------------------------
		// ���Ԍv���J�n
		auto startTime = std::chrono::system_clock::now();

		// �V���O���X���b�h�ł̔���
		if (m_isMultiThread == false)
		{
			// �S�u���肷�鑤�v����u���肳��鑤�v�ւ����蔻��
			for (auto&& doJudge : m_doJudgeList)
			{
				// �L������Ȃ�
				if (doJudge->IsEnable() == false)continue;
				// ���łɓo�^�t���O��OFF
				if (doJudge->IsRegistered() == false)continue;
				// �����傢�Ȃ�
				if (doJudge->ExistOwner() == false)continue;

				// ������s
				HitTestToPassiveList(*doJudge);
			}
		}
		// �}���`�X���b�h�ł̔���
		else
		{
			// ����forC++17����g�p�\)
			// �w�肵���z��̊e�v�f������Ŏ��s�����
			std::for_each(
				std::execution::par,						// ����Ŏ��s
				m_doJudgeList.begin(), m_doJudgeList.end(), // �z��̍ŏ�����Ō�܂�
				[this](KdSptr<ColliderComponent>& doJudge)	// ���s���鏈���֐�
				{
					// �L������Ȃ�
					if (doJudge->IsEnable() == false)return;
					// ���łɓo�^�t���O��OFF
					if (doJudge->IsRegistered() == false)return;
					// �����傢�Ȃ�
					if (doJudge->ExistOwner() == false)return;

					// ������s
					HitTestToPassiveList(*doJudge);
				}
			);

		}

		auto endTime = std::chrono::system_clock::now();
		// �o�ߎ��Ԍv��
		m_debug_ElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

		//-------------------------------
		//
		// ���ʒʒm�֐����s
		//
		//-------------------------------]
		for (auto&& doJudge : m_doJudgeList)
		{
			//----------------
			// �ʒm�֐����s
			//----------------

			// �L������Ȃ�
			if (doJudge->IsEnable() == false)continue;

			// ����q�b�g���̂�
			if (doJudge->m_hitState & HitStates::ENTER)
			{
				if (doJudge->m_onHitEnter)doJudge->m_onHitEnter(doJudge.get());
			}
			// �q�b�g��
			if (doJudge->m_hitState & HitStates::STAY)
			{
				if (doJudge->m_onHitStay)doJudge->m_onHitStay(doJudge.get());
			}
			// �q�b�g���Ȃ��Ȃ����ŏ���1��ڂ̂�
			if (doJudge->m_hitState & HitStates::EXIT)
			{
				if (doJudge->m_onHitExit)doJudge->m_onHitExit(doJudge.get());
			}

			// �o�O�h�~�̂��߁A����null�ɂ���
			doJudge->m_onHitEnter = nullptr;
			doJudge->m_onHitStay = nullptr;
			doJudge->m_onHitExit = nullptr;
		}
	}

	//-------------------------------
	//
	// �R���C�_�[�̃`�F�b�N
	//�@�E�s�K�v�ȃR���C�_�[�̓��X�g�����������
	//
	//-------------------------------

	// ���肷�鑤���X�g
	for (auto it = m_doJudgeList.begin(); it != m_doJudgeList.end();)
	{
		// �Q�ƃJ�E���^���P�� ����
		// ���肳��鑤 ����
		// �o�^�t���O��OFF ����
		// �I�[�i�[�������Ă���
		// �̂��̂́A�o�^��������
		if ((*it).use_count() == 1 || (*it)->IsDoJudge() == false || (*it)->m_isRegistered == false || (*it)->ExistOwner() == false)
		{
			// �o�^����
			(*it)->m_isRegistered = false;
			(*it)->m_refMgr.reset();

			it = m_doJudgeList.erase(it);
		}
		else
		{
			++it;
		}
	}

	// ���肳��鑤���X�g
	for (auto it = m_passiveList.begin(); it != m_passiveList.end();)
	{
		// �Q�ƃJ�E���^���P�� ����
		// ���肷�鑤 ����
		// �o�^�t���O��OFF ����
		// �I�[�i�[�������Ă���
		// �̂��̂́A�o�^��������
		if ((*it).use_count() == 1 || (*it)->IsDoJudge() == true || (*it)->m_isRegistered == false || (*it)->ExistOwner() == false)
		{
			// �o�^����
			(*it)->m_isRegistered = false;
			(*it)->m_refMgr.reset();

			it = m_passiveList.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CollisionManager::HitTestToPassiveList(ColliderComponent& comp)
{
	// ���肷�鑤����Ȃ��Ȃ�A�X�L�b�v
	if (comp.IsDoJudge() == false)return;
	// ���茋�ʃf�[�^���N���A����
	comp.m_resultList.clear();

	// PassiveList�Ɣ���
	for (auto&& passive : m_passiveList)
	{
		//------------------------
		// ���O�`�F�b�N
		//------------------------
		if (passive->IsEnable() == false)continue;
		if (passive->ExistOwner() == false)continue;

		// ���� vs ���� �t�B���^����
		if ((comp.m_groupFilter & passive->m_maskFilter) == 0 || (comp.m_maskFilter & passive->m_groupFilter) == 0)continue;

		// ����������͖���
		if (comp.GetOwner() == passive->GetOwner())continue;

		//------------------------
		// ����
		//------------------------
		comp.HitTest(passive.get());
	}

	//-------------------------
	// �q�b�g��ԃt���O����
	//-------------------------
	// ����HIT�t���O��HIT�I���t���O��OFF
	comp.m_hitState &= ~HitStates::ENTER;// OFF
	comp.m_hitState &= ~HitStates::EXIT;// OFF

	// �q�b�g���Ă���
	if (comp.GetResults().size() > 0)
	{
		// �O��q�b�g���ł͂Ȃ�
		if (!(comp.m_hitState & HitStates::STAY))
		{
			comp.m_hitState |= HitStates::ENTER;// ON
			comp.m_hitState |= HitStates::STAY;// ON
		}
	}
	//�q�b�g���Ă��Ȃ�
	else
	{
		//�O��q�b�g��
		if (comp.m_hitState & HitStates::STAY)
		{
			comp.m_hitState |= HitStates::EXIT;// ON
			comp.m_hitState &= ~HitStates::STAY;// OFF
		}
	}
}

void CollisionManager::Release()
{
	// �o�^����Ă���I�u�W�F�N�g���������Ă���
	for (auto&& n : m_doJudgeList)
	{
		n->m_isRegistered = false;
		n->m_refMgr.reset();
	}
	for (auto&& n : m_passiveList)
	{
		n->m_isRegistered = false;
		n->m_refMgr.reset();
	}
	m_doJudgeList.clear();
	m_passiveList.clear();
}

void CollisionManager::ImGuiUpdate()
{

	ImGui::Checkbox(u8"�}���`�X���b�h", &m_isMultiThread);
	ImGui::Checkbox(u8"�SCollision�f�o�b�O�\��", &ColliderComponent::s_showDebugLine);

	// �F
	if (ImGui::CollapsingHeader(u8"�F", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4(u8"��", &ColliderComponent::s_debugLineColor.x);
		ImGui::ColorEdit4(u8"�I��", &ColliderComponent::s_debugSelectLineColor.x);
	}

	// ���
	if (ImGui::CollapsingHeader(u8"���", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text(u8"���肷�鑤 �@%d ��", m_doJudgeList.size());
		ImGui::Text(u8"���肳��鑤 %d ��", m_passiveList.size());
		ImGui::Text(u8"����ɂ�����������\n  %.3f ms", m_debug_ElapsedTime / 1000.0f);
	}
}


