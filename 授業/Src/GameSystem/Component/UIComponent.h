#pragma once


//==========================================
// オブザーバーコンポーネント基本クラス
// ・サブジェクト(ボタン)クラスに何かアクションがあった時、このクラスに「通知」が行われる
// ・このクラスを継承し、ゲーム特有の処理を書いたクラスにしていく。
//==========================================
class UIObserverComponent : public BaseComponent
{
public:
	// (通知)gameObjがクリックされた時に実行される
	// ・gameObj … クリックされたGameObject
	// ・message … 送信されてきた文字列
	virtual void NotifyClicked(KdSptr<GameObject> gameObj, const std::string& message) {}

};

//==========================================
// サブジェクトコンポーネント基本クラス
// ・このクラスを継承し、ボタンなどを作る
//==========================================
class UISubjectComponent : public BaseComponent
{
public:
	// 初期処理
	virtual void Start() override;
protected:
	// オブザーバーリスト(通知先)
	std::vector<KdWptr<UIObserverComponent>> m_observers;
};

//==========================================
// UIボタンコンポーネント
//==========================================
class UIButtonComponent : public UISubjectComponent
{
public:
	// 更新処理
	virtual void Update() override;
	virtual void Editor_ImGuiUpdate() override
	{
		UISubjectComponent::Editor_ImGuiUpdate();
		ImGui::Text(u8"クリック時に送信する文字列");
		ImGui::InputText(u8"Message", &m_onClickedMessage);
	}
	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData)
	{
		UISubjectComponent::Deserialize(jsonData);
		KdJsonGet(jsonData["OnClickedMessage"], m_onClickedMessage);
	}
	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		UISubjectComponent::Serialize(outJsonObj);
		outJsonObj["OnClickedMessage"] = m_onClickedMessage;
	}
private:
	// クリック時に送信するメッセージ
	std::string m_onClickedMessage = "";
	// クリック制御用フラグ
	bool m_clickFlag = false;
};

