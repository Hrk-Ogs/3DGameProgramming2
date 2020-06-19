#pragma once

class BaseComponent;	// クラスの仮定義
class TransformComponent;

//==============================================
// ゲームオブジェクトクラス
//==============================================
class GameObject final : public std::enable_shared_from_this<GameObject>
{
public:

	// 自分や陣の子たち対象に、有効なGameObjectをgameObjectListへ登録する
	void Collect(std::vector<GameObject*>& gameObjectList);

	// 
	void Init();

	// 削除する
	void Destroy() { m_isDestroy = true; }
	// 削除フラグ取得
	bool IsDestory()const { return m_isDestroy; }


	//=========================================
	// コンポーネント
	//=========================================
	// コンポーネントリスト取得
	const std::list<KdSptr<BaseComponent>>& GetComponentList()const { return m_components; }

	// コンポーネント追加関数
	// comp		……追加するコンポーネント
	void AddComponent(const KdSptr<BaseComponent>& comp);

	// コンポーネント追加（クラス名を文字列で指定Version)
	// className	……地下したいコンポーネントのクラス名
	// 戻り値		……追加されたコンポーネント
	KdSptr<BaseComponent> AddComponent(const std::string& className);

	// 指定型のコンポーネントを検索し取得
	template<class T>
	KdSptr<T> GetComponent()
	{
		// 全コンポーネントから、型TDのクラスを見つける
		for (auto&& comp : m_components) {
			// ダウンキャストでT型に変換できる？
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p) {
				// できた。T型と決定。
				return p;
			}
		}
		return nullptr;
	}

	//=========================================
	// 親子構造
	//=========================================
	// 子リスト取得
	const std::list<KdSptr<GameObject>>& GetChildren() { return m_children; }
	// 親を設定
	void SetParent(const KdSptr<GameObject>& newparent);
	// 親を取得
	KdSptr<GameObject> GetParent() const { return m_parent.lock(); }


	//=========================================
	// シリアライズ/デシリアライズ
	//=========================================
	// 復元（JSONデータから初期設定）
	void Deserialize(const json11::Json& jsonObj);
	// 保存（JSONデータ化）
	void Serialize(json11::Json::object& outJsonObj)const;


	// ImGui処理
	void Editor_ImGuiUpdate();
	// ImGuiツリー表示
	void Editor_ImGuiTree();


	//=========================================
	// 設定・取得
	//=========================================
	const std::string& GetName()const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	// TransformComponent取得
	KdSptr<TransformComponent> Transform()
	{
		// 先頭のコンポーネントをTransformComponentとして取得する
#ifdef _DEBUG
		// ※デバッグビルド時は、ダナミックキャストで型判定も行う
		return std::dynamic_pointer_cast<TransformComponent>(m_components.front());
#else
		// ※リリースビルド時は、先頭のものがTransformと決め打ちで取得する（検索の負荷を減らすため）
		return std::static_pointer_cast<TransformComponent>(m_components.front());
#endif
	}

private:
	// 有効フラグ
	bool				m_enable = true;
	// 名前
	std::string			m_name = "GameObject";
	
	// 削除フラグ
	bool				m_isDestroy = false;

	//=========================================
	// 親子
	//=========================================
	// 子リスト
	std::list<KdSptr<GameObject>>	m_children;
	// 親のポインタ
	KdWptr<GameObject>				m_parent;


	//=========================================
	// コンポーネント
	//=========================================
	std::list<KdSptr<BaseComponent>>	m_components;

};

