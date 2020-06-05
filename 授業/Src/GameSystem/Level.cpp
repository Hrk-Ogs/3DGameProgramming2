#include "GameSystem.h"

#include "Level.h"

void Level::Serialize(json11::Json::object& outJsonObj) const
{
	outJsonObj["Enable"] = m_enable;

	// RootObjectをシリアライズ
	json11::Json::object serialObject;
	m_rootObject->Serialize(serialObject);
	outJsonObj["GameObject"] = serialObject;
}

void Level::Deserialize(const json11::Json& jsonObj)
{
	KdJsonGet(jsonObj["Enable"], m_enable);

	m_rootObject = std::make_shared<GameObject>();
	m_rootObject->Deserialize(jsonObj["GameObject"]);
}

void Level::Editor_ImGuiupdate()
{
	// Levelファイルパス
	ImGui::InputText("Level File", &m_filePath);

	//
	ImGui::Dummy(ImVec2(0, 2));
	ImGui::Separator();

	// RootObject以下をツリー表示
	m_rootObject->Editor_ImGuiTree();

}
