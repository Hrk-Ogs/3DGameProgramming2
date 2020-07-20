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

	// �u3D�s�����`����W���X�g�v�֓o�^
	rdate.m_drawList.push_back(this);
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

	// �`��ݒ��ύX����
	SHADER.m_standardShader.SetLightEnable(m_lightEnable);
	SHADER.m_standardShader.SetFogEnable(m_fogEnable);

	// �s����Z�b�g
	SHADER.m_standardShader.SetToDevice();
	// �`��
	SHADER.m_standardShader.Draw(
		&m_nodeOL,					// �S�m�[�h���
		m_model->GetMaterials(),	// �S�}�e���A���i�ގ��j���
		GetOwner()->Transform()->GetMatrix()		// �I�u�W�F�N�g�̍s��
	);
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
