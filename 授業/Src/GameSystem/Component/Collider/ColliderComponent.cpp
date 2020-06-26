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
	// �������ŋ��̎���
	if (myMass == 0)
	{
		ratio = 0;
	}
	// ���肪�ŋ��̎���
	else if (mass == 0)
	{
		ratio = 1;
	}
	// ���ʔ�v�Z
	else
	{
		ratio = mass / (myMass + mass);
	}
	return ratio;
}

void ColliderComponent::Register()
{
	// �o�^�ς݂Ȃ�X�L�b�v
	if (m_isRegistered) return;

	// ����Active��CollisionManager�ɓo�^����
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

	// ���肷�鑤�H
	if (ImGui::Checkbox(u8"���肷�鑤(IsDoJudge)", &m_isDoJudge))
	{
		// �t�B���^�[������
		m_maskFilter = 0;
		m_groupFilter = 0;
	}
	ImGuiShowHelp(u8"On:������s���A���̏ڍׂȌ��ʂ��擾�ł���\nOff:���肳��鑤�ƂȂ�A���ʂ͂��炦�Ȃ�\n�����Ӂ�On vs Off�y�A�݂̂����肳���\n�����Ӂ�On vs On���m�͔��肳��Ȃ�");

	// Is Trigger
	ImGui::Checkbox("Is Trigger", &m_isTrigger);
	ImGuiShowHelp(u8"On:����̂ݍs�����߂��蔲����\nOff:�Ԃ��菈���ɂ�艟�o�����s��");

	//----------------------
	// �t�B���^
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


	// �O���[�v�t�B���^
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

	// �}�X�N�t�B���^
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
