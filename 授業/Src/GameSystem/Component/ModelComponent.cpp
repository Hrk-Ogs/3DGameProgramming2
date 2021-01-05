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
		
		// Rootモーション有効時
		if (m_enableRootMotion) {
			auto trans = GetOwner()->Transform();
			// 現在の行列を拡大・回転・座標へ分解
			KdVec3 scale = trans->GetLocalScale();
			KdQuaternion rotate = trans->GetlocalRotation();
			KdVec3 pos = trans->GetLocalPosition();

			// アニメーションを進める
			// ※Rootボーンの変化量をscale,rotate,posに適用する
			m_animator.AdvanceTime(m_animationSpeed, m_nodeOL, m_onAnimeScriptProc, &scale, &rotate, &pos);

			// 変化後のscale,rotate,posをセット
			trans->SetLocalScale(scale);
			trans->SetLocalRotation(rotate);
			trans->SetLocalPosition(pos);
		}
		else {
			// アニメーションを進める
			m_animator.AdvanceTime(m_animationSpeed, m_nodeOL, m_onAnimeScriptProc);
		}	
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

	// 描画登録
	if (m_mulMaterialColor.w >= 1.0f)
	{
		// 「3D不透明描画収集リスト」へ登録
		rdate.m_drawList.push_back(this);
	}
	else 
	{
		// 「3D半透明描画収集リスト」へ登録
		rdate.m_drawTransparentList.push_back(this);
	}
}

void ModelComponent::Draw(int phaseID)
{
	// 無効時はスキップ
	if (m_enable == false)return;
	if (m_visible == false)return;

	// GPUのバッファに全ボーンノードの行列を書き込む
	m_nodeOL.WriteToBoneBuffer();

	// TransformComponentを取得
	auto trans = GetOwner()->GetComponent<TransformComponent>();

	// ライトOn/Off
	SHADER.m_modelShader.SetLightEnable(m_lightEnable);
	// フォグOn/Off
	SHADER.m_modelShader.SetFogEnable(m_fogEnable);
	// 乗算色
	SHADER.m_modelShader.SetMulMaterialColor(m_mulMaterialColor);
	// UV設定
	SHADER.m_modelShader.SetUVOffset(m_uvOffset);
	SHADER.m_modelShader.SetUVTiling(m_uvTiling);
	// 屈折率設定
	SHADER.m_modelShader.SetRefractiveIndex(m_refractiveIndex);

	// 波表現
	if (m_useWaveNormalMap)
	{
		for (auto&& mate : m_model->GetMaterials())
		{
			// 強制的に使用する法線マップとしてセット
			mate->ForceUseNormalTex = GAMESYS.GetWaveNormalMap();
		}
	}

	// 描画設定を変更する
	SHADER.m_modelShader.SetToDevice();
	SHADER.m_modelShader.Draw(&m_nodeOL,m_model->GetMaterials(),trans->GetMatrix());

	// 波表現
	if (m_useWaveNormalMap)
	{
		for (auto&& mate : m_model->GetMaterials())
		{
			// 強制的に使用する法線マップを解除
			mate->ForceUseNormalTex = nullptr;
		}
	}

	//------------------------
	// 輪郭描画
	//------------------------
	if (m_enableOutline)
	{
		// 表面をカリング(非表示)にするラスタライザステートをセット
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullFront);

		// 描画
		SHADER.m_modelShader.SetToDevice_Outline();
		SHADER.m_modelShader.Draw_Outline(&m_nodeOL, trans->GetMatrix());

		// 裏面をカリング(非表示)にするラスタライザステートに戻す
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);
	}
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
		ImGui::Checkbox(u8"ライト有効", &m_lightEnable);
		ImGui::Checkbox(u8"フォグ有効", &m_fogEnable);
		ImGui::Checkbox(u8"輪郭有効", &m_enableOutline);
		ImGui::Checkbox(u8"波表現", &m_useWaveNormalMap);
		ImGui::ColorEdit4(u8"Material乗算色", &m_mulMaterialColor.x);
		ImGui::DragFloat(u8"屈折率", &m_refractiveIndex, 0.01f);

		ImGui::DragFloat2("UVOffset", &m_uvOffset.x, 0.001f);
		ImGui::DragFloat2("UVTiling", &m_uvTiling.x, 0.001f);

		ImGui::TreePop();
	}

	// アニメーション
	if (ImGui::TreeNodeEx("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
		// モデルを読み込んでいる時
		if (m_model->IsValid()) {

			ImGui::Checkbox(u8"RootMotion有効", &m_enableRootMotion);

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
