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
		// IBLテクスチャセット
		D3D.GetDevContext()->PSSetShaderResources(11, 1, m_texIBL->GetSRViewAddress());

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

		KdJsonGet(jsonData["IBLTexFilename"], m_texIBL);
		if (m_texIBL == nullptr)
		{
			// nullは避ける
			m_texIBL = std::make_shared<KdTexture>();
		}

		KdJsonGet(jsonData["AmbientLight"], m_ambientLight);
		KdJsonGet(jsonData["DistanceFogColor"], m_distanceFogColor);
		KdJsonGet(jsonData["DistanceFogDensity"], m_distanceFogDensity);
	}
	
	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["IBLTexFilename"] = m_texIBL->GetFilepath();
		outJsonObj["AmbientLight"] = m_ambientLight.ToArray();
		outJsonObj["DistanceFogColor"] = m_distanceFogColor.ToArray();
		outJsonObj["DistanceFogDensity"] = m_distanceFogDensity;
	}
	//
	virtual void Editor_ImGuiUpdate() override
	{
		BaseComponent::Editor_ImGuiUpdate();

		ImGui::Text(u8"IBLテクスチャ");
		ImGuiSelectTexture(m_texIBL);
		if (m_texIBL == nullptr)
		{
			// 未選択時はnullオブジェクトを入れておく
			m_texIBL = std::make_shared<KdTexture>();
		}
		ImGui::ColorEdit3(u8"環境光", &m_ambientLight.x);
		ImGui::ColorEdit3(u8"フォグ色", &m_distanceFogColor.x);
		ImGui::DragFloat(u8"フォグ密度", &m_distanceFogDensity, 0.0001f, 0, 1, "%.4f");
	}

private:
	// IBLテクスチャ
	KdSptr<KdTexture> m_texIBL = std::make_shared<KdTexture>();

	// 環境光
	KdVec3 m_ambientLight = { 0.1f, 0.1f, 0.1f };

	// 距離フォグ
	KdVec3 m_distanceFogColor = { 1,1,1 };
	float m_distanceFogDensity = 0.01f;
};