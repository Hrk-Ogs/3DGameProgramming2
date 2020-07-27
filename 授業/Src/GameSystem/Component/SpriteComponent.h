#pragma once

//=================================
//
// �X�v���C�g�R���|�[�l���g
//
//=================================
class SpriteComponent :public BaseComponent
{
public:
	// �e�N�X�`�����Z�b�g
	void SetTexture(const KdSptr<KdTexture>& tex);

	// �}�E�X���摜�̏�ɂ��邩�H
	bool IsHovered()const;

	// �`�揀��
	virtual void PrepareDraw(RenderingData& rdata)override;

	// �`��
	virtual void Draw(int phaseID)override;

	// �iEditor)ImGui
	virtual void Editor_ImGuiUpdate()override;

	//==================================
	// Serialize/Deserialize
	//==================================
	//JSON�f�[�^����A�N���X�̓��e��ݒ�
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

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["TexFilename"] = m_texture->GetFilepath();

		outJsonObj["Color"] = m_color.ToArray();
		outJsonObj["Pivot"] = m_pivot.ToArray();
		outJsonObj["SrcRect"] = std::vector<int>{ m_srcRect.x,m_srcRect.y,m_srcRect.width,m_srcRect.height };
	}

private:

	// �e�N�X�`��
	KdSptr<KdTexture>		m_texture = std::make_shared<KdTexture>();

	// ���摜��RECT
	Math::Rectangle			m_srcRect;
	// �F
	KdVec4					m_color = { 1,1,1,1 };
	// ��_
	KdVec2					m_pivot = { 0.5f, 0.5f };
};
