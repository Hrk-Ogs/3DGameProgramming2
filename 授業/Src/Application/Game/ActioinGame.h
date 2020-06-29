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

	//================
	// �s���X�e�[�g�}�V��
	//================
	// �s���X�e�[�g��{�N���X
	class State_Base
	{
	public:

		virtual ~State_Base(){}
		// �s������
		virtual void Update() {}
		// �L�����ւ̎Q��
		CharacterController* m_chara = nullptr;
	};

	//�u�����v�s���N���X
	class State_Stand :public State_Base
	{
	public:
		// �s������
		virtual void Update() override;
	};

	//�u����v�s���N���X
	class State_Run :public State_Base
	{
	public:
		// �s������
		virtual void Update() override;
	};

	// ���݂̍s���X�e�[�g�ւ̎Q��
	KdSptr<State_Base>	m_nowState;

	// �S�s���X�e�[�g�����炩���ߍ쐬���ĕۊǂ��Ă����ꏊ
	std::unordered_map<std::string, KdSptr<State_Base>>	m_stateStorage;

};


//=========================================
// �^�C�g������N���X
//=========================================
class TitleController :public BaseComponent
{
public:
	virtual void Update() override
	{
		// �������̓X�L�b�v
		if (m_enable == false)return;
		// ��~���̓X�L�b�v
		if (GAMESYS.IsPlay() == false)return;

		// ���̓R���|�[�l���g
		auto input = GetOwner()->GetComponent<InputComponent>();
		if (input == nullptr)return;

		// ����{�^��
		if (input->IsButtonPressed(GameButtons::A)) {
			// Level�؂�ւ��\��
			GAMESYS.ChangeLevel("Data\\Level\\Stage1.level");
		}
	}

	// ImGui����
	virtual void Editor_ImGuiUpdate()override
	{
		BaseComponent::Editor_ImGuiUpdate();

		auto input = GetOwner()->GetComponent<InputComponent>();
		if (input == nullptr) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(u8"InputComponent��ǉ����Ă��������I�I");
			ImGui::PopStyleColor();
		}
	}

private:

};

