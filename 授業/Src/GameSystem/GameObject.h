#pragma once

class BaseComponent;	// �N���X�̉���`
class TransformComponent;

//==============================================
// �Q�[���I�u�W�F�N�g�N���X
//==============================================
class GameObject final : public std::enable_shared_from_this<GameObject>
{
public:

	// ������w�̎q�����ΏۂɁA�L����GameObject��gameObjectList�֓o�^����
	void Collect(std::vector<GameObject*>& gameObjectList);

	// 
	void Init();

	// �폜����
	void Destroy() { m_isDestroy = true; }
	// �폜�t���O�擾
	bool IsDestory()const { return m_isDestroy; }


	//=========================================
	// �R���|�[�l���g
	//=========================================
	// �R���|�[�l���g���X�g�擾
	const std::list<KdSptr<BaseComponent>>& GetComponentList()const { return m_components; }

	// �R���|�[�l���g�ǉ��֐�
	// comp		�c�c�ǉ�����R���|�[�l���g
	void AddComponent(const KdSptr<BaseComponent>& comp);

	// �R���|�[�l���g�ǉ��i�N���X���𕶎���Ŏw��Version)
	// className	�c�c�n���������R���|�[�l���g�̃N���X��
	// �߂�l		�c�c�ǉ����ꂽ�R���|�[�l���g
	KdSptr<BaseComponent> AddComponent(const std::string& className);

	// �w��^�̃R���|�[�l���g���������擾
	template<class T>
	KdSptr<T> GetComponent()
	{
		// �S�R���|�[�l���g����A�^TD�̃N���X��������
		for (auto&& comp : m_components) {
			// �_�E���L���X�g��T�^�ɕϊ��ł���H
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p) {
				// �ł����BT�^�ƌ���B
				return p;
			}
		}
		return nullptr;
	}

	//=========================================
	// �e�q�\��
	//=========================================
	// �q���X�g�擾
	const std::list<KdSptr<GameObject>>& GetChildren() { return m_children; }
	// �e��ݒ�
	void SetParent(const KdSptr<GameObject>& newparent);
	// �e���擾
	KdSptr<GameObject> GetParent() const { return m_parent.lock(); }


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

	// TransformComponent�擾
	KdSptr<TransformComponent> Transform()
	{
		// �擪�̃R���|�[�l���g��TransformComponent�Ƃ��Ď擾����
#ifdef _DEBUG
		// ���f�o�b�O�r���h���́A�_�i�~�b�N�L���X�g�Ō^������s��
		return std::dynamic_pointer_cast<TransformComponent>(m_components.front());
#else
		// �������[�X�r���h���́A�擪�̂��̂�Transform�ƌ��ߑł��Ŏ擾����i�����̕��ׂ����炷���߁j
		return std::static_pointer_cast<TransformComponent>(m_components.front());
#endif
	}

private:
	// �L���t���O
	bool				m_enable = true;
	// ���O
	std::string			m_name = "GameObject";
	
	// �폜�t���O
	bool				m_isDestroy = false;

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

};

