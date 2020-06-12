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

};