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
	m_stateStorage["JumpStart"] = std::make_shared<State_JumpStart>();
	m_stateStorage["Jump"] = std::make_shared<State_Jump>();
	m_stateStorage["Landing"] = std::make_shared<State_Landing>();
	m_stateStorage["Attack"] = std::make_shared<State_Attack>();

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

	// �X�N���v�g�L�[�������Ƃ��ɂ��̊֐������s���Ă��炤
	auto onScript = [](const KdKeyScript& script) {
		std::string type = script.JsonData["Type"].string_value();
		if (type == "Attack") {
			GAMESYS.m_editorData.LogWindow.AddLog("Attack!!");
		}
		else if (type == "AttackCancel") {
			GAMESYS.m_editorData.LogWindow.AddLog("Cansel!!");
		}

	};

	// �X�N���v�g�֐���ModelComponent�ɓn��
	auto model = GetOwner()->GetComponent<ModelComponent>();
	if (model) {
		model->SetAnimationScriptProc(onScript);
	}

	//-------------------------------
	// �s���X�e�[�g�������s
	//-------------------------------

	if (m_nowState) {
		m_nowState->Update();
	}
	
	//--------------------------------
	// �͂ɂ��ړ�
	//--------------------------------
	KdVec3 pos = GetOwner()->Transform()->GetPosition();	// ���W�擾
	pos += m_vForce;		// ���W�ɗ͂����Z
	GetOwner()->Transform()->SetPosition(pos);		// ���W���Z�b�g
	
	//--------------------------------
	// �n�ʂ̐ݒu�����m����
	//--------------------------------
	{
		// (���j�ݒu��������Z�b�g
		m_wpGround.reset();

		// ���C�R���C�_�[���擾
		auto ray = GetOwner()->GetComponent<RayColliderComponent>();
		if (ray) {
			// �q�b�g���Ɏ��s�����֐�
			ray->m_onHitStay = [this](ColliderComponent* collider) {	// �����_��
				// ���C���q�b�g����

				// �ł��߂����T��
				float closestDist = FLT_MAX;
				KdSptr<GameObject> closestObj;
				// �q�b�g�������̑S��
				for (auto&& res : collider->GetResults()) {
					// ���߂������łЂ��Ƃ��Ă�
					if (res.Dist < closestDist) {
						// �����L��
						closestDist = res.Dist;
						closestObj = res.Collider->GetOwner();
					}

				}

				// ���ړ����Ă��鎞�̂�
				if (m_vForce.y <= 0) {
					// �ڒn�L������c��
					m_wpGround = closestObj;
				}

				// Y�����̗͂����Z�b�g
				m_vForce.y = 0;
			};
		}
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
		// �A�j���[�V�����֐؂�ւ�
		model->Animator().CrossFade(anim, 0.2f, true);
	}

	// �W�����v
	if (input->IsButtonPressed(GameButtons::B)) {
		// �u�W�����v�v�s���֐؂�ւ���
		auto state = std::dynamic_pointer_cast<State_JumpStart>(m_chara->m_stateStorage.at("JumpStart"));
		state->m_force = { 0,0.2f,0 };	// �����
		m_chara->m_nowState = state;

		// �u�W�����v�v�A�j���[�V�����֕ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// �W�����v�A�j���[�V�����f�[�^���擾
		auto anim = model->GetModel()->GetAnimation("JumpStart");
		// �A�j���[�V�����֐؂�ւ�
		model->Animator().CrossFade(anim, 1.0f, false);
	}

	// ���܋󒆂�
	if (m_chara->m_wpGround.expired() == true) {
		// �s���X�e�[�g�ύX
		m_chara->m_nowState = m_chara->m_stateStorage.at("Jump");

		// �A�j���[�V�����ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Jump");
		model->Animator().CrossFade(anim, 0.1f, false);
	}

	// �U��
	if (input->IsButtonPressed(GameButtons::A)) {
		// �u�U���v�s���֐؂�ւ���
		m_chara->m_nowState = m_chara->m_stateStorage.at("Attack");

		// �u�U���v�A�j���[�V�����֕ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// �U���A�j���[�V�����f�[�^���擾
		auto anim = model->GetModel()->GetAnimation("Attack_Slash1_Root");
		// �A�j���[�V�����֐؂�ւ�
		model->Animator().CrossFade(anim, 0.5f, false);
	}

	// �d��
	m_chara->CalcGravity();
	// ���C
	m_chara->CalcFriction();


}

