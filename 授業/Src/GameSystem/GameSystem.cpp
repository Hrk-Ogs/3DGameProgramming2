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


	// Level作成
	m_level = std::make_shared<Level>();
}

void GameSystem::Run()
{
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

	// Levelの更新処理
	//m_level->Update();

	// 収集したGameObjectの更新処理
	for (auto&& gameObj : m_tempGameObjects) {
		// 全コンポーネント実行（直接コンポーネントリストを取得し、実行していく）
		for (auto&& comp : gameObj->GetComponentList()) {
			comp->Update();
		}
	}

	// エディターカメラ処理
	m_editorData.Camera.Update();
	

	//==========================
	//
	// 描画処理
	//
	//==========================

	//-----------
	// カメラ
	//-----------
	// カメラをシェーダーにセット
	m_editorData.Camera.SetToShader();

	//-----------
	// ライト
	//-----------
	// 平行光
	SHADER.m_cb8_Light.Work().DL[0].Dir = { 0, -1, 0 };	// 方向
	SHADER.m_cb8_Light.Work().DL[0].Color = { 3,3,3 };	// 色
	SHADER.m_cb8_Light.Work().DL_Cnt = 1;	// 使用数
	// ライトデータをGPUバッファに書き込む
	SHADER.m_cb8_Light.Write();


	//-----------
	// 描画
	//-----------
	// Levelの描画処理
	//m_level->Draw();

	// 収集したGameObjectの描画準備処理
	for (auto&& gameObj : m_tempGameObjects) {
		// 全コンポーネント実行（直接コンポーネントリストを取得し、実行していく
		for (auto&& comp : gameObj->GetComponentList()) {
			comp->Draw();
		}
	}


	//===========================
	// GUI処理
	//===========================
	Editor_ImGuiUpdate();


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
	if (ImGui::Begin("Level",0,ImGuiWindowFlags_MenuBar)) {
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
