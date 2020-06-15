// �C���N���[�h�K�[�h
#pragma once

// �R���|�[�l���g�N���X���[�J�[
#include "ComponentClassMaker.h"

// ImGui�w���p�[
#include "ImGuiHelper.h"

// �G�f�B�^�[�J����
#include "EditorCamera.h"

// �Q�[���V�X�e���n�̃w�b�_�[���C���N���[�h
#include "GameObject.h"

// �R���|�[�l���g
#include "Component/BaseComponent.h"
#include "Component/TransformComponent.h"
#include "Component/ModelComponent.h"
#include "Component/InputComponent.h"


// ���x��
#include "Level.h"


//===============================
//
// �Q�[���V�X�e��
//
//===============================
class GameSystem
{
public:

	// �����ݒ�
	void Init();

	// ���s
	void Run();

	// ImGui����
	void Editor_ImGuiUpdate();

	//================================================
	// �I�I��������G�f�B�^�[��p�f�[�^�I�I�I
	//================================================
	struct EditorData
	{
		// �G�f�B�^�[�J����
		EditorCamera		Camera;
		// ���ݑI�����Ă���GameObject
		KdWptr<GameObject>	SelectObj;
		// ���O�E�B���h�E
		ImGuiLogWindow		LogWindow;
	};

	EditorData	m_editorData;


private:

	// Root�I�u�W�F�N�g
	//KdSptr<GameObject>		m_rootObject;

	// Level
	KdSptr<Level>			m_level;


	//============================
	// �V���O���g���p�^�[��
	//============================
private:
	GameSystem(){}

public:
	static GameSystem& getInstance()
	{
		static GameSystem instance;
		return instance;
	}
};

#define GAMESYS GameSystem::getInstance()
