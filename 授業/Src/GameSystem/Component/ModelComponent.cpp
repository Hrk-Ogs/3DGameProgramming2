#include "../GameSystem.h"

#include "ModelComponent.h"

void ModelComponent::SetModel(const KdSptr<KdModel>& model)
{
	// モデルデータをセット
	m_model = model;

	// モデルがないときは
	if (m_model == nullptr) {
		m_model = std::make_shared<KdModel>();
	}

	// モデルデータからノードアウトライナーを複製
	m_nodeOL = m_model->GetNodeOutlinear();

	// アニメーター設定
	m_animator.Release();
	// 初期アニメーション
	m_animator.SetAnimation(m_model->GetAnimation(m_startAnimeName), m_startAnimationLoop);
}

void ModelComponent::Update()
{
	// 無効時はスキップ
	if (m_enable == false)return;

	// アニメーション進行
	if (GAMESYS.IsPlay()) {
		m_animator.AdvanceTime(m_animationSpeed, m_nodeOL, nullptr);
	}

	// 全ノードの行列計算
	m_nodeOL.CalcNodeMatrices();
}

void ModelComponent::PrepareDraw(RenderingData& rdate)
{
	// 無効時はスキップ
	if (m_enable == false)return;
	// 非表示時はスキップ
	if (m_visible == false)return;
	// モデルがないときはスキップ
	if (m_model->IsValid() == false)return;

	// 「3D不透明描画収集リスト」へ登録
	rdate.m_drawList.push_back(this);
}

void ModelComponent::Draw(int phaseID)
{
	// 無効時はスキップ
	if (m_enable == false)return;
	if (m_visible == false)return;

	// GPUのバッファに全ボーンノードの行列を書き込む
	m_nodeOL.WriteToBoneBuffer();

	// TransformComponentを取得
	auto trans = GetOwner()->Transform();

	// 行列をセット
	SHADER.m_standardShader.SetToDevice();
	// 描画
	SHADER.m_standardShader.Draw(
		&m_nodeOL,					// 全ノード情報
		m_model->GetMaterials(),	// 全マテリアル（材質）情報
		GetOwner()->Transform()->GetMatrix()		// オブジェクトの行列
	);
}

void ModelComponent::Editor_ImGuiUpdate()
{
	// 継承元のImGui処理を実行する
	BaseComponent::Editor_ImGuiUpdate();

	// モデル
	if (ImGuiSelectModel(m_model)) {
		SetModel(m_model);
	}
	
	// 描画設定
	if (ImGui::TreeNodeEx(u8"描画設定", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox(u8"表示", &m_visible);

		ImGui::TreePop();
	}

	// アニメーション
	if (ImGui::TreeNodeEx("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		// モデルを読み込んでいる時
		if (m_model->IsValid()) {
			if (ImGui::TreeNodeEx(u8"初期アニメーション", ImGuiTreeNodeFlags_DefaultOpen)) {
				// パラメータが変更された？
				int change = 0;

				// 再生速度
				ImGui::DragFloat("Speed", &m_animationSpeed, 0.01f);

				// ループ再生
				change += ImGui::Checkbox("Loop", &m_startAnimationLoop);

				// アニメーション指定
				change += ImGui::InputText(u8"再生アニメ名", &m_startAnimeName);
				if (ImGui::BeginPopupContextItem()) {
					// KdModel内のアニメーションリストを参照し、選択出来るようにする
					for (auto&& n : m_model->GetAnimations()) {
						if (ImGui::Selectable(n->GetName().c_str())) {
							// 選択したら、そのアニメーション名をおぼえる
							m_startAnimeName = n->GetName();
							change = true;
						}
					}
					ImGui::End();
				}
				// アニメーションが指定されたときは、アニメーションを変更する
				if (change >= 1) {
					m_animator.SetAnimation(m_model->GetAnimation(m_startAnimeName), m_startAnimationLoop);
				}

				// このボタンの上にマウスを置いた時だけ再生する
				ImGui::ButtonEx(u8"マウスオーバーで再生", ImVec2(0, 0), ImGuiButtonFlags_Disabled);
				if (ImGui::IsItemHovered()) {
					// アニメ進行
					m_animator.AdvanceTime(m_animationSpeed, m_nodeOL, nullptr);
				}
				// それ以外の時はデフォルトボーズにする
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
