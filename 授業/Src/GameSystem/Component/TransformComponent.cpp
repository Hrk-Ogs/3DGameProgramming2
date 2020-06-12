#include "../GameSystem.h"

#include "TransformComponent.h"

void TransformComponent::Update()
{
	// ���[�J���s�񂩂烏�[���h�s����v�Z
	LocalToWorld();

}

void TransformComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	// ���W
	ImGui::DragFloat3(u8"���W", &m_localMatrix._41, 0.01f);

	// ��]�i�I�C���[�p�j
	KdVec3 angles = m_localMatrix.ComputeAngles() * KdToDegrees;
	if (ImGui::DragFloat3(u8"��]", &angles.x, 1)) {
		// ��x�A�g��E���W�ɕ������A�Đ��삷��
		KdVec3 scale = m_localMatrix.GetScale();

		// �V�����s��
		KdQuaternion rotation = KdQuaternion().CreateRotationFromAngles(
			angles.x * KdToRadians, 
			angles.y * KdToRadians, 
			angles.z * KdToRadians);
		// �s��
		KdVec3 pos = m_localMatrix.Translation();
		m_localMatrix.CreateFromSRT(scale, rotation, pos);
	}

	// �g��
	KdVec3 scale = m_localMatrix.GetScale();
	if (ImGui::DragFloat3(u8"�g��", &scale.x, 0.001f)) {
		// �s���X���x�N�g����ݒ�
		m_localMatrix.Right(m_localMatrix.Right().Normalize() * scale.x);
		// �s���Y���x�N�g����ݒ�
		m_localMatrix.Up(m_localMatrix.Up().Normalize() * scale.y);
		// �s���Z���x�N�g����ݒ�
		m_localMatrix.Forward(m_localMatrix.Forward().Normalize() * scale.z);
	}

	// �P�ʍs��ɂ���
	if (ImGui::Button(u8"�P�ʍs��")) {
		m_localMatrix.CreateIdentity();
	}

}

void TransformComponent::Deserialize(const json11::Json& jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	// �g��E��]�E���W
	KdVec3 scale;
	KdQuaternion rotation;
	KdVec3 pos;

	KdJsonGet(jsonObj["Position"], pos);
	KdJsonGet(jsonObj["Rotation"], rotation);
	KdJsonGet(jsonObj["Scale"], scale);

	m_localMatrix.CreateFromSRT(scale, rotation, pos);

}

void TransformComponent::Serialize(json11::Json::object& outJsonObj) const
{
	BaseComponent::Serialize(outJsonObj);

	// �s���SRT�i�g��E��]�E���W�j�֕�������
	KdVec3 scale;
	KdQuaternion rotation;
	KdVec3 pos;
	m_localMatrix.Decompose(scale, rotation, pos);

	outJsonObj["Position"] = pos.ToArray();
	outJsonObj["Rotation"] = rotation.ToArray();
	outJsonObj["Scale"] = scale.ToArray();
}

void TransformComponent::LocalToWorld()
{
	// �����̃��[�J���s��Ɛe�̃��[���h�s��ƍ�������

	auto parent = GetOwner()->GetParent();
	if (parent) {
		// 1��̐e�̃��[���h�s��ƍ���
		m_worldMatrix = m_localMatrix * parent->Transform()->m_worldMatrix;
	}
	else {
		// �e�����Ȃ��Ƃ��́A���[�J���s������̂܂܎g�p����
		m_worldMatrix = m_localMatrix;
	}
}

void TransformComponent::WorldToLocal()
{
	// �e�ƃ��[���h�s��Ǝ����̃��[���h�s�񂩂�A�����̃��[�J���s����t�Z����

	auto parent = GetOwner()->GetParent();
	if (parent) {
		// �e�̃��[���h�s��̋t�s��
		auto mParentInvWorld = parent->Transform()->m_worldMatrix;
		mParentInvWorld.Inverse();
		// ���[�J���s������߂�
		m_localMatrix = m_worldMatrix * mParentInvWorld;
	}
	else {
		// �e�����Ȃ��ꍇ�̓��[�J���s������̂܂܎g�p
		m_localMatrix = m_worldMatrix;
	}
}
