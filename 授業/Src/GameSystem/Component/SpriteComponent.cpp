#include "../GameSystem.h"

#include "SpriteComponent.h"

void SpriteComponent::SetTexture(const KdSptr<KdTexture>& tex)
{
	m_texture = tex;
	if (m_texture == nullptr) {
		// null�͋����Ȃ��̂ŋ��KdTexture���쐬����
		m_texture = std::make_shared<KdTexture>();
	}
	else {
		m_srcRect = m_texture->GetRect();
	}
}

void SpriteComponent::PrepareDraw(RenderingData& rdata)
{
	// �L������Ȃ�
	if (m_enable == false)return;
	// �e�N�X�`�����Ȃ�
	if (m_texture->IsValid() == false)return;

	// �`��o�^
	rdata.m_drawSpriteList.push_back(this);
}

void SpriteComponent::Draw(int phaseID)
{
	// �L������Ȃ�
	if (m_enable == false)return;
	// �e�N�X�`�����Ȃ�
	if (m_texture->IsValid() == false)return;

	// �s��Z�b�g
	const KdMatrix& m = GetOwner()->Transform()->GetMatrix();
	SHADER.m_spriteShader.SetMatrix(m);

	// �`��
	SHADER.m_spriteShader.DrawTex(m_texture.get(), 0, 0, &m_srcRect, (Math::Color*) & m_color, m_pivot);
}

void SpriteComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	// �e�N�X�`��
	KdSptr<KdTexture> tex = m_texture;
	if (ImGuiSelectTexture(tex)) {
		SetTexture(tex);
	}

	// �F
	ImGui::ColorEdit4(u8"�F", &m_color.x);
	// pivot
	ImGui::DragFloat2("Pivot", &m_pivot.x, 0.01f);
	// Rect
	ImGui::DragInt4("Rect", (int*)&m_srcRect.x);
}
