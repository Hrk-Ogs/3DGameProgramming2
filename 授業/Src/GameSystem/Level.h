#pragma once


//===================================
// Levelクラス
//===================================
class Level :public std::enable_shared_from_this<Level>
{
public:
	// 仮
	void Update()
	{
		if (m_enable == false)return;
		m_rootObject->Update();
	}

	// 仮
	void Draw()
	{
		if (m_enable == false)return;
		m_rootObject->Draw();
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