#include "../GameSystem.h"

#include "SpriteComponent.h"

void SpriteComponent::SetTexture(const KdSptr<KdTexture>& tex)
{
	m_texture = tex;
	if (m_texture == nullptr) {
		// nullは許可しないので空のKdTextureを作成する
		m_texture = std::make_shared<KdTexture>();
	}
	else {
		m_srcRect = m_texture->GetRect();
	}
}

void SpriteComponent::PrepareDraw(RenderingData& rdata)
{
	// 有効じゃない
	if (m_enable == false)return;
	// テクスチャがない
	if (m_texture->IsValid() == false)return;

	// 描画登録
	rdata.m_drawSpriteList.push_back(this);
}

void SpriteComponent::Draw(int phaseID)
{
	// 有効じゃない
	if (m_enable == false)return;
	// テクスチャがない
	if (m_texture->IsValid() == false)return;

	// 行列セット
	const KdMatrix& m = GetOwner()->Transform()->GetMatrix();
	SHADER.m_spriteShader.SetMatrix(m);

	// 描画
	SHADER.m_spriteShader.DrawTex(m_texture.get(), 0, 0, &m_srcRect, (Math::Color*) & m_color, m_pivot);
}

void SpriteComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	// テクスチャ
	KdSptr<KdTexture> tex = m_texture;
	if (ImGuiSelectTexture(tex)) {
		SetTexture(tex);
	}

	// 色
	ImGui::ColorEdit4(u8"色", &m_color.x);
	// pivot
	ImGui::DragFloat2("Pivot", &m_pivot.x, 0.01f);
	// Rect
	ImGui::DragInt4("Rect", (int*)&m_srcRect.x);
}
