#include "Application/main.h"

#include "ActioinGame.h"

void CharacterController::Start()
{
	GAMESYS.m_editorData.LogWindow.AddLog(u8"CharacterComtoller開始！");

	//============================
	// 全行動ステートクラスを作成しておく
	//============================
	m_stateStorage["Stand"] = std::make_shared<State_Stand>();
	m_stateStorage["Run"] = std::make_shared<State_Run>();
	// 全行動クラスに持ち主キャラのアドレスを入れておく
	for (auto&& state : m_stateStorage) {
		// state.first;　→キーの名前が入っている
		state.second->m_chara = this;	// データ本体が入っている
	}

	// 初期の行動ステートは立ちにしておく
	m_nowState = m_stateStorage.at("Stand");

}

void CharacterController::Update()
{

	if (m_enable == false)return;
	// 停止時はスキップ
	if (GAMESYS.IsPlay() == false)return;

	// 入力コンポーネント取得
	auto input = GetOwner()->GetComponent<InputComponent>();
	if (input == nullptr)return;

	//-------------------------------
	// 行動ステート処理実行
	//-------------------------------

	if (m_nowState) {
		m_nowState->Update();
	}

}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();
	// Hp設定
	ImGui::InputInt("Hp", &m_hp);

}

void CharacterController::State_Stand::Update()
{
	// 立ち状態の時のみの処理を書く
	GAMESYS.m_editorData.LogWindow.AddLog("Stand!!");

		// 入力コンポーネント取得
	auto input = m_chara->GetOwner()->GetComponent<InputComponent>();

	// 方向キーを押すと、「走り」に変更
	if (input->GetAxis(GameAxes::L).Length() > 0.1f) {
		// 「走り」行動へ切り替える
		m_chara->m_nowState = m_chara->m_stateStorage.at("Run");
		
		// 「走り」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// 走りアニメーションデータを取得
		auto anim = model->GetModel()->GetAnimation("Run");
		model->Animator().CrossFade(anim, 0.2f, true);
	}

}

void CharacterController::State_Run::Update()
{
	// 走り状態の時のみの処理を書く
	GAMESYS.m_editorData.LogWindow.AddLog("Run!!");

	// 入力コンポーネント取得
	auto input = m_chara->GetOwner()->GetComponent<InputComponent>();

	// 方向キーを話すと、「立ち」に変更
	if (input->GetAxis(GameAxes::L).Length() <= 0.1f) {
		// 「立ち」行動へ切り替える
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// 「立ち」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// 立ちアニメーションの取得
		auto anim = model->GetModel()->GetAnimation("Stand");
		// そのアニメーションへの切り替え
		model->Animator().CrossFade(anim, 0.2f, true);
	}

	// 移動処理
	auto pos = m_chara->GetOwner()->Transform()->GetPosition();
	pos.z += 0.02f;
	m_chara->GetOwner()->Transform()->SetPosition(pos);

}
