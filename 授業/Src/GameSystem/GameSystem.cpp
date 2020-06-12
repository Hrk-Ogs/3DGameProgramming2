#include "GameSystem.h"

void GameSystem::Init()
{
	m_editorData.LogWindow.AddLog(u8"GameSystem起動...");

	// システムコンポーネント登録
	ComponentClassMaker::GetInstance().Register<TransformComponent>();
	ComponentClassMaker::GetInstance().Register<ModelComponent>();

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
	//
	// 更新処理
	//
	//==========================

	// Levelの更新処理
	m_level->Update();

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
	m_level->Draw();


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
	if (ImGui::Begin("Level Window")) {
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
