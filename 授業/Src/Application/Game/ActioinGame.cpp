#include "Application/main.h"

#include "ActioinGame.h"

void CharacterController::Start()
{
	GAMESYS.m_editorData.LogWindow.AddLog(u8"CharacterComtoller�J�n�I");
}

void CharacterController::Update()
{
	// ���̓R���|�[�l���g�擾
	auto input = GetOwner()->GetComponent<InputComponent>();
	if (input == nullptr)return;

	if (m_enable == false)return;
	// ��~���̓X�L�b�v
	if (GAMESYS.IsPlay() == false)return;

	// ���[���h�s��擾
	KdVec3 pos = GetOwner()->Transform()->GetPosition();
	// ��]�擾
	KdQuaternion rotation = GetOwner()->Transform()->GetRotation();


	// �����L�[�ňړ�
	if (input->GetAxis(GameAxes::L).y > 0)
	{
		pos += rotation.Forward() * 0.1f;
	}

	if (input->GetAxis(GameAxes::L).x < 0)
	{
		pos += rotation.Right() * -0.1f;
	}

	if (input->GetAxis(GameAxes::L).x > 0)
	{
		pos += rotation.Right() * 0.1f;
	}

	// A,D��Y����]
	if (input->IsButtonPressed(GameButtons::A))
	{
		rotation = KdQuaternion().CreateRotationFromAngles(0, -1 * KdToRadians, 0) * rotation;
	}

	if (input->IsButtonPressed(GameButtons::B))
	{
		rotation = KdQuaternion().CreateRotationFromAngles(0, 1 * KdToRadians, 0) * rotation;
	}

	// ���[���h�s��Z�b�g
	GetOwner()->Transform()->SetPosition(pos);
	GetOwner()->Transform()->SetRotation(rotation);

	// ���X�ɑ̗͂����炷
	m_hp--;
}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();
	// Hp�ݒ�
	ImGui::InputInt("Hp", &m_hp);

}
