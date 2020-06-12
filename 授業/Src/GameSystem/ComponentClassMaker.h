#pragma once

class BaseComponent;
//=========================================
//
// �R���|�[�l���g�N���X���[�J�[
// �ERegister�֐��Ŏw��^�̃N���X��o�^���Ă����΁A
// MakeShared�֐��ŕ����񖼂���C���X�^���X���쐬�ł���
//
//=========================================
class ComponentClassMaker
{
public:
	// �N���X��o�^
	template<class T>
	void Register()
	{
		m_map[&typeid(T).name()[6]] = []() { return std::make_shared<T>(); };
		// ��&typeid(T).name()[6]�͊��ˑ��ł��B
		// ��VisualStudio�ȊO�̎��͕ύX����K�v������܂��B
	}
	// �w�薼�̃N���X�̃��������m�ۂ��Ԃ�
	KdSptr<BaseComponent> MakeShared(const std::string& className)
	{
		auto it = m_map.find(className);
		if (it == m_map.end())return nullptr;
		// �|�C���^���w��^�֕ϊ�
		return (*it).second();
	}
	// �o�^����Ă���N���X�����X�g���擾����(��ɃG�f�B�^�[�Ŏg�p)
	std::vector<std::string> GetClassNameList()
	{
		std::vector<std::string> list;
		for (auto&& n : m_map)
		{
			list.push_back(n.first);
		}
		return list;
	}
private:
	// �N���X�}�b�v
	std::unordered_map<std::string, std::function<KdSptr<BaseComponent>()>> m_map;
	//----------------------------
	// �V���O���g���p�^�[��
	//----------------------------
private:
	ComponentClassMaker() {}
public:
	static ComponentClassMaker& GetInstance()
	{
		static ComponentClassMaker instance;
		return instance;
	}
};