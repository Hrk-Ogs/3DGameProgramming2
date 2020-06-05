#include "../GameSystem.h"

#include "ModelComponent.h"

void ModelComponent::Update()
{
	// �������̓X�L�b�v
	if (m_enable == false)return;

	// �A�j���[�V�����i�s
	m_anim.AdvanceTime(1.0f, m_nodeOL, nullptr);

	// �S�m�[�h��Transform���v�Z����
	m_nodeOL.CalcNodeMatrices();
}

void ModelComponent::Draw()
{
	// �������̓X�L�b�v
	if (m_enable == false)return;

	// GPU�̃o�b�t�@�ɑS�{�[���m�[�h�̍s�����������
	m_nodeOL.WriteToBoneBuffer();

	// ���̃V�F�[�_���g�p����悤�ɐݒ�
	SHADER.m_standardShader.SetToDevice();
	// �`��
	SHADER.m_standardShader.Draw(
		&m_nodeOL,					// �S�m�[�h���
		m_model->GetMaterials(),	// �S�}�e���A���i�ގ��j���
		GetOwner()->Transform()		// �I�u�W�F�N�g�̍s��
	);

}

void ModelComponent::Editor_ImGuiUpdate()
{
	// �p������ImGui���������s����
	BaseComponent::Editor_ImGuiUpdate();

	// ���f��
	if (ImGuiSelectModel(m_model)) {
		if (m_model == nullptr)m_model = std::make_shared<KdModel>();
		m_nodeOL = m_model->GetNodeOutlinear();
	}

}
