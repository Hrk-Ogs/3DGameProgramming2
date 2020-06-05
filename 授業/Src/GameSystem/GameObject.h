#pragma once

class BaseComponent;	// クラスの仮定義

//==============================================
// ゲームオブジェクトクラス
//==============================================
class GameObject final : public std::enable_shared_from_this<GameObject>
{
public:

	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	//=========================================
	// 親子構造
	//=========================================
	// 子リスト取得
	const std::list<KdSptr<GameObject>>& GetChildren() { return m_children; }
	// 親を設定
	void SetParent(const KdSptr<GameObject>& newparent);


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

	// 仮
	KdMatrix& Transform() { return m_transform; }


private:
	// 有効フラグ
	bool				m_enable = true;
	// 名前
	std::string			m_name = "GameObject";
	
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



	// 仮　オブジェクトの行列
	KdMatrix			m_transform;

};

