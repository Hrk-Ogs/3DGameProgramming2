#include "../GameSystem.h"

#include "TransformComponent.h"

void TransformComponent::Update()
{
	// ローカル行列からワールド行列を計算
	LocalToWorld();

}

void TransformComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	// 座標
	ImGui::DragFloat3(u8"座標", &m_localMatrix._41, 0.01f);

	// 回転（オイラー角）
	KdVec3 angles = m_localMatrix.ComputeAngles() * KdToDegrees;
	if (ImGui::DragFloat3(u8"回転", &angles.x, 1)) {
		// 一度、拡大・座標に分解し、再制作する
		KdVec3 scale = m_localMatrix.GetScale();

		// 新しい行列
		KdQuaternion rotation = KdQuaternion().CreateRotationFromAngles(
			angles.x * KdToRadians, 
			angles.y * KdToRadians, 
			angles.z * KdToRadians);
		// 行列化
		KdVec3 pos = m_localMatrix.Translation();
		m_localMatrix.CreateFromSRT(scale, rotation, pos);
	}

	// 拡大
	KdVec3 scale = m_localMatrix.GetScale();
	if (ImGui::DragFloat3(u8"拡大", &scale.x, 0.001f)) {
		// 行列のX軸ベクトルを設定
		m_localMatrix.Right(m_localMatrix.Right().Normalize() * scale.x);
		// 行列のY軸ベクトルを設定
		m_localMatrix.Up(m_localMatrix.Up().Normalize() * scale.y);
		// 行列のZ軸ベクトルを設定
		m_localMatrix.Forward(m_localMatrix.Forward().Normalize() * scale.z);
	}

	// 単位行列にする
	if (ImGui::Button(u8"単位行列化")) {
		m_localMatrix.CreateIdentity();
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

	m_localMatrix.CreateFromSRT(scale, rotation, pos);

}

void TransformComponent::Serialize(json11::Json::object& outJsonObj) const
{
	BaseComponent::Serialize(outJsonObj);

	// 行列をSRT（拡大・回転・座標）へ分解する
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
	// 自分のローカル行列と親のワールド行列と合成する

	auto parent = GetOwner()->GetParent();
	if (parent) {
		// 1つ上の親のワールド行列と合成
		m_worldMatrix = m_localMatrix * parent->Transform()->m_worldMatrix;
	}
	else {
		// 親がいないときは、ローカル行列をそのまま使用する
		m_worldMatrix = m_localMatrix;
	}
}

void TransformComponent::WorldToLocal()
{
	// 親とワールド行列と自分のワールド行列から、自分のローカル行列を逆算する

	auto parent = GetOwner()->GetParent();
	if (parent) {
		// 親のワールド行列の逆行列
		auto mParentInvWorld = parent->Transform()->m_worldMatrix;
		mParentInvWorld.Inverse();
		// ローカル行列を求める
		m_localMatrix = m_worldMatrix * mParentInvWorld;
	}
	else {
		// 親がいない場合はローカル行列をそのまま使用
		m_localMatrix = m_worldMatrix;
	}
}
