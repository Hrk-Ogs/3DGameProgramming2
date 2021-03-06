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
#include "Component/ParticleComponent.h"
#include "Component/GraphicsSettingComponent.h"
#include "Component/UIComponent.h"

// 
#include "Component/Collider/CollisionManager.h"


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

	// 波法線マップ取得
	KdSptr<KdTexture> GetWaveNormalMap() { return m_texWave.m_rtNormal; }

	// 初期設定
	void Init();

	// 実行
	void Run();

	// ImGui処理
	void Editor_ImGuiUpdate();

	// 実行中？
	bool IsPlay() const { return m_isPlay; }

	// Prefabファイルをインスタンス化しLevelに追加する
	KdSptr<GameObject> Instantiate(const std::string& prefabFilename);

	// レベル切り替え（予約）
	void ChangeLevel(const std::string& levelFilename)
	{
		m_nextLevelFilename = levelFilename;
	}

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

		// ImGuizmo
		ImGuizmo::MODE		GizmoMode = ImGuizmo::LOCAL;	// 座標系モード
		ImGuizmo::OPERATION	GizmoOperation = ImGuizmo::TRANSLATE;	// 操作モード
	};

	EditorData	m_editorData;

	// デバッグ描画システム
	KdDebugLineDraw& DebugLine() { return m_debugLineDraw; }

	// レベルの取得
	KdSptr<Level> GetLevel() const { return m_level; }

private:

	// 実行フラグ
	bool						m_isPlay = false;

	// 収集GameObjectリスト
	std::vector<GameObject*>	m_tempGameObjects;
	// レンダリングデータ用
	RenderingData				m_tempRenderingDate;

	// Level
	KdSptr<Level>			m_level;

	// 次のレベルのファイル名（予約）
	std::string				m_nextLevelFilename;

	// デバッグ線秒がシステム
	KdDebugLineDraw			m_debugLineDraw;

	// コリジョン管理クラス
	KdSptr<CollisionManager> m_collisionMgr = std::make_shared<CollisionManager>();

	// グラフィックス
	KdSptr<KdTexture> m_texOpaqueWork; // 結果用(不透明物のみ)
	KdSptr<KdTexture> m_texWork; // 結果用
	KdSptr<KdTexture> m_texHighBrightness; // 高輝度用
	KdBlurTexture m_texBlur; // ブラーテクスチャ
	KdWaveTexture m_texWave;

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
