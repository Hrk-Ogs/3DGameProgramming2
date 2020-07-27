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

bool SpriteComponent::IsHovered() const
{
	// �摜���Ȃ��ꍇ�̓X�L�b�v
	if (m_texture->IsValid() == false)return false;

	// �}�E�X���W���擾���N���C�A���g�̈��
	POINT curPos;
	GetCursorPos(&curPos);
	ScreenToClient(GetActiveWindow(), &curPos);
	// -��/2 �` ��/2 ��2D���W�n�֕ϊ�(Y�͏オ�{)
	KdVec2 pos = { (float)curPos.x, (float)curPos.y };
	pos.x -= D3D.GetBackBuffer()->GetWidth() / 2;
	pos.y -= D3D.GetBackBuffer()->GetHeight() / 2;
	pos.y *= -1;

	// ����Transform�̍��W�n�֕ϊ�
	KdMatrix mInv = GetOwner()->Transform()->GetMatrix();
	mInv.Inverse();
	pos.TransformCoord(mInv);

	// �摜�����ǂ�������
	Math::Rectangle rect;
	rect.x = -(long)(m_pivot.x * m_texture->GetWidth());
	rect.y = -(long)(m_pivot.y * m_texture->GetHeight());
	rect.width = (long)m_texture->GetWidth();
	rect.height = (long)m_texture->GetHeight();

	if (
		pos.x >= rect.x && pos.x <= rect.x + rect.width &&
		pos.y >= rect.y && pos.y <= rect.y + rect.height
		)
	{
		return true;
	}
	return false;
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
