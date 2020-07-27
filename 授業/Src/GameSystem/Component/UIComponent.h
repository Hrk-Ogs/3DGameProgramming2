#pragma once


//==========================================
// �I�u�U�[�o�[�R���|�[�l���g��{�N���X
// �E�T�u�W�F�N�g(�{�^��)�N���X�ɉ����A�N�V���������������A���̃N���X�Ɂu�ʒm�v���s����
// �E���̃N���X���p�����A�Q�[�����L�̏������������N���X�ɂ��Ă����B
//==========================================
class UIObserverComponent : public BaseComponent
{
public:
	// (�ʒm)gameObj���N���b�N���ꂽ���Ɏ��s�����
	// �EgameObj �c �N���b�N���ꂽGameObject
	// �Emessage �c ���M����Ă���������
	virtual void NotifyClicked(KdSptr<GameObject> gameObj, const std::string& message) {}

};

//==========================================
// �T�u�W�F�N�g�R���|�[�l���g��{�N���X
// �E���̃N���X���p�����A�{�^���Ȃǂ����
//==========================================
class UISubjectComponent : public BaseComponent
{
public:
	// ��������
	virtual void Start() override;
protected:
	// �I�u�U�[�o�[���X�g(�ʒm��)
	std::vector<KdWptr<UIObserverComponent>> m_observers;
};

//==========================================
// UI�{�^���R���|�[�l���g
//==========================================
class UIButtonComponent : public UISubjectComponent
{
public:
	// �X�V����
	virtual void Update() override;
	virtual void Editor_ImGuiUpdate() override
	{
		UISubjectComponent::Editor_ImGuiUpdate();
		ImGui::Text(u8"�N���b�N���ɑ��M���镶����");
		ImGui::InputText(u8"Message", &m_onClickedMessage);
	}
	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonData)
	{
		UISubjectComponent::Deserialize(jsonData);
		KdJsonGet(jsonData["OnClickedMessage"], m_onClickedMessage);
	}
	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		UISubjectComponent::Serialize(outJsonObj);
		outJsonObj["OnClickedMessage"] = m_onClickedMessage;
	}
private:
	// �N���b�N���ɑ��M���郁�b�Z�[�W
	std::string m_onClickedMessage = "";
	// �N���b�N����p�t���O
	bool m_clickFlag = false;
};

