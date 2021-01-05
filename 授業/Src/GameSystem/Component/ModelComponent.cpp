#include "../GameSystem.h"

#include "ModelComponent.h"

void ModelComponent::SetModel(const KdSptr<KdModel>& model)
{
	// ���f���f�[�^���Z�b�g
	m_model = model;

	// ���f�����Ȃ��Ƃ���
	if (m_model == nullptr) {
		m_model = std::make_shared<KdModel>();
	}

	// ���f���f�[�^����m�[�h�A�E�g���C�i�[�𕡐�
	m_nodeOL = m_model->GetNodeOutlinear();

	// �A�j���[�^�[�ݒ�
	m_animator.Release();
	// �����A�j���[�V����
	m_animator.SetAnimation(m_model->GetAnimation(m_startAnimeName), m_startAnimationLoop);
}

void ModelComponent::Update()
{
	// �������̓X�L�b�v
	if (m_enable == false)return;

	// �A�j���[�V�����i�s
	if (GAMESYS.IsPlay()) {
		
		// Root���[�V�����L����
		if (m_enableRootMotion) {
			auto trans = GetOwner()->Transform();
			// ���݂̍s����g��E��]�E���W�֕���
			KdVec3 scale = trans->GetLocalScale();
			KdQuaternion rotate = trans->GetlocalRotation();
			KdVec3 pos = trans->GetLocalPosition();

			// �A�j���[�V������i�߂�
			// ��Root�{�[���̕ω��ʂ�scale,rotate,pos�ɓK�p����
			m_animator.AdvanceTime(m_animationSpeed, m_nodeOL, m_onAnimeScriptProc, &scale, &rotate, &pos);

			// �ω����scale,rotate,pos���Z�b�g
			trans->SetLocalScale(scale);
			trans->SetLocalRotation(rotate);
			trans->SetLocalPosition(pos);
		}
		else {
			// �A�j���[�V������i�߂�
			m_animator.AdvanceTime(m_animationSpeed, m_nodeOL, m_onAnimeScriptProc);
		}	
	}


	// �S�m�[�h�̍s��v�Z
	m_nodeOL.CalcNodeMatrices();
}

void ModelComponent::PrepareDraw(RenderingData& rdate)
{
	// �������̓X�L�b�v
	if (m_enable == false)return;
	// ��\�����̓X�L�b�v
	if (m_visible == false)return;
	// ���f�����Ȃ��Ƃ��̓X�L�b�v
	if (m_model->IsValid() == false)return;

	// �`��o�^
	if (m_mulMaterialColor.w >= 1.0f)
	{
		// �u3D�s�����`����W���X�g�v�֓o�^
		rdate.m_drawList.push_back(this);
	}
	else 
	{
		// �u3D�������`����W���X�g�v�֓o�^
		rdate.m_drawTransparentList.push_back(this);
	}
}

void ModelComponent::Draw(int phaseID)
{
	// �������̓X�L�b�v
	if (m_enable == false)return;
	if (m_visible == false)return;

	// GPU�̃o�b�t�@�ɑS�{�[���m�[�h�̍s�����������
	m_nodeOL.WriteToBoneBuffer();

	// TransformComponent���擾
	auto trans = GetOwner()->GetComponent<TransformComponent>();

	// ���C�gOn/Off
	SHADER.m_modelShader.SetLightEnable(m_lightEnable);
	// �t�H�OOn/Off
	SHADER.m_modelShader.SetFogEnable(m_fogEnable);
	// ��Z�F
	SHADER.m_modelShader.SetMulMaterialColor(m_mulMaterialColor);
	// UV�ݒ�
	SHADER.m_modelShader.SetUVOffset(m_uvOffset);
	SHADER.m_modelShader.SetUVTiling(m_uvTiling);
	// ���ܗ��ݒ�
	SHADER.m_modelShader.SetRefractiveIndex(m_refractiveIndex);

	// �g�\��
	if (m_useWaveNormalMap)
	{
		for (auto&& mate : m_model->GetMaterials())
		{
			// �����I�Ɏg�p����@���}�b�v�Ƃ��ăZ�b�g
			mate->ForceUseNormalTex = GAMESYS.GetWaveNormalMap();
		}
	}

	// �`��ݒ��ύX����
	SHADER.m_modelShader.SetToDevice();
	SHADER.m_modelShader.Draw(&m_nodeOL,m_model->GetMaterials(),trans->GetMatrix());

	// �g�\��
	if (m_useWaveNormalMap)
	{
		for (auto&& mate : m_model->GetMaterials())
		{
			// �����I�Ɏg�p����@���}�b�v������
			mate->ForceUseNormalTex = nullptr;
		}
	}

	//------------------------
	// �֊s�`��
	//------------------------
	if (m_enableOutline)
	{
		// �\�ʂ��J�����O(��\��)�ɂ��郉�X�^���C�U�X�e�[�g���Z�b�g
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullFront);

		// �`��
		SHADER.m_modelShader.SetToDevice_Outline();
		SHADER.m_modelShader.Draw_Outline(&m_nodeOL, trans->GetMatrix());

		// ���ʂ��J�����O(��\��)�ɂ��郉�X�^���C�U�X�e�[�g�ɖ߂�
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);
	}
}

