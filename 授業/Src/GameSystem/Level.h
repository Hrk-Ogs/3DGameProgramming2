#pragma once


//===================================
// Level�N���X
//===================================
class Level :public std::enable_shared_from_this<Level>
{
public:

	// �t�@�C���p�X�擾
	const std::string& GetFilepath() const { return m_filePath; }
	// �t�@�C���p�X�ݒ�
	void SetFilepath(const std::string& path) { m_filePath = path; }

	// �S�Ắu�L���v��GmaeOvject�AgameObjectList�����W����
	void CollectGameObject(std::vector<GameObject*>& gameObjectList)
	{
		if (m_enable == false)return;
		m_rootObject->Collect(gameObjectList);
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
		m_rootObject->Init();
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