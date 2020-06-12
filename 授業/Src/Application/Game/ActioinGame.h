#pragma once


class CharacterController : public BaseComponent
{
public:
	// �X�V����
	virtual void Start() override;

	// �X�V����
	virtual void Update() override;

	// ImGui����
	virtual void Editor_ImGuiUpdate() override;


	//===============================
	// �V���A���C�Y/�f�V���A���C�Y
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

	// �̗�
	int m_hp = 600;

};