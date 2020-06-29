#include "Application/main.h"

#include "ActioinGame.h"

void CharacterController::Start()
{
	GAMESYS.m_editorData.LogWindow.AddLog(u8"CharacterComtoller�J�n�I");

	//============================
	// �S�s���X�e�[�g�N���X���쐬���Ă���
	//============================
	m_stateStorage["Stand"] = std::make_shared<State_Stand>();
	m_stateStorage["Run"] = std::make_shared<State_Run>();
	// �S�s���N���X�Ɏ�����L�����̃A�h���X�����Ă���
	for (auto&& state : m_stateStorage) {
		// state.first;�@���L�[�̖��O�������Ă���
		state.second->m_chara = this;	// �f�[�^�{�̂������Ă���
	}

	// �����̍s���X�e�[�g�͗����ɂ��Ă���
	m_nowState = m_stateStorage.at("Stand");

}

void CharacterController::Update()
{

	if (m_enable == false)return;
	// ��~���̓X�L�b�v
	if (GAMESYS.IsPlay() == false)return;

	// ���̓R���|�[�l���g�擾
	auto input = GetOwner()->GetComponent<InputComponent>();
	if (input == nullptr)return;

	//-------------------------------
	// �s���X�e�[�g�������s
	//-------------------------------

	if (m_nowState) {
		m_nowState->Update();
	}

}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();
	// Hp�ݒ�
	ImGui::InputInt("Hp", &m_hp);

}

void CharacterController::State_Stand::Update()
{
	// ������Ԃ̎��݂̂̏���������
	GAMESYS.m_editorData.LogWindow.AddLog("Stand!!");

		// ���̓R���|�[�l���g�擾
	auto input = m_chara->GetOwner()->GetComponent<InputComponent>();

	// �����L�[�������ƁA�u����v�ɕύX
	if (input->GetAxis(GameAxes::L).Length() > 0.1f) {
		// �u����v�s���֐؂�ւ���
		m_chara->m_nowState = m_chara->m_stateStorage.at("Run");
		
		// �u����v�A�j���[�V�����֕ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// ����A�j���[�V�����f�[�^���擾
		auto anim = model->GetModel()->GetAnimation("Run");
		model->Animator().CrossFade(anim, 0.2f, true);
	}

}

void CharacterController::State_Run::Update()
{
	// �����Ԃ̎��݂̂̏���������
	GAMESYS.m_editorData.LogWindow.AddLog("Run!!");

	// ���̓R���|�[�l���g�擾
	auto input = m_chara->GetOwner()->GetComponent<InputComponent>();

	// �����L�[��b���ƁA�u�����v�ɕύX
	if (input->GetAxis(GameAxes::L).Length() <= 0.1f) {
		// �u�����v�s���֐؂�ւ���
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// �u�����v�A�j���[�V�����֕ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// �����A�j���[�V�����̎擾
		auto anim = model->GetModel()->GetAnimation("Stand");
		// ���̃A�j���[�V�����ւ̐؂�ւ�
		model->Animator().CrossFade(anim, 0.2f, true);
	}

	// �ړ�����
	auto pos = m_chara->GetOwner()->Transform()->GetPosition();
	pos.z += 0.02f;
	m_chara->GetOwner()->Transform()->SetPosition(pos);

}
