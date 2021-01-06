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

	// Level�쐬
	m_level = std::make_shared<Level>();

	// �����[�X��
#ifndef DEVELOPMENT_MODE
	// ���s���[�h
	m_isPlay = true;
	// �R���W�����̃f�o�b�O���b����OFF
	ColliderComponent::s_showDebugLine = false;
	// �������x��
	ChangeLevel("Data\\Level\\Title.level");
#endif

	// ���ʗp�e�N�X�`��
	m_texOpaqueWork = std::make_shared<KdTexture>();
	m_texOpaqueWork->CreateRenderTarget(D3D.GetBackBuffer()->GetWidth(), D3D.GetBackBuffer()->GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_texWork = std::make_shared<KdTexture>();
	m_texWork->CreateRenderTarget(D3D.GetBackBuffer()->GetWidth(), D3D.GetBackBuffer()->GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT);
	// ���P�x�p�e�N�X�`��
	m_texHighBrightness = std::make_shared<KdTexture>();
	m_texHighBrightness->CreateRenderTarget(D3D.GetBackBuffer()->GetWidth(), D3D.GetBackBuffer()->GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT);

	// �u���[�e�N�X�`���쐬
	m_texBlur.Create(D3D.GetBackBuffer()->GetWidth(), D3D.GetBackBuffer()->GetHeight());

	// �g�e�N�X�`���쐬
	m_texWave.Create(512, 512);
	// �����ł�����x�g��i�s�����Ă���
	for (int i = 0; i < 1000; i++)
	{
		SHADER.m_postProcessShader.AdvanceWave(m_texWave);
	}
}

