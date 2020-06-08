#include "GameSystem.h"

#include "GameObject.h"

void GameObject::Init()
{
	// TransformComponentの追加
	auto comp = AddComponent("TransformCompnent");
}

void GameObject::Update()
{
	// 無効時はスキップする
	if (!m_enable)return;

	// 全コンポーネントを動作
	for (auto&& comp : m_components) {
		comp->Update();
	}

	// 仮　子も処理
	for (auto&& child : m_children) {
		child->Update();
	}

}

void GameObject::Draw()
{
	// 無効状態はスキップする
	if (!m_enable)return;

	// 全コンポーネントを描画
	for (auto&& comp : m_components) {
		comp->Draw();
	}

	// 仮　子も描画
	for (auto&& child : m_children) {
		child->Draw();
	}
}

void GameObject::AddComponent(const KdSptr<BaseComponent>& comp)
{
	// 持ち主セット
	comp->SetOwner(shared_from_this());
	// 追加
	m_components.push_back(comp);

}

KdSptr<BaseComponent> GameObject::AddComponent(const std::string& className)
{
	KdSptr<BaseComponent> comp;
	if (className == "ModelComponent") {
		comp = std::make_shared<ModelComponent>();
	}
	AddComponent(comp);

	return comp;
}

void GameObject::SetParent(const KdSptr<GameObject>& newParent)
{
	// 親から脱退する前に自分のshared_ptrを記憶させておく
	KdSptr<GameObject> spThis = shared_from_this();

	// 現在の親から脱退する
	auto nowParent = m_parent.lock();
	if (nowParent) {	// 親が存在する
		// 親のリストから自分を検索
		auto it = std::find(nowParent->m_children.begin(), nowParent->m_children.end(), spThis);
		// 自分を発見
		if (it != nowParent->m_children.end()) {
			// 親のリストから解除する
			nowParent->m_children.erase(it);
		}
	}

	// 新しい親のアドレスを記憶する
	m_parent = newParent;

	// 新しい親の子として、自分を登録する
	if (newParent) {
		newParent->m_children.push_back(spThis);
	}

}

void GameObject::Deserialize(const json11::Json& jsonObj)
{
	// 有効フラグ
	KdJsonGet(jsonObj["Enable"], m_enable);
	// 名前
	KdJsonGet(jsonObj["Name"], m_name);

	// 座標
	KdVec3 pos;
	KdJsonGet(jsonObj["Pos"], pos);
	m_transform.Translation(pos);	// 座標をセット

	// コンポーネント
	m_components.clear();
	for (auto&& jsonComp : jsonObj["Components"].array_items()) {
		// JSONデータから、コンポーネントのクラス名を取得
		std::string className = jsonComp["ClassName"].string_value();
		// コンポーネントを追加
		auto comp = AddComponent(className);
		if (comp) {
			// デシリアライズ
			comp->Deserialize(jsonComp);
		}
	}
	// 子GameObject
	m_children.clear();
	for (auto&& jsonChild : jsonObj["Children"].array_items()) {
		// メモリ確保
		KdSptr<GameObject> child = std::make_shared<GameObject>();
		// デシリアライズ
		child->Deserialize(jsonChild);
		// 自分を親として記憶させる
		child->m_parent = shared_from_this();
		// 子として登録
		m_children.push_back(child);
	}

}

void GameObject::Serialize(json11::Json::object& outJsonObj) const
{
	outJsonObj["Name"] = m_name;
	outJsonObj["Enable"] = m_enable;
	outJsonObj["Pos"] = m_transform.Translation().ToArray();

	/*
	if (m_model) {
		outJsonObj["ModelFilename"] = m_model->GetFilepath();
	}
	*/

	// 子リストもシリアライズ
	json11::Json::array jsonChildren;
	for (auto&& child : m_children) {
		// JSONデータ化
		json11::Json::object serial;
		child->Serialize(serial);
		// JSON子配列へ追加
		jsonChildren.push_back(serial);
	}
	outJsonObj["Children"] = jsonChildren;

	// 全コンポーネントをシリアライズ
	json11::Json::array jsonCompAry;
	for (auto&& comp : m_components) {
		// JSONデータ化
		json11::Json::object serial;
		comp->Serialize(serial);
		// 配列へ追加
		jsonCompAry.push_back(serial);
	}
	outJsonObj["Components"] = jsonCompAry;
}

