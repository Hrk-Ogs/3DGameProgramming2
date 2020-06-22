#include "../GameSystem.h"

#include "LightComponent.h"

void LightComponent::PrepareDraw(RenderingData& rdata)
{
	if (m_enable == false)return;

	m_myLightIndex = -1;

	//-----------------
	// ライトデータ登録
	//-----------------
	// ディレクショナルライト
	if (m_lightType == LightTypes::Directional) {
		// 現在の登録数
		int idx = SHADER.m_cb8_Light.Work().DL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().DL.size()) {
			m_myLightIndex = idx;	// 自分のライト番号

			KdMatrix m = GetOwner()->Transform()->GetMatrix();

			// ライトデータを定数バッファへ書き込む
			SHADER.m_cb8_Light.Work().DL[idx].Dir = m.Forward();
			SHADER.m_cb8_Light.Work().DL[idx].Dir.Normalize();
			SHADER.m_cb8_Light.Work().DL[idx].Color = m_color * m_intensity;

			// ライト数を加算
			SHADER.m_cb8_Light.Work().DL_Cnt++;
		}
	}
	// ポイントライト
	else if (m_lightType == LightTypes::Point) {
		KdMatrix m = GetOwner()->Transform()->GetMatrix();

		// 現在の登録数
		int idx = SHADER.m_cb8_Light.Work().PL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().PL.size())	// 限界数を超えている場合は、このライトは使用しない
		{
			m_myLightIndex = idx;

			// ライトデータを定数バッファへ書き込む
			SHADER.m_cb8_Light.Work().PL[idx].Pos = m.Translation();
			SHADER.m_cb8_Light.Work().PL[idx].Radius = m_range;
			SHADER.m_cb8_Light.Work().PL[idx].Color = m_color * m_intensity;

			// ライト数を加算
			SHADER.m_cb8_Light.Work().PL_Cnt++;
		}
	}
	else if(m_lightType==LightTypes::Spot){
		KdMatrix m = GetOwner()->Transform()->GetMatrix();

		// 現在の登録数
		int idx = SHADER.m_cb8_Light.Work().SL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().SL.size()) {
			m_myLightIndex = idx;		// 自分のライト番号

			// ライトデータを定数バッファへ書き込む
			SHADER.m_cb8_Light.Work().SL[idx].Pos = m.Translation();
			SHADER.m_cb8_Light.Work().SL[idx].Dir = m.Forward();
			SHADER.m_cb8_Light.Work().SL[idx].Dir.Normalize();
			SHADER.m_cb8_Light.Work().SL[idx].Range = m_range;
			SHADER.m_cb8_Light.Work().SL[idx].Color = m_color * m_intensity;
			SHADER.m_cb8_Light.Work().SL[idx].OuterCorn = m_spotAngle * KdToRadians;

			// ライト数を加算
			SHADER.m_cb8_Light.Work().SL_Cnt++;
		}
	}
}

void LightComponent::Editor_ImGuiUpdate()
{
	// 継承前の処理も実行
	BaseComponent::Editor_ImGuiUpdate();

	// ライトの種類
	std::vector<char*> lightTypes = { u8"Directional",u8"Point",u8"Spot" };
	ImGui::Combo(u8"種類", (int*)&m_lightType, &lightTypes[0], lightTypes.size());
	// 色
	ImGui::ColorEdit3(u8"Color", &m_color.x);
	// 強度
	ImGui::DragFloat(u8"Intensity", &m_intensity, 0.01f, 0, 100);

	// 範囲（点光と集中光のみの設定)
	if (m_lightType == LightTypes::Point || m_lightType == LightTypes::Spot) {
		ImGui::DragFloat(u8"Range", &m_range, 0.01f, 0, 10000);
	}
	// 角度（集中光のみの設定）
	if (m_lightType == LightTypes::Spot) {
		ImGui::DragFloat(u8"Angle", &m_spotAngle, 0, 0, 360);
	}

	//================
	// デバッグ線
	//================
	// ディレクショナルライト
	KdMatrix m = GetOwner()->Transform()->GetMatrix();
	if (m_lightType == LightTypes::Directional) {
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
		m = KdMatrix().CreateTranslation(0.1f, 0, 0) * m;
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
		m = KdMatrix().CreateTranslation(-0.2f, 0, 0) * m;
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
	}
	else if (m_lightType == LightTypes::Point) {
		// デバッグ線
		GAMESYS.DebugLine().AddSphereLine(m.Translation(), m_range, m_color.ToVec4(0.5f));
	}

}
