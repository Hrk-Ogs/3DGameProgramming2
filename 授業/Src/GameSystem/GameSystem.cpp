#include "GameSystem.h"

void GameSystem::Init()
{
	m_editorData.LogWindow.AddLog(u8"GameSystem�N��...");

	// �V�X�e���R���|�[�l���g�o�^
	ComponentClassMaker::GetInstance().Register<TransformComponent>();
	ComponentClassMaker::GetInstance().Register<ModelComponent>();
	ComponentClassMaker::GetInstance().Register<InputComponent>();
	ComponentClassMaker::GetInstance().Register<PlayerInputComponent>();
	ComponentClassMaker::GetInstance().Register<SimpleAIInputComponent>();


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
	// GameObject�����W
	//==========================
	// ���WGameObject���X�g�̃��Z�b�g
	m_tempGameObjects.clear();

	// Level����GameObject�����W
	m_level->CollectGameObject(m_tempGameObjects);
	
	//==========================
	//
	// �X�V����
	//
	//==========================

	// Level�̍X�V����
	//m_level->Update();

	// ���W����GameObject�̍X�V����
	for (auto&& gameObj : m_tempGameObjects) {
		// �S�R���|�[�l���g���s�i���ڃR���|�[�l���g���X�g���擾���A���s���Ă����j
		for (auto&& comp : gameObj->GetComponentList()) {
			comp->Update();
		}
	}

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
	//m_level->Draw();

	// ���W����GameObject�̕`�揀������
	for (auto&& gameObj : m_tempGameObjects) {
		// �S�R���|�[�l���g���s�i���ڃR���|�[�l���g���X�g���擾���A���s���Ă���
		for (auto&& comp : gameObj->GetComponentList()) {
			comp->Draw();
		}
	}


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
	if (ImGui::Begin("Level",0,ImGuiWindowFlags_MenuBar)) {
		//-------------------
		// ���C�����j���[
		//-------------------
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// �V�KLevel
				if (ImGui::Selectable(u8"�V�KLevel"))
				{
					// ���\�[�X�t�@�N�g���[���N���A����
					KDResFactory.Clear();
					// Level�쐬
					m_level = std::make_shared<Level>();
				}
				// Level�ǂݍ���
				if (ImGui::Selectable(u8"Level�J��"))
				{
					std::string filepath = m_level->GetFilepath();
					if (KdWindow::OpenFileDialog(filepath, "Level�t�@�C���J��", "Level�t�@�C��\0*.level\0"))
					{
						// ���\�[�X�t�@�N�g���[���N���A����
						KDResFactory.Clear();
						// JSON�ǂݍ���
						json11::Json jsn = KdLoadJSONFile(filepath);
						if (jsn.is_null() == false)
						{
							// Level�V�K�쐬
							m_level = std::make_shared<Level>();
							// �f�V���A���C�Y
							m_level->Deserialize(jsn);
							// �t�@�C���p�X�L��
							m_level->SetFilepath(filepath);
						}
					}
				}
				// Level�ۑ�
				if (ImGui::Selectable(u8"���O��t����Level�ۑ�"))
				{
					std::string filepath = m_level->GetFilepath();
					if (KdWindow::SaveFileDialog(filepath, "Level�t�@�C���ۑ�", "Level�t�@�C��\0*.level\0"))
					{
						// �t�@�C���p�X�L��
						m_level->SetFilepath(filepath);
						// �V���A���C�Y
						json11::Json::object serialLevel;
						m_level->Serialize(serialLevel);
						// JSON�𕶎��񉻂��A�t�@�C���֕ۑ�
						json11::Json jsn = serialLevel;
						std::string strJson = jsn.dump(true);
						// �t�@�C����������
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