void CharacterController::State_Run::Update()
{
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

	// �W�����v
	if (input->IsButtonPressed(GameButtons::B)) {
		// �u�W�����v�v�s���֐؂�ւ���
		auto state = std::dynamic_pointer_cast<State_JumpStart>(m_chara->m_stateStorage.at("JumpStart"));
		state->m_force = { 0,0.2f,0 };	// �����
		state->m_force += vDir * 0.1f;
		m_chara->m_nowState = state;

		// �u�W�����v�v�A�j���[�V�����֕ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// �W�����v�A�j���[�V�����f�[�^���擾
		auto anim = model->GetModel()->GetAnimation("JumpStart");
		// �A�j���[�V�����֐؂�ւ�
		model->Animator().CrossFade(anim, 1.0f, false);
	}

	// ���܋󒆂�
	if (m_chara->m_wpGround.expired() == true) {
		// �s���X�e�[�g�ύX
		m_chara->m_nowState = m_chara->m_stateStorage.at("Jump");

		// �A�j���[�V�����ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Jump");
		model->Animator().CrossFade(anim, 0.1f, false);
	}

	// �U��
	if (input->IsButtonPressed(GameButtons::A)) {
		// �u�U���v�s���֐؂�ւ���
		m_chara->m_nowState = m_chara->m_stateStorage.at("Attack");

		// �u�U���v�A�j���[�V�����֕ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// �U���A�j���[�V�����f�[�^���擾
		auto anim = model->GetModel()->GetAnimation("Attack_Slash1_Root");
		// �A�j���[�V�����֐؂�ւ�
		model->Animator().CrossFade(anim, 0.5f, false);
	}

	// �d��
	m_chara->CalcGravity();
	// ���C
	m_chara->CalcFriction();
}


void CharacterController::State_JumpStart::Update()
{
	// 
	GAMESYS.m_editorData.LogWindow.AddLog("JumpStart!!");

	// ModelComponent�擾
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// �A�j���I������
	if (model->Animator().IsAnimationEnd()) {
		// �s���X�e�[�g�ύX
		m_chara->m_nowState = m_chara->m_stateStorage.at("Jump");

		// �A�j���[�V�����ύX
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Jump");
		model->Animator().CrossFade(anim, 0.05f, true);

		// �͂�����
		m_chara->m_vForce = m_force;
	}
	// �d��
	m_chara->CalcGravity();
	// ���C����
	m_chara->CalcFriction();
}

void CharacterController::State_Jump::Update()
{
	// ������Ԃ̎��݂̂̏���������
	GAMESYS.m_editorData.LogWindow.AddLog("Jump!!");

	// ModelComponent�擾
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// ���n�ʂ�
	if (m_chara->m_wpGround.expired() == false) {
		// �s���X�e�[�g��ύX
		m_chara->m_nowState = m_chara->m_stateStorage.at("Landing");

		// �A�j���[�V�����ύX
		auto anim = model->GetModel()->GetAnimation("Landing");
		model->Animator().CrossFade(anim, 1.0f, false);
	}

	// �d��
	m_chara->CalcGravity();
	// ���C
	m_chara->CalcFriction();
}

void CharacterController::State_Landing::Update()
{
	// ������Ԃ̎��݂̂̏���������
	GAMESYS.m_editorData.LogWindow.AddLog("Landing!!");

	// ModelComponent�擾
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// �A�j���I��
	if (model->Animator().IsAnimationEnd()) {
		// �s���X�e�[�g��ύX
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// �A�j���[�V�����ύX
		auto anim = model->GetModel()->GetAnimation("Stand");
		model->Animator().CrossFade(anim, 0.5f, true);
	}

	// �d��
	m_chara->CalcGravity();
	// ���C
	m_chara->CalcFriction();
}

void CharacterController::State_Attack::Update()
{
	// ModelComponent�擾
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// �A�j���I��
	if (model->Animator().IsAnimationEnd()) {
		// �s���X�e�[�g��ύX
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// �A�j���[�V�����ύX
		auto anim = model->GetModel()->GetAnimation("Stand");
		model->Animator().CrossFade(anim, 0.5f, true);
	}

	// �d��
	m_chara->CalcGravity();
	// ���C
	m_chara->CalcFriction();

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

