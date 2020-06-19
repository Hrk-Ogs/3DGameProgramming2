#pragma once

class BaseComponent;
class CameraComponent;
class LightComponent;

//===========================================
// �`��f�[�^
//===========================================
struct RenderingData
{
	// �`��J����
	CameraComponent*				m_camera = nullptr;

	// �s�����RD�`����W���X�g
	std::vector<BaseComponent*>		m_drawList;
	// �������RD�`����W���X�g
	std::vector<BaseComponent*>		m_drawTransparentList;
	// �������G�t�F�N�g�RD�`����W���X�g
	std::vector<BaseComponent*>		m_drawEffectList;
	// �QD�`����W���X�g
	std::vector<BaseComponent*>		m_drawSpriteList;

	// Phase�萔
	enum
	{
		kGenShadowMap,			// �V���h�E�}�b�v����
		kOpaquePhase,			// �s�����`��
		kTransparentPhase,		// �������`��
		kEffectPhase,			// �G�t�F�N�g�`��
		kSpritePhase,			// 2D�`��
	};

	// �o�^�f�[�^���N���A����
	void Reset()
	{
		m_camera = nullptr;
		m_drawList.clear();
		m_drawTransparentList.clear();
		m_drawEffectList.clear();
		m_drawSpriteList.clear();
	}
};

