#pragma once

class BaseComponent;
class CameraComponent;
class LightComponent;

//===========================================
// `æf[^
//===========================================
struct RenderingData
{
	// `æJ
	CameraComponent*				m_camera = nullptr;

	// s§¾RD`æûWXg
	std::vector<BaseComponent*>		m_drawList;
	// ¼§¾RD`æûWXg
	std::vector<BaseComponent*>		m_drawTransparentList;
	// ¼§¾GtFNgRD`æûWXg
	std::vector<BaseComponent*>		m_drawEffectList;
	// QD`æûWXg
	std::vector<BaseComponent*>		m_drawSpriteList;

	// Phaseè
	enum
	{
		kGenShadowMap,			// VhE}bv¶¬
		kOpaquePhase,			// s§¾`æ
		kTransparentPhase,		// ¼§¾`æ
		kEffectPhase,			// GtFNg`æ
		kSpritePhase,			// 2D`æ
	};

	// o^f[^ðNA·é
	void Reset()
	{
		m_camera = nullptr;
		m_drawList.clear();
		m_drawTransparentList.clear();
		m_drawEffectList.clear();
		m_drawSpriteList.clear();
	}
};

