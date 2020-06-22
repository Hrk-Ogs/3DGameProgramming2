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


	// Level作成
	m_level = std::make_shared<Level>();
}

void GameSystem::Run()
{

	// デバッグ線を全てクリアする
	m_debugLineDraw.ResetLines();

	//==========================
	// ImGui開始
	//==========================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui Demo ウィンドウ表示 ※すごく参考になるウィンドウです。imgui_demo.cpp参照。
	ImGui::ShowDemoWindow(nullptr);



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

	// 収集したGameObjectの更新処理
	for (auto&& gameObj : m_tempGameObjects) {
		// 全コンポーネント実行（直接コンポーネントリストを取得し、実行していく）
		for (auto&& comp : gameObj->GetComponentList()) {
			comp->CallStartOnce();
			comp->Update();
		}
	}

	// エディターカメラ処理
	if (m_editorData.FreeCameraMode) {
		// どのウィンドウの上にもマウスがいないときのみ
		if (ImGui::IsAnyWindowHovered() == false) {
			m_editorData.Camera.Update();
		}
	}


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
	// カメラをシェーダーにセット
	if (m_tempRenderingDate.m_camera && m_editorData.FreeCameraMode == false) {	// カメラコンポーネントがある時
		// カメラコンポーネントのカメラ情報をセットする
		m_tempRenderingDate.m_camera->SetCamera();
	}
	else {	// ないとき
		// Editorカメラセット
		m_editorData.Camera.SetToShader();
	}


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

}

// ImGui系の処理
void GameSystem::Editor_ImGuiUpdate()
{
	//===============================================
	// Levelウィンドウ
	//===============================================
	if (ImGui::Begin("Level", 0, ImGuiWindowFlags_MenuBar)) {

		//---------------------
		// エディター設定
		//---------------------
		ImGui::Checkbox("FreeCamer Mode", &m_editorData.FreeCameraMode);

		ImGui::Separator();

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


}
