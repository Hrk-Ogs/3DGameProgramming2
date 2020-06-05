#include "../GameSystem.h"

#include "ModelComponent.h"

void ModelComponent::Update()
{
	// 無効時はスキップ
	if (m_enable == false)return;

	// アニメーション進行
	m_anim.AdvanceTime(1.0f, m_nodeOL, nullptr);

	// 全ノードのTransformを計算する
	m_nodeOL.CalcNodeMatrices();
}

void ModelComponent::Draw()
{
	// 無効時はスキップ
	if (m_enable == false)return;

	// GPUのバッファに全ボーンノードの行列を書き込む
	m_nodeOL.WriteToBoneBuffer();

	// このシェーダを使用するように設定
	SHADER.m_standardShader.SetToDevice();
	// 描画
	SHADER.m_standardShader.Draw(
		&m_nodeOL,					// 全ノード情報
		m_model->GetMaterials(),	// 全マテリアル（材質）情報
		GetOwner()->Transform()		// オブジェクトの行列
	);

}

void ModelComponent::Editor_ImGuiUpdate()
{
	// 継承元のImGui処理を実行する
	BaseComponent::Editor_ImGuiUpdate();

	// モデル
	if (ImGuiSelectModel(m_model)) {
		if (m_model == nullptr)m_model = std::make_shared<KdModel>();
		m_nodeOL = m_model->GetNodeOutlinear();
	}

}
