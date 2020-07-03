#include "GameSystem.h"

#include "GameObject.h"

void GameObject::Collect(std::vector<GameObject*>& gameObjectList)
{
	// 無効時は対象外
	if (m_enable == false)return;

	// 処理対象に登録する
	gameObjectList.push_back(this);

	// 子も再帰で実行していく（イテレータでループしてるのは、削除機能も後に実装するため）
	for (auto it = m_children.begin(); it != m_children.end();) {
		// 削除フラグがOn？
		if ((*it)->IsDestory()) {
			// リストから消す
			it = m_children.erase(it);
			continue;
		}

		// 登録
		(*it)->Collect(gameObjectList);

		// 次へ
		++it;
	}
}

void GameObject::Init()
{
	// TransformComponentの追加
	m_components.clear();
	auto trans = std::make_shared<TransformComponent>();
	AddComponent(trans);
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
	// クラス名からインスタンスを生成
	KdSptr<BaseComponent> comp = ComponentClassMaker::GetInstance().MakeShared(className);
	/*
	if (className == "ModelComponent") {
		comp = std::make_shared<ModelComponent>();
	}
	if (className == "ModelComponent") {
		comp = std::make_shared<TransformComponent>();
	}
	*/

	if (comp == nullptr)return nullptr;

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

KdSptr<GameObject> GameObject::Find(std::string_view name)
{
	// 名前を判定
	if (m_name == name)return shared_from_this();

	// 子も検索
	for (auto&& child : m_children) {
		auto ret = child->Find(name);
		// 見つかったらそのまま返す
		if (ret)return ret;
	}

	return nullptr;
}

void GameObject::Deserialize(const json11::Json& jsonObj)
{
	// 有効フラグ
	KdJsonGet(jsonObj["Enable"], m_enable);
	// 名前
	KdJsonGet(jsonObj["Name"], m_name);
	// 質量
	KdJsonGet(jsonObj["Mass"], m_mass);

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
	outJsonObj["Mass"] = m_mass;

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

	// 質量
	ImGui::InputFloat(u8"質量", &m_mass);

	// 全コンポーネントを動作
	//for (auto&& comp : m_components) {
	for(auto&& it=m_components.begin();it!=m_components.end();)
	{
		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 2));

		auto& comp = (*it);

		ImGui::PushID(comp.get());

		bool bOpen = ImGui::CollapsingHeader(&typeid(*comp).name()[6], ImGuiTreeNodeFlags_DefaultOpen);

		// メニュー
		if (ImGui::BeginPopupContextItem("ComponentMenuPopup")) {
			if (ImGui::Selectable(u8"削除")) {
				it = m_components.erase(it);
				bOpen = false;
			}

			ImGui::EndPopup();
		}

		if (bOpen) {
			comp->Editor_ImGuiUpdate();
		}

		ImGui::PopID();

		if (it == m_components.end())break;
		++it;
	}

	// コンポーネント追加ボタン
	ImGui::Dummy(ImVec2(0, 2));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 5));

	ImGui::Button(u8"コンポーネント追加");
	// ↑のボタンをクリックするとポップアップメニュー表示
	if (ImGui::BeginPopupContextItem("AddComponentPopup", 0)) {
		// ComponentClassMakerに登録されているクラス名を一覧で取得し、リスト表示する
		std::vector<std::string> classList = ComponentClassMaker::GetInstance().GetClassNameList();
		for (auto&& className : classList) {
			if (ImGui::Selectable(className.c_str())) {
				AddComponent(className);
			}
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

	//---------------------
	// Drag & Drop
	//---------------------
	//Drag
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		// GameObjectのアドレスを数値として記憶しておく
		intptr_t n = (intptr_t)this;
		ImGui::SetDragDropPayload("GameObjectDragDrop", &n, sizeof(n));
		ImGui::Text(u8"親となるGameObjectを選択してください");
		ImGui::EndDragDropSource();
	}
	// Drop
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObjectDragDrop")) {
			IM_ASSERT(payload->DataSize == sizeof(intptr_t));
			// int型で記憶していたアドレスをOjbectに復元
			GameObject* srcObj = (GameObject*)(*(intptr_t*)payload->Data);

			// 親子構造チェック（親から子へのドロップダウンはまずいのでそれをチェック!）
			GameObject* o = this;
			while (o != nullptr) {
				if (o == srcObj)break;
				// 一つ上の親へ
				o = o->GetParent().get();
			}
			if (o == nullptr) {
				// そいつの子になる
				srcObj->SetParent(shared_from_this());
			}
		}
		ImGui::EndDragDropTarget();
	}


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
		// 新規作成
		//------------------------
		if (ImGui::Selectable(u8"新規GameObjectを作成")) {
			KdSptr<GameObject> obj = std::make_shared<GameObject>();
			obj->Init();
			// 自分を親としｔ設定
			obj->SetParent(shared_from_this());

			// 選択状態にする
			GAMESYS.m_editorData.SelectObj = obj;

		}

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
		// 複製
		//------------------------
		if (ImGui::Selectable(u8"複製")) {
			// 自分の親
			auto parent = GetParent();
			if (parent) {

				// 自分をシリアライズ
				json11::Json::object serial;
				Serialize(serial);

				// GameObjectを作成（メモリ確保）
				KdSptr<GameObject> obj = std::make_shared<GameObject>();

				// JSONデータを流し込む
				obj->Deserialize(serial);

				// 兄弟として自分の親に登録
				obj->SetParent(parent);

				// 選択状態にする
				GAMESYS.m_editorData.SelectObj = obj;
			}
		}



		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 5));

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

					// 選択状態にする
					GAMESYS.m_editorData.SelectObj = obj;

				}
			}
		}

		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 5));

		//------------------------
		// 削除
		//------------------------
		if (ImGui::Selectable(u8"削除")) {
			Destroy();
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
