#include "../GameSystem.h"

#include "TransformComponent.h"

void TransformComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	// 座標
	ImGui::DragFloat3(u8"座標", &m_worldMatrix._41, 0.01f);

	// 回転（オイラー系）
	KdVec3 angles = m_worldMatrix.ComputeAngles() * KdToDegrees;
	if (ImGui::DragFloat3(u8"回転", &angles.x, 1)) {
		// 一度、拡大・座標に分解し、再制作する
		KdVec3 scale = m_worldMatrix.GetScale();
		KdQuaternion rotation = KdQuaternion().CreateRotationFromAngles(angles.x * KdToRadians, angles.y * KdToRadians, angles.z * KdToRadians);
		KdVec3 pos = m_worldMatrix.Translation();
		m_worldMatrix.CreateFromSRT(scale, rotation, pos);
	}

	// 拡大
	KdVec3 scale = m_worldMatrix.GetScale();
	if (ImGui::DragFloat3(u8"拡大", &scale.x, 0.001f)) {
		// 行列のX軸ベクトルを設定
		m_worldMatrix.CreateRotationAxis.Right(m_worldMatrix.Right().Normalize() * scale.x);
		// 行列のY軸ベクトルを設定
		m_worldMatrix.Up(m_worldMatrix.Up().Normalize * scale.y);
		// 行列のZ軸ベクトルを設定
		m_worldMatrix.Forward(m_worldMatrix.Forward().Normalize() * scale.z);
	}

	// 単位行列にする
	if (ImGui::Button(u8"単位行列化")) {
		m_worldMatrix.CreateIdentity();
	}

}

void TransformComponent::Deserialize(const json11::Json& jsonObj)
{
	BaseComponent::Deserialize(jsonObj);

	// 拡大・回転・座標
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

	// 行列をSRT（拡大・回転・座標）へ分解する
	KdVec3 scale;
	KdQuaternion rotation;
	KdVec3 pos;
	m_worldMatrix.Decompose(scale, rotation, pos);
	outJsonObj["Position"] = pos.ToArray();
	outJsonObj["Rotation"] = rotation.ToArray();
	outJsonObj["Scale"] = scale.ToArray();
}
