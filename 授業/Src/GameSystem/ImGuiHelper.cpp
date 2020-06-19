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

// KdTexture�p�{�^��
// �Etex�@�c�c�I�����ꂽ�摜������
bool ImGuiSelectTexture(KdSptr<KdTexture>& tex)
{
	bool ret = false;
	ImGui::PushID(tex.get());
	std::string str = "No Texture File";
	// �t�@�C����������Ƃ�
	if (tex && tex->GetFilepath().size() >= 1)str = tex->GetFilepath();
	// �{�^��
	if (ImGui::Button(str.c_str())) {
		// �t�@�C�����J��
		if (KdWindow::OpenFileDialog(str, "�摜�t�@�C�����J��", "�摜�t�@�C��\0*.png;*.bmp;*.jpg;*.dds;*.hdr;*.tga\0")) {
			// �摜�擾�i�ǂݍ��݁j
			tex = KDResFactory.GetTexture(str);
			// �I�����N���A
			ret = true;
		}
	}

	// ���j���[
	if (ImGui::BeginPopupContextItem("SelectTexturePopupUPID")) {
		if (ImGui::Selectable(u8"����")) {
			tex = nullptr;
			ret = true;
		}
		ImGui::EndPopup();
	}
	
	ImGui::PopID();
	return ret;
}
