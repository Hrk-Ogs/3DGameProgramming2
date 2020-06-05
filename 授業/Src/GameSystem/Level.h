#pragma once


//===================================
// Level�N���X
//===================================
class Level :public std::enable_shared_from_this<Level>
{
public:
	// ��
	void Update()
	{
		if (m_enable == false)return;
		m_rootObject->Update();
	}

	// ��
	void Draw()
	{
		if (m_enable == false)return;
		m_rootObject->Draw();
	}


	//============================
	// �V���A���C�Y/�f�V���A���C�Y
	//============================
	void Serialize(json11::Json::object& outJsonObj) const;
	void Deserialize(const json11::Json& jsonObj);

	//============================
	// ImGui����
	//============================
	void Editor_ImGuiupdate();

	Level()
	{
		// ������ԂŕK��RootObject�͑��݂���悤��
		m_rootObject = std::make_shared<GameObject>();
		m_rootObject->SetName("Root");
	}

private:

	// �t�@�C���̃p�X�L���p
	std::string				m_filePath;

	// �L���t���O
	bool					m_enable = true;

	// Root�I�u����������
	KdSptr<GameObject>		m_rootObject;

};