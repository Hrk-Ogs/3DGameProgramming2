#pragma once

#include "BaseComponent.h"

//============================================
// 描画の詳細設定コンポーネント
//============================================
class GraphicsSettingComponent : public BaseComponent
{
public:
	// 描画準備処理
	virtual void PrepareDraw(RenderingData& rData) override
	{
		// 環境光設定
		SHADER.m_cb8_Light.Work().AmbientLight = m_ambientLight;
		
		// 距離フォグ設定
		SHADER.m_cb7_Camera.Work().DistanceFogEnable = 1;
		SHADER.m_cb7_Camera.Work().DistanceFogColor = m_distanceFogColor;
		SHADER.m_cb7_Camera.Work().DistanceFogDensity = m_distanceFogDensity;
	}


	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);
		KdJsonGet(jsonData["AmbientLight"], m_ambientLight);
		KdJsonGet(jsonData["DistanceFogColor"], m_distanceFogColor);
		KdJsonGet(jsonData["DistanceFogDensity"], m_distanceFogDensity);
	}
	
	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseComponent::Serialize(outJsonObj);
		outJsonObj["AmbientLight"] = m_ambientLight.ToArray();
		outJsonObj["DistanceFogColor"] = m_distanceFogColor.ToArray();
		outJsonObj["DistanceFogDensity"] = m_distanceFogDensity;
	}
	//
	virtual void Editor_ImGuiUpdate() override
	{
		BaseComponent::Editor_ImGuiUpdate();
		ImGui::ColorEdit3(u8"環境光", &m_ambientLight.x);
		ImGui::ColorEdit3(u8"フォグ色", &m_distanceFogColor.x);
		ImGui::DragFloat(u8"フォグ密度", &m_distanceFogDensity, 0.0001f, 0, 1, "%.4f");
	}

private:
	// 環境光
	KdVec3 m_ambientLight = { 0.1f, 0.1f, 0.1f };

	// 距離フォグ
	KdVec3 m_distanceFogColor = { 1,1,1 };
	float m_distanceFogDensity = 0.01f;
};