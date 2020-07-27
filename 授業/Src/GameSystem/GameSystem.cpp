#include "GameSystem.h"

void GameSystem::Init()
{
	m_editorData.LogWindow.AddLog(u8"GameSystem起動...");

	// システムコンポーネント登録
	ComponentClassMaker::GetInstance().Register<TransformComponent>();
	ComponentClassMaker::GetInstance().Register<ModelComponent>();
	ComponentClassMaker::GetInstance().Register<InputComponent>();
	ComponentClassMaker::GetInstance().Register<PlayerInputComponent>();
	ComponentClassMaker::GetInstance().Register<SimpleAIInputComponent>();

	ComponentClassMaker::GetInstance().Register<SpriteComponent>();
	ComponentClassMaker::GetInstance().Register<CameraComponent>();
	ComponentClassMaker::GetInstance().Register<LightComponent>();
	ComponentClassMaker::GetInstance().Register<ParticleComponent>();

	ComponentClassMaker::GetInstance().Register<SphereColliderComponent>();
	ComponentClassMaker::GetInstance().Register<RayColliderComponent>();
	ComponentClassMaker::GetInstance().Register<BoxColliderComponent>();
	ComponentClassMaker::GetInstance().Register<MeshColliderComponent>();

	ComponentClassMaker::GetInstance().Register<GraphicsSettingComponent>();

	ComponentClassMaker::GetInstance().Register<UIButtonComponent>();

	// Level作成
	m_level = std::make_shared<Level>();

	// リリース版
#ifndef DEVELOPMENT_MODE
	// 実行モード
	m_isPlay = true;
	// コリジョンのデバッグ線秒がをOFF
	ColliderComponent::s_showDebugLine = false;
	// 初期レベル
	ChangeLevel("Data\\Level\\Title.level");
#endif
}

