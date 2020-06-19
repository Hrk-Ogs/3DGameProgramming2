#pragma once

//============================
// ログウィンドウクラス
//============================
class ImGuiLogWindow 
{
public:
	// ログをクリア
	void Clear() { m_Buf.clear(); }

	// 文字列を追加
	template<class... Args>
	void AddLog(const char* fmt, Args... args)
	{
		// 改行を加える
		std::string str = fmt;
		str += "\n";
		m_Buf.appendf(str.c_str(), args...);
		m_ScrollToBottom = true;
	}

	// ウィンドウ描画
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

//ヘルプツールチップ表示関数
// ・text … 表示文字列
// ・showIcom … true:(?)を表示しその上にマウスがきたら表示
// false:現在のコントロールの上にマウスがきたら表示
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


// KdModel用ボタン
bool ImGuiSelectModel(KdSptr<KdModel>& model);


// KdTexture用ボタン
bool ImGuiSelectTexture(KdSptr<KdTexture>& tex);

