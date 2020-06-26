#include "../../GameSystem.h"

#include "CollisionManager.h"

bool ColliderComponent::s_showDebugLine = true;
KdVec4 ColliderComponent::s_debugLineColor = { 0.2f, 0.2f, 0.5f, 0.5f };
KdVec4 ColliderComponent::s_debugSelectLineColor = { 0.0f, 0.0f, 1.0f, 0.5f };

// 
float ColliderComponent::CalcMassRatio(float mass)
{
	float myMass = GetOwner() ? GetOwner()->GetMass() : 0;

	float ratio = 0;
	// 自分が最強の質量
	if (myMass == 0)
	{
		ratio = 0;
	}
	// 相手が最強の質量
	else if (mass == 0)
	{
		ratio = 1;
	}
	// 質量比計算
	else
	{
		ratio = mass / (myMass + mass);
	}
	return ratio;
}

void ColliderComponent::Register()
{
	// 登録済みならスキップ
	if (m_isRegistered) return;

	// 現在ActiveのCollisionManagerに登録する
	CollisionManager::sGetNowManager()->Register(
		std::static_pointer_cast<ColliderComponent>(shared_from_this())
	);
}

void ColliderComponent::Editor_ImGuiUpdate()
{
	// 
	BaseComponent::Editor_ImGuiUpdate();

	// Tag
	ImGui::InputText("Tag", &m_strTag);

	// 判定する側？
	if (ImGui::Checkbox(u8"判定する側(IsDoJudge)", &m_isDoJudge))
	{
		// フィルター初期化
		m_maskFilter = 0;
		m_groupFilter = 0;
	}
	ImGuiShowHelp(u8"On:判定を行い、その詳細な結果を取得できる\nOff:判定される側となり、結果はもらえない\n※注意※On vs Offペアのみが判定される\n※注意※On vs On同士は判定されない");

	// Is Trigger
	ImGui::Checkbox("Is Trigger", &m_isTrigger);
	ImGuiShowHelp(u8"On:判定のみ行うためすり抜ける\nOff:ぶつかり処理により押出しも行う");

	//----------------------
	// フィルタ
	//----------------------
	const std::vector<CollisionManager::FilterNode>* groupFilter;
	const std::vector<CollisionManager::FilterNode>* maskFilter;

	if (m_isDoJudge)
	{
		groupFilter = &CollisionManager::s_filterType1;
		maskFilter = &CollisionManager::s_filterType2;
	}
	else
	{
		groupFilter = &CollisionManager::s_filterType2;
		maskFilter = &CollisionManager::s_filterType1;
	}


	// グループフィルタ
	ImGui::BeginChild("GroupFilter", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 200), true);
	{
		ImGui::InputInt("##GroupFilter", &m_groupFilter);

		int pushCnt = 0;
		if (m_groupFilter == 0)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			pushCnt++;
		}

		ImGui::Text("Group Filter");
		for (auto&& node : *groupFilter)
		{
			ImGui::CheckboxFlags(node.Name.c_str(), (UINT*)&m_groupFilter, node.Flag);
		}
		if (ImGui::Button(u8"Clear"))
		{
			m_groupFilter = 0;
		}
		ImGui::PopStyleColor(pushCnt);
	}
	ImGui::EndChild();
	ImGui::SameLine();

	// マスクフィルタ
	ImGui::BeginChild("MaskFilter", ImVec2(0, 200), true);
	{
		ImGui::InputInt("##MaskFilter", &m_maskFilter);

		int pushCnt = 0;
		if (m_maskFilter == 0)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			pushCnt++;
		}

		ImGui::Text("Mask Filter");
		for (auto&& node : *maskFilter)
		{
			ImGui::CheckboxFlags(node.Name.c_str(), (UINT*)&m_maskFilter, node.Flag);
		}
		if (ImGui::Button(u8"Clear"))
		{
			m_maskFilter = 0;
		}

		ImGui::PopStyleColor(pushCnt);
	}
	ImGui::EndChild();
}
