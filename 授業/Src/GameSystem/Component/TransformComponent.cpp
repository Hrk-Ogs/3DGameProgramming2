#include "../GameSystem.h"

#include "TransformComponent.h"

void TransformComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	// ���W
	ImGui::DragFloat3(u8"���W", &m_worldMatrix._41, 0.01f);

	// ��]�i�I�C���[�n�j
	KdVec3 angles = m_worldMatrix.ComputeAngles() * KdToDegrees;
	if (ImGui::DragFloat3(u8"��]", &angles.x, 1)) {
		// ��x�A�g��E���W�ɕ������A�Đ��삷��
		KdVec3 scale = m_worldMatrix.GetScale();
		KdQuaternion rotation = KdQuaternion().CreateRotationFromAngles(angles.x * KdToRadians, angles.y * KdToRadians, angles.z * KdToRadians);
		KdVec3 pos = m_worldMatrix.Translation();
		m_worldMatrix.CreateFromSRT(scale, rotation, pos);
	}

	// �g��
	KdVec3 scale = m_worldMatrix.GetScale();
	if (ImGui::DragFloat3(u8"�g��", &scale.x, 0.001f)) {
		// �s���X���x�N�g����ݒ�
		m_worldMatrix.CreateRotationAxis.Right(m_worldMatrix.Right().Normalize() * scale.x);
		// �s���Y���x�N�g����ݒ�
		m_worldMatrix.Up(m_worldMatrix.Up().Normalize * scale.y);
		// �s���Z���x�N�g����ݒ�
		m_worldMatrix.Forward(m_worldMatrix.Forward().Normalize() * scale.z);
	}

	// �P�ʍs��ɂ���
	if (ImGui::Button(u8"�P�ʍs��")) {
		m_worldMatrix.CreateIdentity();
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
	m_worldMatrix.CreateFromSRT(scale, rotation, pos);

}

void TransformComponent::Serialize(json11::Json::object& outJsonObj) const
{
	BaseComponent::Serialize(outJsonObj);

	// �s���SRT�i�g��E��]�E���W�j�֕�������
	KdVec3 scale;
	KdQuaternion rotation;
	KdVec3 pos;
	m_worldMatrix.Decompose(scale, rotation, pos);
	outJsonObj["Position"] = pos.ToArray();
	outJsonObj["Rotation"] = rotation.ToArray();
	outJsonObj["Scale"] = scale.ToArray();
}
