// �C���N���[�h�K�[�h
#pragma once

// �R���|�[�l���g�N���X���[�J�[
#include "ComponentClassMaker.h"

// ImGui�w���p�[
#include "ImGuiHelper.h"

// �G�f�B�^�[�J����
#include "EditorCamera.h"

// RenderingData
#include "RenderingData.h"

// �Q�[���V�X�e���n�̃w�b�_�[���C���N���[�h
#include "GameObject.h"

// �R���|�[�l���g
#include "Component/BaseComponent.h"
#include "Component/TransformComponent.h"
#include "Component/ModelComponent.h"
#include "Component/InputComponent.h"
#include "Component/SpriteComponent.h"
#include "Component/CameraComponent.h"
#include "Component/LightComponent.h"
#include "Component/ParticleComponent.h"

// 
#include "Component/Collider/CollisionManager.h"


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

	// ���s���H
	bool IsPlay() const { return m_isPlay; }

	// ���x���؂�ւ��i�\��j
	void ChangeLevel(const std::string& levelFilename)
	{
		m_nextLevelFilename = levelFilename;
	}

	//================================================
	// �I�I��������G�f�B�^�[��p�f�[�^�I�I�I
	//================================================
	struct EditorData
	{
		// �G�f�B�^�[�J����
		EditorCamera		Camera;
		bool				FreeCameraMode = false;		// �����I��Editor�̃J�������g�p����B

		// ���ݑI�����Ă���GameObject
		KdWptr<GameObject>	SelectObj;
		// ���O�E�B���h�E
		ImGuiLogWindow		LogWindow;
	};

	EditorData	m_editorData;

	// �f�o�b�O�`��V�X�e��
	KdDebugLineDraw& DebugLine() { return m_debugLineDraw; }

	// ���x���̎擾
	KdSptr<Level> GetLevel() const { return m_level; }

private:

	// ���s�t���O
	bool						m_isPlay = false;

	// ���WGameObject���X�g
	std::vector<GameObject*>	m_tempGameObjects;
	// �����_�����O�f�[�^�p
	RenderingData				m_tempRenderingDate;

	// Level
	KdSptr<Level>			m_level;

	// ���̃��x���̃t�@�C�����i�\��j
	std::string				m_nextLevelFilename;

	// �f�o�b�O���b���V�X�e��
	KdDebugLineDraw			m_debugLineDraw;

	// �R���W�����Ǘ��N���X
	KdSptr<CollisionManager> m_collisionMgr = std::make_shared<CollisionManager>();

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