// Inspectorで表示される内容
void GameObject::Editor_ImGuiUpdate()
{
	// 有効無効
	ImGui::Checkbox("Enable", &m_enable);

	ImGui::Dummy(ImVec2(0, 2));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 2));

	// 名前
	ImGui::InputText("Name", &m_name);

	// 座標
	ImGui::DragFloat3("Pos", &m_transform._41, 0.01f);

	// 全コンポーネントを動作
	for (auto&& comp : m_components) {

		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 2));

		ImGui::PushID(comp.get());

		if (ImGui::CollapsingHeader(&typeid(*comp).name()[6], ImGuiTreeNodeFlags_DefaultOpen)) {
			comp->Editor_ImGuiUpdate();
		}

		ImGui::PopID();
	}

	// コンポーネント追加ボタン
	ImGui::Dummy(ImVec2(0, 2));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 5));

	ImGui::Button(u8"コンポーネント追加");
	// ↑のボタンをクリックするとポップアップメニュー表示
	if (ImGui::BeginPopupContextItem("AddComponentPopup", 0)) {
		// 仮
		if (ImGui::Selectable("ModelComponent")) {
			// コンポーネント生成
			auto comp = std::make_shared<ModelComponent>();
			// 親を登録
			comp->SetOwner(shared_from_this());
			// リストに接続
			m_components.push_back(comp);
		}

		ImGui::EndPopup();
	}

}

void GameObject::Editor_ImGuiTree()
{
	ImGui::PushID(this);

	// TreeNodeフラグ
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

	// 選択中？
	if (GAMESYS.m_editorData.SelectObj.lock() == shared_from_this()) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	// GameObject
	bool bOpen = ImGui::TreeNodeEx(m_name.c_str(), flags);

	// クリック判定
	if (ImGui::IsItemClicked()) {
		// 選択物として記憶
		GAMESYS.m_editorData.SelectObj = shared_from_this();
	}


	//--------------------------------------------
	// 右クリックでコンテキストメニューを表示
	//--------------------------------------------
	if (ImGui::BeginPopupContextItem("GameObjectPopupID", 1)) {

		//------------------------
		// Prefab保存
		//------------------------
		if (ImGui::Selectable(u8"Prefabファイルへ保存")) {

			std::string filepath;
			if (KdWindow::SaveFileDialog(filepath, "Prefab保存", "prefファイル\0*.pref\0", "pref")) {
				// 保存
				// GameObjectをシリアライズ
				json11::Json::object serial;
				Serialize(serial);
				// JSONオブジェクトを文字列化
				json11::Json jsn = serial;
				std::string text = jsn.dump(true);	// 文字列化
				// ファイルへ保存
				std::ofstream ofs(filepath);
				if (ofs) {
					ofs.write(text.c_str(), text.size());
				}
				// ログ表示
				GAMESYS.m_editorData.LogWindow.AddLog(u8"Prefab保存[%s]", filepath.c_str());
			}
		}

		//------------------------
		// Prefab読み込み
		//------------------------
		if (ImGui::Selectable(u8"Prefabファイルを読み込み")) {

			std::string filepath;
			if (KdWindow::OpenFileDialog(filepath, "Prefab開く", "prefファイル\0*.pref\0")) {
				// JSONファイル読み込み
				json11::Json jsonObj = KdLoadJSONFile(filepath);
				if (jsonObj.is_null() == false) {
					// GameObject作成
					KdSptr<GameObject> obj = std::make_shared<GameObject>();

					// デシリアライズ
					obj->Deserialize(jsonObj);

					// 自分を親として設定
					obj->SetParent(shared_from_this());
				}
			}
		}

		//------------------------
		// 新規作成
		//------------------------
		if (ImGui::Selectable(u8"新規GameObjectを作成")) {
			KdSptr<GameObject> obj = std::make_shared<GameObject>();
			// 自分を親としｔ設定
			obj->SetParent(shared_from_this());
		}

		// 必ず最後に呼ぶ
		ImGui::EndPopup();
	}
	//-----------ここまでメニュー-----------------------------------

	//-----------------------
	// 子もImGuiTree処理
	//-----------------------
	if (bOpen) {
		for (auto&& child : m_children) {
			child->Editor_ImGuiTree();
		}

		// TreeNodeExの最後には絶対に実行する
		ImGui::TreePop();
	}

	// 
	ImGui::PopID();
}