void GameSystem::Run()
{

	// �f�o�b�O����S�ăN���A����
	m_debugLineDraw.ResetLines();

	//==========================
	// ���x���̐؂�ւ�����
	//==========================
	if (m_nextLevelFilename.empty() == false) {
		// Level�쐬
		m_level = std::make_shared<Level>();

		// ���\�[�X�Ǘ��N���X�œK��
		// ���ǂ�������Q�Ƃ���Ă��Ȃ��s�K�v�ȃ��\�[�X�݂̂��������
		int cnt = KDResFactory.Optimize();
		m_editorData.LogWindow.AddLog(u8"%d�̃��\�[�X�����", cnt);

		// JSON�ǂݍ��݁�Level�̃f�V���A���C�Y
		json11::Json jsn = KdLoadJSONFile(m_nextLevelFilename);
		if (jsn.is_null() == false) {
			// �f�V���A���C�Y
			m_level->Deserialize(jsn);

			// �t�@�C���p�X�L��
			m_level->SetFilepath(m_nextLevelFilename);
		}
		// �\����N���A
		m_nextLevelFilename = "";
	}

#ifdef DEVELOPMENT_MODE
	//==========================
	// ImGui�J�n
	//==========================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ImGui Demo �E�B���h�E�\�� ���������Q�l�ɂȂ�E�B���h�E�ł��Bimgui_demo.cpp�Q�ƁB
	ImGui::ShowDemoWindow(nullptr);

	// ImGuizmo�J�n
	ImGuizmo::BeginFrame();
#endif

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

	// ���̃R���W�����Ǘ��N���X���g�p����悤�ɃZ�b�g
	m_collisionMgr->SetManagerToDefault();

	// �����̑O�ɁA�STransformComponent�ɑ΂��čs����L�������Ă���
	for (auto&& gameObj : m_tempGameObjects) {
		gameObj->Transform()->BackupMatrix();
	}

	// ���W����GameObject�̍X�V����
	for (auto&& gameObj : m_tempGameObjects) {
		// �S�R���|�[�l���g���s�i���ڃR���|�[�l���g���X�g���擾���A���s���Ă����j
		for (auto&& comp : gameObj->GetComponentList()) {
			// Start���s
			if (m_isPlay) {
				comp->CallStartOnce();
			}
			comp->Update();
		}
	}

	// ���W����GameObject�̍X�V����
	for (auto&& gameObj : m_tempGameObjects) {
		// �S�R���|�[�l���g���s
		for (auto&& comp : gameObj->GetComponentList()) {
			// �X�V����
			comp->LateUpdate();
		}
	}


	// �u��~�v���́A�R���C�_�[���̍X�V�̂ݎ��s����
	// �u���s�v���́A�R���C�_�[���̍X�V�Ƃ����蔻������s
	bool onlyUpdateCollider = (m_isPlay == false);
	// �R���W���������A���ʒʒm
	m_collisionMgr->Run(onlyUpdateCollider);

	// �g
	SHADER.m_postProcessShader.AdvanceWave(m_texWave);

#ifdef DEVELOPMENT_MODE
	// �G�f�B�^�[�J��������
	if (m_isPlay == false || m_editorData.FreeCameraMode) {
		// �ǂ̃E�B���h�E�̏�ɂ��}�E�X�����Ȃ��Ƃ��̂�
		if (ImGui::IsAnyWindowHovered() == false) {
			m_editorData.Camera.Update();
		}
	}
#endif

	//-----------------------
	// �`�揀��
	//-----------------------
	// �����_�����O�f�[�^�����Z�b�g
	m_tempRenderingDate.Reset();

	// ���C�g�����Z�b�g
	SHADER.m_cb8_Light.Work().DL_Cnt = 0;
	SHADER.m_cb8_Light.Work().PL_Cnt = 0;
	SHADER.m_cb8_Light.Work().SL_Cnt = 0;


	// GameOvject�`�揀������
	for (auto&& gameObj : m_tempGameObjects) {
		// �S�R���|�[�l���g���s
		for (auto&& comp : gameObj->GetComponentList()) {
			comp->PrepareDraw(m_tempRenderingDate);
		}
	}

	// ���C�g�f�[�^�������݁iGPU�ɓ]������`��i�V�F�[�_�[�j�Ŏg�p�����j
	SHADER.m_cb8_Light.Write();


	//==========================
	//
	// �`��
	//
	//==========================
	//-----------
	// �J����
	//-----------
#ifdef DEVELOPMENT_MODE
	// �J�������V�F�[�_�[�ɃZ�b�g
	if (m_tempRenderingDate.m_camera && m_editorData.FreeCameraMode == false && m_isPlay) {	// �J�����R���|�[�l���g�����鎞
		// �J�����R���|�[�l���g�̃J���������Z�b�g����
		m_tempRenderingDate.m_camera->SetCamera();
	}
	else {	// �Ȃ��Ƃ�
		// Editor�J�����Z�b�g
		m_editorData.Camera.SetToShader();
	}
#else
	if (m_tempRenderingDate.m_camera) {
		// �J�����R���|�[�l���g�̃J���������Z�b�g����
		m_tempRenderingDate.m_camera->SetCamera();
	}
#endif

	//------------------
	// �V���h�E�}�b�v�����`��
	//------------------
	for (auto&& lightComp : m_tempRenderingDate.m_shadowMapLight)
	{
		auto drawProc = [this]()
		{
			for (auto&& comp : m_tempRenderingDate.m_drawShadowList)
			{
				comp->Draw(RenderingData::kGenShadowMap);
			}
		};

		// �V���h�E�}�b�v�����`��
		lightComp->GenerateShadowMap(drawProc);
	}
	// �V���h�E�}�b�v���Z�b�g
	D3D.GetDevContext()->PSSetShaderResources(13, 1, SHADER.m_genShadowMapShader.GetSpotShadowMap()->GetSRViewAddress());
	D3D.GetDevContext()->PSSetShaderResources(14, 1, SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRViewAddress());

	//-----------
	// ���C�g
	//-----------


	//-----------
	// �`��
	//-----------

	//+++++++++++++++++++
	// �s���������ʗpRT�֕ύX
	//+++++++++++++++++++	
// ���ʗpRT�̃N���A
	D3D.GetDevContext()->ClearRenderTargetView(m_texOpaqueWork->GetRTView(), KdVec4(0, 0, 0, 1));
	// RT�̕ύX
	D3D.GetDevContext()->OMSetRenderTargets(1, m_texOpaqueWork->GetRTViewAddress(), D3D.GetZBuffer()->GetDSView());
	// �r���[�|�[�g�̐ݒ�
	D3D.SetViewportFromRTV(m_texOpaqueWork->GetRTView());

	//++++++++++++++++++++
	// 3D�`��i�s�����j
	//++++++++++++++++++++
	// �������ŁAm_tempRenderingData.m_drawList���J��������߂����ԂɃ\�[�g����΃p�t�H�[�}���X�A�b�v

	for (auto&& comp : m_tempRenderingDate.m_drawList) {
		comp->Draw(RenderingData::kOpaquePhase);
	}

	//+++++++++++++++++++
	// ���ʗpRT�֕ύX
	//+++++++++++++++++++
	// OPaqueWork -> Work�փR�s�[
	D3D.GetDevContext()->CopyResource(m_texWork->GetResource(), m_texOpaqueWork->GetResource());
	// RT�̕ύX
	D3D.GetDevContext()->OMSetRenderTargets(1, m_texWork->GetRTViewAddress(), D3D.GetZBuffer()->GetDSView());
	// �r���[�|�[�g�̐ݒ�
	D3D.SetViewportFromRTV(m_texWork->GetRTView());
	// �s���������ʗpRT��SRV�Ƃ��ăZ�b�g
	D3D.GetDevContext()->PSSetShaderResources(10, 1, m_texOpaqueWork->GetSRViewAddress());

	//++++++++++++++++++++
	// 3D�`��i�������j
	//++++++++++++++++++++

	for (auto&& comp : m_tempRenderingDate.m_drawTransparentList) {
		comp->Draw(RenderingData::kTransparentPhase);
	}

	// �Z�b�g���Ă����s���������ʗpRT������
	D3D.GetDevContext()->PSSetShaderResources(10, 1, KdTexture().GetSRViewAddress());

	// �V���h�E�}�b�v������
	D3D.GetDevContext()->PSSetShaderResources(13, 1, KdTexture().GetSRViewAddress());
	D3D.GetDevContext()->PSSetShaderResources(14, 1, KdTexture().GetSRViewAddress());


	//++++++++++++++++++++
	// 3D�G�t�F�N�g�`��i�������j
	//++++++++++++++++++++
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);	 // Z�o�b�t�@�̏������݂�OFF
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);	// �J�����O�𖳂��i��ʕ`��j
	for (auto&& comp : m_tempRenderingDate.m_drawEffectList) {
		comp->Draw(RenderingData::kEffectPhase);
	}
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);	 // Zbaffa no�������݂�ON
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);

	//+++++++++++++++++++
	// �|�X�g�v���Z�X
	//+++++++++++++++++++
	// ���P�x���o
	SHADER.m_postProcessShader.BrightFiltering(m_texHighBrightness.get(), m_texWork.get());

	// MGF
	SHADER.m_postProcessShader.GenerateBlur(m_texBlur, m_texHighBrightness.get());

	// ���C�g�u���[���\��
	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Add, KdVec4(0, 0, 0, 0), 0xFFFFFFFF);
	for (int i = 0; i < 5; i++)
	{
		SHADER.m_postProcessShader.ColorDraw(m_texBlur.m_rt[i][0].get(), { 0.2f,0.2f,0.2f, 1 });
	}
	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, KdVec4(0, 0, 0, 0), 0xFFFFFFFF);

	//+++++++++++++++++++
	// RT���o�b�N�o�b�t�@�֖߂�
	//+++++++++++++++++++
	D3D.GetDevContext()->OMSetRenderTargets(1, D3D.GetBackBuffer()->GetRTViewAddress(), D3D.GetZBuffer()->GetDSView());
	D3D.SetViewportFromRTV(D3D.GetBackBuffer()->GetRTView());

	// ���ʉ摜���o�b�N�o�b�t�@�֕`��
	//SHADER.m_postProcessShader.ColorDraw(m_texWork.get(), { 1,1,1,1 });
	SHADER.m_postProcessShader.ToneMappingDraw(m_texWork.get());

	//++++++++++++++++++++
	// 2D�`��
	//++++++++++++++++++++
	SHADER.m_spriteShader.Begin(true, false);
	for (auto&& comp : m_tempRenderingDate.m_drawSpriteList) {
		comp->Draw(RenderingData::kSpritePhase);
	}
	SHADER.m_spriteShader.End();

