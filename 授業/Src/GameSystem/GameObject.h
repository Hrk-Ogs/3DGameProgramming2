#pragma once

class BaseComponent;	// �N���X�̉���`

//==============================================
// �Q�[���I�u�W�F�N�g�N���X
//==============================================
class GameObject final : public std::enable_shared_from_this<GameObject>
{
public:

	// �X�V����
	void Update();
	// �`�揈��
	void Draw();

	//=========================================
	// �e�q�\��
	//=========================================
	// �q���X�g�擾
	const std::list<KdSptr<GameObject>>& GetChildren() { return m_children; }
	// �e��ݒ�
	void SetParent(const KdSptr<GameObject>& newparent);


	//=========================================
	// �V���A���C�Y/�f�V���A���C�Y
	//=========================================
	// �����iJSON�f�[�^���珉���ݒ�j
	void Deserialize(const json11::Json& jsonObj);
	// �ۑ��iJSON�f�[�^���j
	void Serialize(json11::Json::object& outJsonObj)const;


	// ImGui����
	void Editor_ImGuiUpdate();
	// ImGui�c���[�\��
	void Editor_ImGuiTree();

	//=========================================
	// �ݒ�E�擾
	//=========================================
	const std::string& GetName()const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	// ��
	KdMatrix& Transform() { return m_transform; }


private:
	// �L���t���O
	bool				m_enable = true;
	// ���O
	std::string			m_name = "GameObject";
	
	//=========================================
	// �e�q
	//=========================================
	// �q���X�g
	std::list<KdSptr<GameObject>>	m_children;
	// �e�̃|�C���^
	KdWptr<GameObject>				m_parent;

	//=========================================
	// �R���|�[�l���g
	//=========================================
	std::list<KdSptr<BaseComponent>>	m_components;



	// ���@�I�u�W�F�N�g�̍s��
	KdMatrix			m_transform;

};

