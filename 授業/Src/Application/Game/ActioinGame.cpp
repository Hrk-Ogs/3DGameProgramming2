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
	
	//--------------------------------
	// ���_��]
	//--------------------------------
	{
		const float kMaxAngle = 2;	// �J�����̉�]���x
		
		// �qGameObject�́hCamera�h����]������
		auto camObj = GetOwner()->Find("Camera");
		if (camObj) {
			// CharacterCamerController���擾
			auto compCtrl = camObj->GetComponent<CharacteCameraController>();
			if (compCtrl) {
				// Y����]
				compCtrl->CamAngles().y += input->GetAxis(GameAxes::R).x * kMaxAngle;
			}
		}
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

	//--------------------------
	// �J�����x�[�X�ړ�
	//--------------------------
	auto camObj = m_chara->GetOwner()->Find("Camera");

	KdVec3 vDir;
	if (camObj) {
		// �J����GameObject�̍s��(���[���h�s��j
		KdMatrix mCamWorld = camObj->Transform()->GetMatrix();
		// �����L�[�̏�񂩂�A�ړ����������肷��
		vDir += mCamWorld.Forward() * input->GetAxis(GameAxes::L).y;
		vDir += mCamWorld.Right() * input->GetAxis(GameAxes::L).x;
		vDir.y = 0;
		vDir.Normalize();

		if (vDir.Length() > 0) {
			// �L������Transform���擾
			auto charaTrans = m_chara->GetOwner()->Transform();
			
			//-------------
			// ��]
			//-------------
			KdVec3 vCharaZ = charaTrans->GetRotation().Forward();
			// Y��ׂ��AXZ���ʂɕ��s�ɂ���B
			vCharaZ.y = 0;
			vCharaZ.Normalize();

			// �L�����̌����x�N�g�����A�w������Ɏw��p�x����������
			constexpr float kMaxAngle = 15;	// �ő��]�p�x
			vCharaZ.LookTo(vDir, kMaxAngle * KdToRadians);

			// �����̉�]�i�N�I�[�^�j�I���j�擾
			KdQuaternion rota = charaTrans->GetRotation();
			// ��]���w������֌����A�Z�b�g����
			rota.LookTo(vCharaZ, { 0,1,0 });
			charaTrans->SetRotation(rota);
			
			//-------------
			// �ړ�
			//-------------
			const float kSpeed = 0.05f;	// �ړ����x
			
			KdVec3 pos = charaTrans->GetPosition();
			pos += vDir * kSpeed;
			charaTrans->SetPosition(pos);
		}


	}
}

void CharacteCameraController::Update()
{

	if (m_enable == false)return;
	//if (GAMESYS.IsPlay() == false)return;	// �G�f�B�^�[���ł����삳����

	// �e��GameObject
	auto parent = GetOwner()->GetParent();
	if (parent) {
		// �e�̕ϊ��s��
		auto& parentTransform = parent->Transform();

		// �e�̍s�񂩂�A�J�����̍s������肷��
		KdMatrix camMat;
		// �I�t�Z�b�g���W���ړ�
		camMat.Move(m_camOffset);

		// ��]
		camMat.RotateX(m_camAngles.x * KdToRadians);
		camMat.RotateY(m_camAngles.y * KdToRadians);

		// �e�́u���W�̂݁v�����Z
		camMat.Move(parentTransform->GetMatrix().Translation());

		// �Z�b�g
		GetOwner()->Transform()->SetMatrix(camMat);
	}

}
