#pragma once


//===================================
// Levelクラス
//===================================
class Level :public std::enable_shared_from_this<Level>
{
public:

	// ファイルパス取得
	const std::string& GetFilepath() const { return m_filePath; }
	// ファイルパス設定
	void SetFilepath(const std::string& path) { m_filePath = path; }

	// 全ての「有効」なGmaeOvject、gameObjectListを収集する
	void CollectGameObject(std::vector<GameObject*>& gameObjectList)
	{
		if (m_enable == false)return;
		m_rootObject->Collect(gameObjectList);
	}

	//============================
	// シリアライズ/デシリアライズ
	//============================
	void Serialize(json11::Json::object& outJsonObj) const;
	void Deserialize(const json11::Json& jsonObj);

	//============================
	// ImGui処理
	//============================
	void Editor_ImGuiupdate();

	Level()
	{
		// 初期状態で必ずRootObjectは存在するように
		m_rootObject = std::make_shared<GameObject>();
		m_rootObject->Init();
		m_rootObject->SetName("Root");
	}

private:

	// ファイルのパス記憶用
	std::string				m_filePath;

	// 有効フラグ
	bool					m_enable = true;

	// Rootオブじぇっくと
	KdSptr<GameObject>		m_rootObject;

};