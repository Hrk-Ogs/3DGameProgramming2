#include "Application/main.h"

#include "ActioinGame.h"

void CharacterController::Start()
{
	GAMESYS.m_editorData.LogWindow.AddLog(u8"CharacterComtoller開始！");
}

void CharacterController::Update()
{
	// 入力コンポーネント取得
	auto input = GetOwner()->GetComponent<InputComponent>();
	if (input == nullptr)return;

	if (m_enable == false)return;
	// 停止時はスキップ
	if (GAMESYS.IsPlay() == false)return;

	// ワールド行列取得
	KdVec3 pos = GetOwner()->Transform()->GetPosition();
	// 回転取得
	KdQuaternion rotation = GetOwner()->Transform()->GetRotation();


	// 方向キーで移動
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

	// A,DでY軸回転
	if (input->IsButtonPressed(GameButtons::A))
	{
		rotation = KdQuaternion().CreateRotationFromAngles(0, -1 * KdToRadians, 0) * rotation;
	}

	if (input->IsButtonPressed(GameButtons::B))
	{
		rotation = KdQuaternion().CreateRotationFromAngles(0, 1 * KdToRadians, 0) * rotation;
	}

	// ワールド行列セット
	GetOwner()->Transform()->SetPosition(pos);
	GetOwner()->Transform()->SetRotation(rotation);

	// 徐々に体力を減らす
	m_hp--;
}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();
	// Hp設定
	ImGui::InputInt("Hp", &m_hp);

}