#ifdef DEVELOPMENT_MODE
	//===========================
	// GUI����
	//===========================
	Editor_ImGuiUpdate();

	//-------------------
	// �f�o�b�O���`��
	//-------------------
	m_debugLineDraw.Draw();

	//===========================
	// ImGui�`��
	//===========================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}

// ImGui�n�̏���
void GameSystem::Editor_ImGuiUpdate()
{
	//===============================================
	// GameSystem
	//===============================================
	if (ImGui::Begin("GameSystem")) {

		//---------------------
		// �G�f�B�^�[�ݒ�
		//---------------------

		// ���s/��~
		if (ImGui::Checkbox(u8"���s", &m_isPlay)) {
			// ���s�OLevel���V���A���C�Y�������̒u����
			static json11::Json::object sSerialLevel;

			// ���s
			if (m_isPlay) {
				// ����Level�̏�Ԃ��V���A���C�Y���Ă���
				m_level->Serialize(sSerialLevel);

				// �ǂݍ��݂Ȃǂ��Č����邽�߁A��x���\�[�X�֌W��S���
				// �i����͂��΂₭���s�������̂ŁA�����̓R�����g�ɂ���j
				//m_editorData.LogWindow.AddLog(u8"���\�[�X�S���");
				//KDResFactory.Clear():

				// �f�V���A���C�Y�ŏ�����
				m_level->Deserialize(sSerialLevel);
			}
			// ��~
			else {
				// �ǂݍ��݂Ȃǂ��Č����邽�߁A��x���\�[�X��S���
				//m_editorData.LogWindow.AddLog(u8"���\�[�X��S���");
				//KDResFactory.Clear();

				// �Đ��O�̏�Ԃ֖߂�
				m_level->Deserialize(sSerialLevel);
			}
		}

		// ���s���̂�CheckBox��\������
		if (m_isPlay) {
			ImGui::Indent();
			ImGui::Checkbox("FreeCamer Mode", &m_editorData.FreeCameraMode);
			ImGui::Unindent();
		}
		ImGui::Separator();




		//------------------
		// ImGuizmo�̑���ݒ�
		//------------------
		if (ImGui::CollapsingHeader("Gizmo", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::RadioButton("Local", m_editorData.GizmoMode == ImGuizmo::LOCAL)) {
				m_editorData.GizmoMode = ImGuizmo::LOCAL;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("World", m_editorData.GizmoMode == ImGuizmo::WORLD)) {
				m_editorData.GizmoMode = ImGuizmo::WORLD;
			}
			if (ImGui::RadioButton(u8"�ړ�", m_editorData.GizmoOperation == ImGuizmo::TRANSLATE)) {
				m_editorData.GizmoOperation = ImGuizmo::TRANSLATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"��]", m_editorData.GizmoOperation == ImGuizmo::ROTATE)) {
				m_editorData.GizmoOperation = ImGuizmo::ROTATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"�g��", m_editorData.GizmoOperation == ImGuizmo::SCALE)) {
				m_editorData.GizmoOperation = ImGuizmo::SCALE;
			}
		}
	}
	ImGui::End();


	//===============================================
	// Level�E�B���h�E
	//===============================================
	if (ImGui::Begin("Level", 0, ImGuiWindowFlags_MenuBar)) {

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


	//===============================================
	// CollisionManager�E�B���h�E
	// �E����ݒ��f�o�b�O�\���E���ו\���Ȃǂ��o����
	//===============================================
	if (ImGui::Begin("Collision")) {
		m_collisionMgr->ImGuiUpdate();
	}
	ImGui::End();

	//===========================
	// �O���t�B�b�N
	//===========================
	if (ImGui::Begin("GraphicsDebug"))
	{
		if (ImGui::CollapsingHeader(u8"�g�}�b�v", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Image(m_texWave.m_rtHeight[0]->GetSRView(), ImVec2(100 * m_texWave.m_rtHeight[0] ->GetAspectRatio(), 100));
			ImGui::SameLine();
			ImGui::Image(m_texWave.m_rtNormal->GetSRView(), ImVec2(100 * m_texWave.m_rtNormal->GetAspectRatio(), 100));
		}
	}
	ImGui::End();

}

KdSptr<GameObject> GameSystem::Instantiate(const std::string& prefabFilename)
{
	// Prefab�t�@�C���ǂݍ���
	json11::Json jsn = KDResFactory.GetJSON(prefabFilename);
	if (jsn.is_object()) {
		// GameObject����
		KdSptr<GameObject> newObj = std::make_shared<GameObject>();
		// JSON�f�[�^�𗬂�����
		newObj->Deserialize(jsn);
		// �Ƃ肠�������[�g�̎q�ɂ���
		newObj->SetParent(m_level->GetRootObject());
		// ��������GameObject��Ԃ�
		return newObj;
	}

	// ���s����null��Ԃ�
	return nullptr;
}