void ModelComponent::Editor_ImGuiUpdate()
{
	// �p������ImGui���������s����
	BaseComponent::Editor_ImGuiUpdate();

	// ���f��
	if (ImGuiSelectModel(m_model)) {
		SetModel(m_model);
	}
	
	// �`��ݒ�
	if (ImGui::TreeNodeEx(u8"�`��ݒ�", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox(u8"�\��", &m_visible);
		ImGui::Checkbox(u8"���C�g�L��", &m_lightEnable);
		ImGui::Checkbox(u8"�t�H�O�L��", &m_fogEnable);
		ImGui::Checkbox(u8"�֊s�L��", &m_enableOutline);
		ImGui::Checkbox(u8"�g�\��", &m_useWaveNormalMap);
		ImGui::ColorEdit4(u8"Material��Z�F", &m_mulMaterialColor.x);
		ImGui::DragFloat(u8"���ܗ�", &m_refractiveIndex, 0.01f);

		ImGui::DragFloat2("UVOffset", &m_uvOffset.x, 0.001f);
		ImGui::DragFloat2("UVTiling", &m_uvTiling.x, 0.001f);

		ImGui::TreePop();
	}

	// �A�j���[�V����
	if (ImGui::TreeNodeEx("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		// ���f����ǂݍ���ł��鎞
		if (m_model->IsValid()) {

			ImGui::Checkbox(u8"RootMotion�L��", &m_enableRootMotion);

			if (ImGui::TreeNodeEx(u8"�����A�j���[�V����", ImGuiTreeNodeFlags_DefaultOpen)) {
				// �p�����[�^���ύX���ꂽ�H
				int change = 0;

				// �Đ����x
				ImGui::DragFloat("Speed", &m_animationSpeed, 0.01f);

				// ���[�v�Đ�
				change += ImGui::Checkbox("Loop", &m_startAnimationLoop);

				// �A�j���[�V�����w��
				change += ImGui::InputText(u8"�Đ��A�j����", &m_startAnimeName);
				if (ImGui::BeginPopupContextItem()) {
					// KdModel���̃A�j���[�V�������X�g���Q�Ƃ��A�I���o����悤�ɂ���
					for (auto&& n : m_model->GetAnimations()) {
						if (ImGui::Selectable(n->GetName().c_str())) {
							// �I��������A���̃A�j���[�V�����������ڂ���
							m_startAnimeName = n->GetName();
							change = true;
						}
					}
					ImGui::End();
				}
				// �A�j���[�V�������w�肳�ꂽ�Ƃ��́A�A�j���[�V������ύX����
				if (change >= 1) {
					m_animator.SetAnimation(m_model->GetAnimation(m_startAnimeName), m_startAnimationLoop);
				}

				// ���̃{�^���̏�Ƀ}�E�X��u�����������Đ�����
				ImGui::ButtonEx(u8"�}�E�X�I�[�o�[�ōĐ�", ImVec2(0, 0), ImGuiButtonFlags_Disabled);
				if (ImGui::IsItemHovered()) {
					// �A�j���i�s
					m_animator.AdvanceTime(m_animationSpeed, m_nodeOL, nullptr);
				}
				// ����ȊO�̎��̓f�t�H���g�{�[�Y�ɂ���
				else
				{
					for (UINT ni = 0; ni < m_nodeOL.GetAllNodes().size(); ni++)
					{
						m_nodeOL.GetAllNodes()[ni].LocalTransform =
							m_model->GetNodeOutlinear().GetAllNodes()[ni].LocalTransform;
					}
				}

				//
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

}
