// インクルードガード
#pragma once

// コンポーネントクラスメーカー
#include "ComponentClassMaker.h"

// ImGuiヘルパー
#include "ImGuiHelper.h"

// エディターカメラ
#include "EditorCamera.h"

// ゲームシステム系のヘッダーをインクルード
#include "GameObject.h"

// コンポーネント
#include "Component/BaseComponent.h"
#include "Component/TransformComponent.h"
#include "Component/ModelComponent.h"
#include "Component/InputComponent.h"


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
		// 現在選択しているGameObject
		KdWptr<GameObject>	SelectObj;
		// ログウィンドウ
		ImGuiLogWindow		LogWindow;
	};

	EditorData	m_editorData;


private:

	// Rootオブジェクト
	//KdSptr<GameObject>		m_rootObject;

	// Level
	KdSptr<Level>			m_level;


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
