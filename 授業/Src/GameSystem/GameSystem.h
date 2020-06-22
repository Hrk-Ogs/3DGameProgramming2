// インクルードガード
#pragma once

// コンポーネントクラスメーカー
#include "ComponentClassMaker.h"

// ImGuiヘルパー
#include "ImGuiHelper.h"

// エディターカメラ
#include "EditorCamera.h"

// RenderingData
#include "RenderingData.h"

// ゲームシステム系のヘッダーをインクルード
#include "GameObject.h"

// コンポーネント
#include "Component/BaseComponent.h"
#include "Component/TransformComponent.h"
#include "Component/ModelComponent.h"
#include "Component/InputComponent.h"
#include "Component/SpriteComponent.h"
#include "Component/CameraComponent.h"
#include "Component/LightComponent.h"


// レベル
#include "Level.h"


//===============================
//
// ゲームシステム
//
//===============================
class GameSystem
{
public:

	// 初期設定
	void Init();

	// 実行
	void Run();

	// ImGui処理
	void Editor_ImGuiUpdate();

	//================================================
	// ！！ここからエディター専用データ！！！
	//================================================
	struct EditorData
	{
		// エディターカメラ
		EditorCamera		Camera;
		bool				FreeCameraMode = false;		// 強制的にEditorのカメラを使用する。

		// 現在選択しているGameObject
		KdWptr<GameObject>	SelectObj;
		// ログウィンドウ
		ImGuiLogWindow		LogWindow;
	};

	EditorData	m_editorData;

	// デバッグ描画システム
	KdDebugLineDraw& DebugLine() { return m_debugLineDraw; }

private:

	// 収集GameObjectリスト
	std::vector<GameObject*>	m_tempGameObjects;
	// レンダリングデータ用
	RenderingData				m_tempRenderingDate;

	// Level
	KdSptr<Level>			m_level;

	// デバッグ線秒がシステム
	KdDebugLineDraw			m_debugLineDraw;

	//============================
	// シングルトンパターン
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
