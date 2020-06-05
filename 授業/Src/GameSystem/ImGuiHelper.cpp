#include "GameSystem.h"

#include "ImGuiHelper.h"

bool ImGuiSelectModel(KdSptr<KdModel>& model)
{
	bool ret = false;
	ImGui::PushID(model.get());
	std::string str = "No Model File";
	// �t�@�C����������Ƃ�
	if (model && model->GetFilepath().size() >= 1)str = model->GetFilepath();
	// �{�^��
	if (ImGui::Button(str.c_str()))
	{
		// �t�@�C���J��
		if (KdWindow::OpenFileDialog(str, "���f���t�@�C�����J��", "gltf�t�@�C��\0*.gltf\0"))
		{
			// ���f���擾(�ǂݍ���)
			model = KDResFactory.GetModel(str);
			// �I�����N���A
			ret = true;
		}
	}

	// ���j���[
	if (ImGui::BeginPopupContextItem("SelectModelPopupUpID"))
	{
		if (ImGui::Selectable(u8"����"))
		{
			model = nullptr;
			ret = true;
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
	return ret;
}