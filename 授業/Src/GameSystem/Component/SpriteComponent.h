#pragma once

//=================================
//
// スプライトコンポーネント
//
//=================================
class SpriteComponent :public BaseComponent
{
public:
	// テクスチャをセット
	void SetTexture(const KdSptr<KdTexture>& tex);

	// マウスが画像の上にあるか？
	bool IsHovered()const;

	// 描画準備
	virtual void PrepareDraw(RenderingData& rdata)override;

	// 描画
	virtual void Draw(int phaseID)override;

	// （Editor)ImGui
	virtual void Editor_ImGuiUpdate()override;

	//==================================
	// Serialize/Deserialize
	//==================================
	//JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);

		std::string filename;
		KdJsonGet(jsonData["TexFilename"], filename);
		SetTexture(KDResFactory.GetTexture(filename));

		KdJsonGet(jsonData["Color"], m_color);
		KdJsonGet(jsonData["Pivot"], m_pivot);

		auto& rectAry = jsonData["SrcRect"].array_items();
		KdJsonGet(rectAry[0], m_srcRect.x);
		KdJsonGet(rectAry[1], m_srcRect.y);
		KdJsonGet(rectAry[2], m_srcRect.width);
		KdJsonGet(rectAry[3], m_srcRect.height);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["TexFilename"] = m_texture->GetFilepath();

		outJsonObj["Color"] = m_color.ToArray();
		outJsonObj["Pivot"] = m_pivot.ToArray();
		outJsonObj["SrcRect"] = std::vector<int>{ m_srcRect.x,m_srcRect.y,m_srcRect.width,m_srcRect.height };
	}

private:

	// テクスチャ
	KdSptr<KdTexture>		m_texture = std::make_shared<KdTexture>();

	// 元画像のRECT
	Math::Rectangle			m_srcRect;
	// 色
	KdVec4					m_color = { 1,1,1,1 };
	// 基準点
	KdVec2					m_pivot = { 0.5f, 0.5f };
};
