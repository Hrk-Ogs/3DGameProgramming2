#pragma once

//=======================================
// �R���|�[�l���g��{�N���X
//=======================================
class BaseComponent :public std::enable_shared_from_this<BaseComponent>
{
public:

	// ���z�f�X�g���N�^
	virtual ~BaseComponent(){}

	//============================
	// ���쏈��
	//============================
	// ����Ɉ�x�������s�����֐�
	virtual void Start() {}
	// ������s�����X�V����
	virtual void Update() {}
	// ������s�����`��֐�
	virtual void Draw() {}


	//============================
	// ImGui����
	//============================
	virtual void Editor_ImGuiUpdate()
	{
		ImGui::Checkbox("Enable", &m_enable);
	}

	//============================
	// �V���A���C�Y/�f�V���A���C�Y
	//============================
	// JSON�f�[�^���玩���̃f�[�^�𕜌�����
	virtual void Deserialize(const json11::Json& jsonObj)
	{
		KdJsonGet(jsonObj["Enable"], m_enable);
	}

	// ������JSON�f�[�^��
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		// �����̃N���X���𕶎���œ����
		outJsonObj["ClassName"] = &typeid(*this).name()[6];
		outJsonObj["Enable"] = m_enable;
	}

	//============================
	// �ݒ�E�擾
	//============================
	// �I�[�i�[�i������j���擾
	KdSptr<GameObject> GetOwner() const { return m_pOwner.lock(); }
	// �I�[�i�[�̐ݒ�
	void SetOwner(KdSptr<GameObject>& obj) { m_pOwner = obj; }

	// �L���t���O�̎擾


	// �L���t���O�̐ݒ�
	

protected:

	// �L���t���O
	bool			m_enable = true;

	// �쐬�����āH�̃t���O
	bool			m_isNew = true;

	// ������ւ̃A�h���X
	KdWptr<GameObject>	m_pOwner;

};