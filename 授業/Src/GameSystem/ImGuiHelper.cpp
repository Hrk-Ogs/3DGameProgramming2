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