#pragma once

//============================
// ���O�E�B���h�E�N���X
//============================
class ImGuiLogWindow 
{
public:
	// ���O���N���A
	void Clear() { m_Buf.clear(); }

	// �������ǉ�
	template<class... Args>
	void AddLog(const char* fmt, Args... args)
	{
		// ���s��������
		std::string str = fmt;
		str += "\n";
		m_Buf.appendf(str.c_str(), args...);
		m_ScrollToBottom = true;
	}

	// �E�B���h�E�`��
	void ImGuiUpdate(const char* title, bool* p_opened = NULL)
	{
		ImGui::Begin(title, p_opened);
		ImGui::TextUnformatted(m_Buf.begin());
		if (m_ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		m_ScrollToBottom = false;
		ImGui::End();
	}

private:
	ImGuiTextBuffer m_Buf;
	bool m_ScrollToBottom = false;
};

//�w���v�c�[���`�b�v�\���֐�
// �Etext �c �\��������
// �EshowIcom �c true:(?)��\�������̏�Ƀ}�E�X��������\��
// false:���݂̃R���g���[���̏�Ƀ}�E�X��������\��
inline void ImGuiShowHelp(const std::string& text, bool showIcon = false)
{
	if (showIcon) {
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(text.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

};


// KdModel�p�{�^��
bool ImGuiSelectModel(KdSptr<KdModel>& model);


// KdTexture�p�{�^��
bool ImGuiSelectTexture(KdSptr<KdTexture>& tex);

