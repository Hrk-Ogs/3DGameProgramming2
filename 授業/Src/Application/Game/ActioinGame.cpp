#include "Application/main.h"

#include "ActioinGame.h"

void CharacterController::Start()
{
	GAMESYS.m_editorData.LogWindow.AddLog(u8"CharacterComtoller開始！");
}

void CharacterController::Update()
{
	if (m_enable == false)return;

	// ワールド行列取得
	KdMatrix m = GetOwner()->Transform()->GetMatrix();

	// 方向キーで移動
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

	// A,DでY軸回転
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m = KdMatrix().CreateRotationY(-1 * KdToRadians) * m;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m = KdMatrix().CreateRotationY(1 * KdToRadians) * m;
	}

	// ワールド行列セット
	GetOwner()->Transform()->SetMatrix(m);

	// 徐々に体力を減らす
	m_hp--;
}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();
	// Hp設定
	ImGui::InputInt("Hp", &m_hp);

}