void GameSystem::Run()
{

	// デバッグ線を全てクリアする
	m_debugLineDraw.ResetLines();

	//==========================
	// レベルの切り替え処理
	//==========================
	if (m_nextLevelFilename.empty() == false) {
		// Level作成
		m_level = std::make_shared<Level>();

		// リソース管理クラス最適化
		// ※どこからも参照されていない不必要なリソースのみを解放する
		int cnt = KDResFactory.Optimize();
		m_editorData.LogWindow.AddLog(u8"%d個のリソースを解放", cnt);

		// JSON読み込み＆Levelのデシリアライズ
		json11::Json jsn = KdLoadJSONFile(m_nextLevelFilename);
		if (jsn.is_null() == false) {
			// デシリアライズ
			m_level->Deserialize(jsn);

			// ファイルパス記憶
			m_level->SetFilepath(m_nextLevelFilename);
		}
		// 予約をクリア
		m_nextLevelFilename = "";
	}

#ifdef DEVELOPMENT_MODE
	//==========================
	// ImGui開始
	//==========================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui Demo ウィンドウ表示 ※すごく参考になるウィンドウです。imgui_demo.cpp参照。
	ImGui::ShowDemoWindow(nullptr);

	// ImGuizmo開始
	ImGuizmo::BeginFrame();
#endif

	//==========================
	// GameObjectを収集
	//==========================
	// 収集GameObjectリストのリセット
	m_tempGameObjects.clear();

	// LevelからGameObjectを収集
	m_level->CollectGameObject(m_tempGameObjects);

	//==========================
	//
	// 更新処理
	//
	//==========================

	// このコリジョン管理クラスを使用するようにセット
	m_collisionMgr->SetManagerToDefault();

	// 処理の前に、全TransformComponentに対して行列を記憶させておく
	for (auto&& gameObj : m_tempGameObjects) {
		gameObj->Transform()->BackupMatrix();
	}

	// 収集したGameObjectの更新処理
	for (auto&& gameObj : m_tempGameObjects) {
		// 全コンポーネント実行（直接コンポーネントリストを取得し、実行していく）
		for (auto&& comp : gameObj->GetComponentList()) {
			// Start実行
			if (m_isPlay) {
				comp->CallStartOnce();
			}
			comp->Update();
		}
	}

	// 収集したGameObjectの更新処理
	for (auto&& gameObj : m_tempGameObjects) {
		// 全コンポーネント実行
		for (auto&& comp : gameObj->GetComponentList()) {
			// 更新処理
			comp->LateUpdate();
		}
	}


	// 「停止」時は、コライダー情報の更新のみ実行する
	// 「実行」時は、コライダー情報の更新とあたり判定を実行
	bool onlyUpdateCollider = (m_isPlay == false);
	// コリジョン処理、結果通知
	m_collisionMgr->Run(onlyUpdateCollider);

#ifdef DEVELOPMENT_MODE
	// エディターカメラ処理
	if (m_isPlay == false || m_editorData.FreeCameraMode) {
		// どのウィンドウの上にもマウスがいないときのみ
		if (ImGui::IsAnyWindowHovered() == false) {
			m_editorData.Camera.Update();
		}
	}
#endif

	//-----------------------
	// 描画準備
	//-----------------------
	// レンダリングデータをリセット
	m_tempRenderingDate.Reset();

	// ライト数リセット
	SHADER.m_cb8_Light.Work().DL_Cnt = 0;
	SHADER.m_cb8_Light.Work().PL_Cnt = 0;
	SHADER.m_cb8_Light.Work().SL_Cnt = 0;


	// GameOvject描画準備処理
	for (auto&& gameObj : m_tempGameObjects) {
		// 全コンポーネント実行
		for (auto&& comp : gameObj->GetComponentList()) {
			comp->PrepareDraw(m_tempRenderingDate);
		}
	}

	// ライトデータ書き込み（GPUに転送され描画（シェーダー）で使用される）
	SHADER.m_cb8_Light.Write();


	//==========================
	//
	// 描画
	//
	//==========================
	//-----------
	// カメラ
	//-----------
#ifdef DEVELOPMENT_MODE
	// カメラをシェーダーにセット
	if (m_tempRenderingDate.m_camera && m_editorData.FreeCameraMode == false && m_isPlay) {	// カメラコンポーネントがある時
		// カメラコンポーネントのカメラ情報をセットする
		m_tempRenderingDate.m_camera->SetCamera();
	}
	else {	// ないとき
		// Editorカメラセット
		m_editorData.Camera.SetToShader();
	}
#else
	if (m_tempRenderingDate.m_camera) {
		// カメラコンポーネントのカメラ情報をセットする
		m_tempRenderingDate.m_camera->SetCamera();
	}
#endif


	//-----------
	// ライト
	//-----------


	//-----------
	// 描画
	//-----------

	//++++++++++++++++++++
	// 3D描画（不透明）
	//++++++++++++++++++++
	// ※ここで、m_tempRenderingData.m_drawListをカメラから近い順番にソートすればパフォーマンスアップ

	for (auto&& comp : m_tempRenderingDate.m_drawList) {
		comp->Draw(RenderingData::kOpaquePhase);
	}

	//++++++++++++++++++++
	// 3D描画（半透明）
	//++++++++++++++++++++

	for (auto&& comp : m_tempRenderingDate.m_drawTransparentList) {
		comp->Draw(RenderingData::kTransparentPhase);
	}

	//++++++++++++++++++++
	// 3Dエフェクト描画（半透明）
	//++++++++++++++++++++
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);	 // Zバッファの書き込みをOFF
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);	// カリングを無し（画面描画）
	for (auto&& comp : m_tempRenderingDate.m_drawEffectList) {
		comp->Draw(RenderingData::kEffectPhase);
	}
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);	 // Zbaffa no書き込みをON
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);


	//++++++++++++++++++++
	// 2D描画
	//++++++++++++++++++++
	SHADER.m_spriteShader.Begin(true, false);
	for (auto&& comp : m_tempRenderingDate.m_drawSpriteList) {
		comp->Draw(RenderingData::kSpritePhase);
	}
	SHADER.m_spriteShader.End();

#ifdef DEVELOPMENT_MODE
	//===========================
	// GUI処理
	//===========================
	Editor_ImGuiUpdate();

	//-------------------
	// デバッグ線描画
	//-------------------
	m_debugLineDraw.Draw();

	//===========================
	// ImGui描画
	//===========================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}

