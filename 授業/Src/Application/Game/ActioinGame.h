#pragma once


// �_���[�W�ʒm��
struct DamageArg
{
	int AtkPower = 0;		// �U���̈З�

};

// �_���[�W���ʕԐM��
struct DamageReply
{
	bool	IsGuard = false;		// �h�䂵����I
};

//=====================================
// �Q�[���L�����N�^�[��{�N���X
//=====================================
class BaseCharacter : public BaseComponent
{
public:
	// �_���[�W�ʒm
	// �Edmg �c�c�U���҂��瑗����_���[�W�ʒm�f�[�^
	// �Erep �c�c�U���҂֕ԐM������e
	virtual bool OnDamage(const DamageArg& dmg, DamageReply& rep) { return false; }

};



//=====================================
// �L�����N�^�[����
//=====================================
class CharacterController : public BaseCharacter
{
public:
	// �X�V����
	virtual void Start() override;

	// �X�V����
	virtual void Update() override;

	// �_���[�W�ʒm
// �Edmg �c�c�U���҂��瑗����_���[�W�ʒm�f�[�^
// �Erep �c�c�U���҂֕ԐM������e
	virtual bool OnDamage(const DamageArg& dmg, DamageReply& rep) override;


	// ImGui����
	virtual void Editor_ImGuiUpdate() override;


	//===============================
	// �V���A���C�Y/�f�V���A���C�Y
	//===============================
	virtual void Deserialize(const json11::Json& jsonObj)
	{
		BaseCharacter::Deserialize(jsonObj);
		KdJsonGet(jsonObj["Hp"], m_hp);
	}
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseCharacter::Serialize(outJsonObj);
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

	// ��낯
	class State_Stagger :public State_Base
	{
	public:
		// �s������
		virtual void Update() override;
	};

	// ���S
	class State_Die :public State_Base
	{
	public:
		// �s������
		virtual void Update() override;

		void Reset()
		{
			m_cnt = 0;
		}

	private:
		// �폜�܂ł̃J�E���^�[
		int m_cnt = 180;
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

//=========================================
//
// ���퐧��X�N���v�g
//
//=========================================
class WeaponScript :public BaseComponent
{
	struct Parameter;
public:
	// ����̐��\�擾
	const Parameter& GetParameter()const { return m_param; }

	// �L�����Ԃ�ݒ�
	void SetEnableTime(int count)
	{
		m_enableTime = count;
	}

	// �q�b�g�Ԋu�ݒ�
	void SetHitInterval(int value)
	{
		m_hitInterbal = value;
	}

	// �������X�g���N���A����
	void ClearIgnoreList()
	{
		m_ignoreList.clear();
	}

	// �X�V����
	virtual void Update() override;

	//=============================
	// Serialize/Deserialize
	//=============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonData) 
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["Param_Power"], m_param.Power);
		KdJsonGet(jsonData["Param_CrititalRate"], m_param.CrititalRate);
		KdJsonGet(jsonData["HitInterval"], m_hitInterbal);
		KdJsonGet(jsonData["EnablieTime"], m_enableTime);
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["Param_Power"] = m_param.Power;
		outJsonObj["Param_CrititalRate"] = m_param.CrititalRate;
		outJsonObj["HitInterval"] = m_hitInterbal;
		outJsonObj["EnablieTime"] = m_enableTime;
	}

	// 
	virtual void Editor_ImGuiUpdate()override
	{
		BaseComponent::Editor_ImGuiUpdate();

		ImGui::InputInt(u8"�З�", &m_param.Power);
		ImGui::InputInt(u8"�N���e�B�J����", &m_param.CrititalRate);

		ImGui::InputInt(u8"�q�b�g�X�g�b�v�Ԋu", &m_hitInterbal);
		ImGui::InputInt(u8"�L������", &m_enableTime);
	}


private:
	// �Œ�̕���̐��\
	struct Parameter 
	{
		int Power = 1;			// ����̈З�
		int CrititalRate = 0;	// �N���e�B�J����
	};
	Parameter m_param;

	// ���I�ȕ���̐��\
	int m_hitInterbal = 0;	

	// ���s���ϐ�
	int m_enableTime = 0;

	// �������X�g�i�X�ɓo�^����Ă���GameObject�̓q�b�g���Ă��Ă���������j
	std::unordered_map<const GameObject*, int> m_ignoreList;



};
