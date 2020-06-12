#include "Application/main.h"

#include "ActioinGame.h"

void CharacterController::Start()
{
	GAMESYS.m_editorData.LogWindow.AddLog(u8"CharacterComtoller�J�n�I");
}

void CharacterController::Update()
{
	if (m_enable == false)return;

	// ���[���h�s��擾
	KdMatrix m = GetOwner()->Transform()->GetMatrix();

	// �����L�[�ňړ�
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m = KdMatrix().CreateTranslation(0, 0, 0.1f) * m;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m = KdMatrix().CreateTranslation(-0.1f, 0, 0) * m;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m = KdMatrix().CreateTranslation(0.1f, 0, 0) * m;
	}

	// A,D��Y����]
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m = KdMatrix().CreateRotationY(-1 * KdToRadians) * m;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m = KdMatrix().CreateRotationY(1 * KdToRadians) * m;
	}

	// ���[���h�s��Z�b�g
	GetOwner()->Transform()->SetMatrix(m);

	// ���X�ɑ̗͂����炷
	m_hp--;
}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();
	// Hp�ݒ�
	ImGui::InputInt("Hp", &m_hp);

}