// ImGui系の処理
void GameSystem::Editor_ImGuiUpdate()
{
	//===============================================
	// GameSystem
	//===============================================
	if (ImGui::Begin("GameSystem")) {

		//---------------------
		// エディター設定
		//---------------------

		// 実行/停止
		if (ImGui::Checkbox(u8"実行", &m_isPlay)) {
			// 実行前Levelをシリアライズしたもの置き場
			static json11::Json::object sSerialLevel;

			// 実行
			if (m_isPlay) {
				// 現在Levelの状態をシリアライズしておく
				m_level->Serialize(sSerialLevel);

				// 読み込みなども再現するため、一度リソース関係を全解放
				// （今回はすばやく実行したいので、ここはコメントにする）
				//m_editorData.LogWindow.AddLog(u8"リソース全解放");
				//KDResFactory.Clear():

				// デシリアライズで初期化
				m_level->Deserialize(sSerialLevel);
			}
			// 停止
			else {
				// 読み込みなども再現するため、一度リソースを全解放
				//m_editorData.LogWindow.AddLog(u8"リソースを全解放");
				//KDResFactory.Clear();

				// 再生前の状態へ戻す
				m_level->Deserialize(sSerialLevel);
			}
		}

		// 実行時のみCheckBoxを表示する
		if (m_isPlay) {
			ImGui::Indent();
			ImGui::Checkbox("FreeCamer Mode", &m_editorData.FreeCameraMode);
			ImGui::Unindent();
		}
		ImGui::Separator();




		//------------------
		// ImGuizmoの操作設定
		//------------------
		if (ImGui::CollapsingHeader("Gizmo", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::RadioButton("Local", m_editorData.GizmoMode == ImGuizmo::LOCAL)) {
				m_editorData.GizmoMode = ImGuizmo::LOCAL;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("World", m_editorData.GizmoMode == ImGuizmo::WORLD)) {
				m_editorData.GizmoMode = ImGuizmo::WORLD;
			}
			if (ImGui::RadioButton(u8"移動", m_editorData.GizmoOperation == ImGuizmo::TRANSLATE)) {
				m_editorData.GizmoOperation = ImGuizmo::TRANSLATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"回転", m_editorData.GizmoOperation == ImGuizmo::ROTATE)) {
				m_editorData.GizmoOperation = ImGuizmo::ROTATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"拡大", m_editorData.GizmoOperation == ImGuizmo::SCALE)) {
				m_editorData.GizmoOperation = ImGuizmo::SCALE;
			}
		}
	}
	ImGui::End();


	//===============================================
	// Levelウィンドウ
	//===============================================
	if (ImGui::Begin("Level", 0, ImGuiWindowFlags_MenuBar)) {

		//-------------------
		// メインメニュー
		//-------------------
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// 新規Level
				if (ImGui::Selectable(u8"新規Level"))
				{
					// リソースファクトリーをクリアする
					KDResFactory.Clear();
					// Level作成
					m_level = std::make_shared<Level>();
				}
				// Level読み込み
				if (ImGui::Selectable(u8"Level開く"))
				{
					std::string filepath = m_level->GetFilepath();
					if (KdWindow::OpenFileDialog(filepath, "Levelファイル開く", "Levelファイル\0*.level\0"))
					{
						// リソースファクトリーをクリアする
						KDResFactory.Clear();
						// JSON読み込み
						json11::Json jsn = KdLoadJSONFile(filepath);
						if (jsn.is_null() == false)
						{
							// Level新規作成
							m_level = std::make_shared<Level>();
							// デシリアライズ
							m_level->Deserialize(jsn);
							// ファイルパス記憶
							m_level->SetFilepath(filepath);
						}
					}
				}
				// Level保存
				if (ImGui::Selectable(u8"名前を付けてLevel保存"))
				{
					std::string filepath = m_level->GetFilepath();
					if (KdWindow::SaveFileDialog(filepath, "Levelファイル保存", "Levelファイル\0*.level\0"))
					{
						// ファイルパス記憶
						m_level->SetFilepath(filepath);
						// シリアライズ
						json11::Json::object serialLevel;
						m_level->Serialize(serialLevel);
						// JSONを文字列化し、ファイルへ保存
						json11::Json jsn = serialLevel;
						std::string strJson = jsn.dump(true);
						// ファイル書き込み
						std::ofstream ofs(filepath);
						if (ofs)
						{
							ofs.write(strJson.c_str(), strJson.size());
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// ルートからの階層表示
		m_level->Editor_ImGuiupdate();
	}
	ImGui::End();


	//===============================================
	// Inspectorウィンドウ
	//===============================================
	if (ImGui::Begin("Inspector")) {

		// 選択物
		KdSptr<GameObject> selectObj = m_editorData.SelectObj.lock();
		if (selectObj) {
			// GameObjectのImGui処理を実行
			selectObj->Editor_ImGuiUpdate();
		}

	}
	ImGui::End();

	//===============================================
	// ログウィンドウ
	//===============================================
	m_editorData.LogWindow.ImGuiUpdate("Log");


	//===============================================
	// CollisionManagerウィンドウ
	// ・動作設定やデバッグ表示・負荷表示などが出来る
	//===============================================
	if (ImGui::Begin("Collision")) {
		m_collisionMgr->ImGuiUpdate();
	}
	ImGui::End();


}

KdSptr<GameObject> GameSystem::Instantiate(const std::string& prefabFilename)
{
	// Prefabファイル読み込み
	json11::Json jsn = KDResFactory.GetJSON(prefabFilename);
	if (jsn.is_object()) {
		// GameObject生成
		KdSptr<GameObject> newObj = std::make_shared<GameObject>();
		// JSONデータを流し込む
		newObj->Deserialize(jsn);
		// とりあえずルートの子にする
		newObj->SetParent(m_level->GetRootObject());
		// 生成したGameObjectを返す
		return newObj;
	}

	// 失敗時はnullを返す
	return nullptr;
}
