#pragma once


class CharacterController : public BaseComponent
{
public:
	// 更新処理
	virtual void Start() override;

	// 更新処理
	virtual void Update() override;

	// ImGui処理
	virtual void Editor_ImGuiUpdate() override;


	//===============================
	// シリアライズ/デシリアライズ
	//===============================
	virtual void Deserialize(const json11::Json& jsonObj)
	{
		BaseComponent::Deserialize(jsonObj);
		KdJsonGet(jsonObj["Hp"], m_hp);
	}
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseComponent::Serialize(outJsonObj);
		outJsonObj["Hp"] = m_hp;
	}

private:

	// 体力
	int m_hp = 600;

	//================
	// 行動ステートマシン
	//================
	// 行動ステート基本クラス
	class State_Base
	{
	public:

		virtual ~State_Base(){}
		// 行動処理
		virtual void Update() {}
		// キャラへの参照
		CharacterController* m_chara = nullptr;
	};

	//「立ち」行動クラス
	class State_Stand :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	//「走り」行動クラス
	class State_Run :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	// 現在の行動ステートへの参照
	KdSptr<State_Base>	m_nowState;

	// 全行動ステートをあらかじめ作成して保管しておく場所
	std::unordered_map<std::string, KdSptr<State_Base>>	m_stateStorage;

};


//=========================================
// タイトル制御クラス
//=========================================
class TitleController :public BaseComponent
{
public:
	virtual void Update() override
	{
		// 無効時はスキップ
		if (m_enable == false)return;
		// 停止時はスキップ
		if (GAMESYS.IsPlay() == false)return;

		// 入力コンポーネント
		auto input = GetOwner()->GetComponent<InputComponent>();
		if (input == nullptr)return;

		// 決定ボタン
		if (input->IsButtonPressed(GameButtons::A)) {
			// Level切り替え予約
			GAMESYS.ChangeLevel("Data\\Level\\Stage1.level");
		}
	}

	// ImGui処理
	virtual void Editor_ImGuiUpdate()override
	{
		BaseComponent::Editor_ImGuiUpdate();

		auto input = GetOwner()->GetComponent<InputComponent>();
		if (input == nullptr) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(u8"InputComponentを追加してください！！");
			ImGui::PopStyleColor();
		}
	}

private:

};

