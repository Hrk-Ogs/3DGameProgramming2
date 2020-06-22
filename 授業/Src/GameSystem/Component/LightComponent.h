#pragma once

//======================================
//
// ライトコンポーネント
//
//======================================
class LightComponent :public BaseComponent
{
public:

	//========================
	// コンポーネント処理
	//========================

	// 描画準備
	virtual void PrepareDraw(RenderingData& rdata)override;

	//(Editor用）ImGui
	virtual void Editor_ImGuiUpdate()override;

	//========================
	// Serialize/Deserialize
	//========================
	//JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["LightType"], (int&)m_lightType);
		KdJsonGet(jsonData["Color"], m_color);
		KdJsonGet(jsonData["Range"], m_range);
		KdJsonGet(jsonData["SpotAngle"], m_spotAngle);
		KdJsonGet(jsonData["Intensity"], m_intensity);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["LightType"] = (int)m_lightType;
		outJsonObj["Color"] = m_color.ToArray();
		outJsonObj["Range"] = m_range;
		outJsonObj["SpotAngle"] = m_spotAngle;
		outJsonObj["Intensity"] = m_intensity;

	}


private:

	// ライトの種類定数
	enum class LightTypes {
		Directional = 0,		// 平行光
		Point=1,				// 点光
		Spot=2,					// 集中光
	};
	// ライトの種類　0:平行ライト　1:ポイントライト 2：スポットライト
	LightTypes m_lightType = LightTypes::Point;

	KdVec3	m_color = { 1,1,1 };	// 色
	float	m_range = 5;			// 点光の範囲
	float	m_spotAngle = 30;		//（スポットライト専用）角度
	float	m_intensity = 2.0f;		//強度
	
	// 実行時、現在のライト番号
	int		m_myLightIndex = -1;
};
