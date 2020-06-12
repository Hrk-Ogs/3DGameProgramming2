#include "GameSystem.h"

void GameSystem::Init()
{
	m_editorData.LogWindow.AddLog(u8"GameSystem�N��...");

	// �V�X�e���R���|�[�l���g�o�^
	ComponentClassMaker::GetInstance().Register<TransformComponent>();
	ComponentClassMaker::GetInstance().Register<ModelComponent>();

	// Level�쐬
	m_level = std::make_shared<Level>();
}

void GameSystem::Run()
{
	//==========================
	// ImGui�J�n
	//==========================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui Demo �E�B���h�E�\�� ���������Q�l�ɂȂ�E�B���h�E�ł��Bimgui_demo.cpp�Q�ƁB
	ImGui::ShowDemoWindow(nullptr);


	//==========================
	//
	// �X�V����
	//
	//==========================

	// Level�̍X�V����
	m_level->Update();

	// �G�f�B�^�[�J��������
	m_editorData.Camera.Update();
	

	//==========================
	//
	// �`�揈��
	//
	//==========================

	//-----------
	// �J����
	//-----------
	// �J�������V�F�[�_�[�ɃZ�b�g
	m_editorData.Camera.SetToShader();

	//-----------
	// ���C�g
	//-----------
	// ���s��
	SHADER.m_cb8_Light.Work().DL[0].Dir = { 0, -1, 0 };	// ����
	SHADER.m_cb8_Light.Work().DL[0].Color = { 3,3,3 };	// �F
	SHADER.m_cb8_Light.Work().DL_Cnt = 1;	// �g�p��
	// ���C�g�f�[�^��GPU�o�b�t�@�ɏ�������
	SHADER.m_cb8_Light.Write();


	//-----------
	// �`��
	//-----------
	// Level�̕`�揈��
	m_level->Draw();


	//===========================
	// GUI����
	//===========================
	Editor_ImGuiUpdate();


	//===========================
	// ImGui�`��
	//===========================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

// ImGui�n�̏���
void GameSystem::Editor_ImGuiUpdate()
{
	//===============================================
	// Level�E�B���h�E
	//===============================================
	if (ImGui::Begin("Level Window")) {
		// ���[�g����̊K�w�\��
		m_level->Editor_ImGuiupdate();
	}
	ImGui::End();


	//===============================================
	// Inspector�E�B���h�E
	//===============================================
	if (ImGui::Begin("Inspector")) {

		// �I��
		KdSptr<GameObject> selectObj = m_editorData.SelectObj.lock();
		if (selectObj) {
			// GameObject��ImGui���������s
			selectObj->Editor_ImGuiUpdate();
		}

	}
	ImGui::End();


	//===============================================
	// ���O�E�B���h�E
	//===============================================
	m_editorData.LogWindow.ImGuiUpdate("Log");


}
