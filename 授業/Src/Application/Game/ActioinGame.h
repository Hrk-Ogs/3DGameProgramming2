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

	// �ݒu����ւ̃A�h���X�i�R�����݂�΁A�N�̏�ɗ����Ă��邩�킩��j
	KdWptr<GameObject> m_wpGround;

	// �̓x�N�g��
	// �E���̃L�����N�^�[�Ɋ|�����Ă����
	// �E�����̂�����悤�Ȉړ��Ŏg�p����i�d�͂Ȃǁj
	KdVec3 m_vForce;

	// �d��
	void CalcGravity()
	{
		const float kGravityPower = 0.01f;
		m_vForce.y -= kGravityPower;
	}
	// ���C
	void CalcFriction()
	{
		const float kFrictionGround = 0.9f;	// �n�㎞�̖��C��
		const float kFrictionAir = 0.99f;	// �󒆎��̖��C��
		// �󒆎�
		if (m_wpGround.expired()) {
			m_vForce *= kFrictionAir;
		}
		// �n�㎞
		else {
			m_vForce *= kFrictionGround;
		}
	}

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

	// �W�����v�J�n
	class State_JumpStart :public State_Base
	{
	public:
		// �s������
		virtual void Update() override;
		KdVec3 m_force;	// ���ۂɃW�����v����Ƃ��̗�
	};

	// �W�����v��
	class State_Jump :public State_Base
	{
	public:
		// �s������
		virtual void Update() override;
	};

	// ���n
	class State_Landing :public State_Base
	{
	public:
		// �s������
		virtual void Update() override;
	};

	// �U��
	class State_Attack :public State_Base
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

//=================================================
//
// TPS�n�J�����̓���������R���|�[�l���g�iCamerComponent�ł͂Ȃ��j
//
//=================================================
class CharacteCameraController : public BaseComponent
{
public:
	// �p�����[�^�擾�E�ݒ�
	KdVec2& CamAngles() { return m_camAngles; }
	KdVec3& CamOffset() { return m_camOffset; }

	// �X�V����
	virtual void Update()override;

	// ImGui����
	virtual void Editor_ImGuiUpdate()override
	{
		BaseComponent::Editor_ImGuiUpdate();

		ImGui::DragFloat2(u8"�J�����p�x", &m_camAngles.x, 0.1f);
		ImGui::DragFloat3(u8"�J�����I�t�Z�b�g", &m_camOffset.x, 0.01f);
	}

	//==============================
	// Serialize/Deserialize
	//==============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonData) 
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["CamAngles"], m_camAngles);
		KdJsonGet(jsonData["CamOffset"], m_camOffset);
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["CamAngles"] = m_camAngles.ToArray();
		outJsonObj["CamOffset"] = m_camOffset.ToArray();
	}

private:

	// �J�����p�x
	KdVec2 m_camAngles;
	// 
	KdVec3 m_camOffset;

};



