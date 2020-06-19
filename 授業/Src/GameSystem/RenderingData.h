#pragma once

class BaseComponent;
class CameraComponent;
class LightComponent;

//===========================================
// 描画データ
//===========================================
struct RenderingData
{
	// 描画カメラ
	CameraComponent*				m_camera = nullptr;

	// 不透明３D描画収集リスト
	std::vector<BaseComponent*>		m_drawList;
	// 半透明３D描画収集リスト
	std::vector<BaseComponent*>		m_drawTransparentList;
	// 半透明エフェクト３D描画収集リスト
	std::vector<BaseComponent*>		m_drawEffectList;
	// ２D描画収集リスト
	std::vector<BaseComponent*>		m_drawSpriteList;

	// Phase定数
	enum
	{
		kGenShadowMap,			// シャドウマップ生成
		kOpaquePhase,			// 不透明描画
		kTransparentPhase,		// 半透明描画
		kEffectPhase,			// エフェクト描画
		kSpritePhase,			// 2D描画
	};

	// 登録データをクリアする
	void Reset()
	{
		m_camera = nullptr;
		m_drawList.clear();
		m_drawTransparentList.clear();
		m_drawEffectList.clear();
		m_drawSpriteList.clear();
	}
};

