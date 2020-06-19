#include "GameSystem.h"

#include "ImGuiHelper.h"

bool ImGuiSelectModel(KdSptr<KdModel>& model)
{
	bool ret = false;
	ImGui::PushID(model.get());
	std::string str = "No Model File";
	// ファイル名があるとき
	if (model && model->GetFilepath().size() >= 1)str = model->GetFilepath();
	// ボタン
	if (ImGui::Button(str.c_str()))
	{
		// ファイル開く
		if (KdWindow::OpenFileDialog(str, "モデルファイルを開く", "gltfファイル\0*.gltf\0"))
		{
			// モデル取得(読み込み)
			model = KDResFactory.GetModel(str);
			// 選択をクリア
			ret = true;
		}
	}

	// メニュー
	if (ImGui::BeginPopupContextItem("SelectModelPopupUpID"))
	{
		if (ImGui::Selectable(u8"解除"))
		{
			model = nullptr;
			ret = true;
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
	return ret;
}

// KdTexture用ボタン
// ・tex　……選択された画像が入る
bool ImGuiSelectTexture(KdSptr<KdTexture>& tex)
{
	bool ret = false;
	ImGui::PushID(tex.get());
	std::string str = "No Texture File";
	// ファイル名があるとき
	if (tex && tex->GetFilepath().size() >= 1)str = tex->GetFilepath();
	// ボタン
	if (ImGui::Button(str.c_str())) {
		// ファイルを開く
		if (KdWindow::OpenFileDialog(str, "画像ファイルを開く", "画像ファイル\0*.png;*.bmp;*.jpg;*.dds;*.hdr;*.tga\0")) {
			// 画像取得（読み込み）
			tex = KDResFactory.GetTexture(str);
			// 選択をクリア
			ret = true;
		}
	}

	// メニュー
	if (ImGui::BeginPopupContextItem("SelectTexturePopupUPID")) {
		if (ImGui::Selectable(u8"解除")) {
			tex = nullptr;
			ret = true;
		}
		ImGui::EndPopup();
	}
	
	ImGui::PopID();
	return ret;
}
