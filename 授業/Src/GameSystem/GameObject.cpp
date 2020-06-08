#include "GameSystem.h"

#include "GameObject.h"

void GameObject::Init()
{
	// TransformComponent�̒ǉ�
	auto comp = AddComponent("TransformCompnent");
}

void GameObject::Update()
{
	// �������̓X�L�b�v����
	if (!m_enable)return;

	// �S�R���|�[�l���g�𓮍�
	for (auto&& comp : m_components) {
		comp->Update();
	}

	// ���@�q������
	for (auto&& child : m_children) {
		child->Update();
	}

}

void GameObject::Draw()
{
	// ������Ԃ̓X�L�b�v����
	if (!m_enable)return;

	// �S�R���|�[�l���g��`��
	for (auto&& comp : m_components) {
		comp->Draw();
	}

	// ���@�q���`��
	for (auto&& child : m_children) {
		child->Draw();
	}
}

void GameObject::AddComponent(const KdSptr<BaseComponent>& comp)
{
	// ������Z�b�g
	comp->SetOwner(shared_from_this());
	// �ǉ�
	m_components.push_back(comp);

}

KdSptr<BaseComponent> GameObject::AddComponent(const std::string& className)
{
	KdSptr<BaseComponent> comp;
	if (className == "ModelComponent") {
		comp = std::make_shared<ModelComponent>();
	}
	AddComponent(comp);

	return comp;
}

void GameObject::SetParent(const KdSptr<GameObject>& newParent)
{
	// �e����E�ނ���O�Ɏ�����shared_ptr���L�������Ă���
	KdSptr<GameObject> spThis = shared_from_this();

	// ���݂̐e����E�ނ���
	auto nowParent = m_parent.lock();
	if (nowParent) {	// �e�����݂���
		// �e�̃��X�g���玩��������
		auto it = std::find(nowParent->m_children.begin(), nowParent->m_children.end(), spThis);
		// �����𔭌�
		if (it != nowParent->m_children.end()) {
			// �e�̃��X�g�����������
			nowParent->m_children.erase(it);
		}
	}

	// �V�����e�̃A�h���X���L������
	m_parent = newParent;

	// �V�����e�̎q�Ƃ��āA������o�^����
	if (newParent) {
		newParent->m_children.push_back(spThis);
	}

}

void GameObject::Deserialize(const json11::Json& jsonObj)
{
	// �L���t���O
	KdJsonGet(jsonObj["Enable"], m_enable);
	// ���O
	KdJsonGet(jsonObj["Name"], m_name);

	// ���W
	KdVec3 pos;
	KdJsonGet(jsonObj["Pos"], pos);
	m_transform.Translation(pos);	// ���W���Z�b�g

	// �R���|�[�l���g
	m_components.clear();
	for (auto&& jsonComp : jsonObj["Components"].array_items()) {
		// JSON�f�[�^����A�R���|�[�l���g�̃N���X�����擾
		std::string className = jsonComp["ClassName"].string_value();
		// �R���|�[�l���g��ǉ�
		auto comp = AddComponent(className);
		if (comp) {
			// �f�V���A���C�Y
			comp->Deserialize(jsonComp);
		}
	}
	// �qGameObject
	m_children.clear();
	for (auto&& jsonChild : jsonObj["Children"].array_items()) {
		// �������m��
		KdSptr<GameObject> child = std::make_shared<GameObject>();
		// �f�V���A���C�Y
		child->Deserialize(jsonChild);
		// ������e�Ƃ��ċL��������
		child->m_parent = shared_from_this();
		// �q�Ƃ��ēo�^
		m_children.push_back(child);
	}

}

void GameObject::Serialize(json11::Json::object& outJsonObj) const
{
	outJsonObj["Name"] = m_name;
	outJsonObj["Enable"] = m_enable;
	outJsonObj["Pos"] = m_transform.Translation().ToArray();

	/*
	if (m_model) {
		outJsonObj["ModelFilename"] = m_model->GetFilepath();
	}
	*/

	// �q���X�g���V���A���C�Y
	json11::Json::array jsonChildren;
	for (auto&& child : m_children) {
		// JSON�f�[�^��
		json11::Json::object serial;
		child->Serialize(serial);
		// JSON�q�z��֒ǉ�
		jsonChildren.push_back(serial);
	}
	outJsonObj["Children"] = jsonChildren;

	// �S�R���|�[�l���g���V���A���C�Y
	json11::Json::array jsonCompAry;
	for (auto&& comp : m_components) {
		// JSON�f�[�^��
		json11::Json::object serial;
		comp->Serialize(serial);
		// �z��֒ǉ�
		jsonCompAry.push_back(serial);
	}
	outJsonObj["Components"] = jsonCompAry;
}

// Inspector�ŕ\���������e
void GameObject::Editor_ImGuiUpdate()
{
	// �L������
	ImGui::Checkbox("Enable", &m_enable);

	ImGui::Dummy(ImVec2(0, 2));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 2));

	// ���O
	ImGui::InputText("Name", &m_name);

	// ���W
	ImGui::DragFloat3("Pos", &m_transform._41, 0.01f);

	// �S�R���|�[�l���g�𓮍�
	for (auto&& comp : m_components) {

		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 2));

		ImGui::PushID(comp.get());

		if (ImGui::CollapsingHeader(&typeid(*comp).name()[6], ImGuiTreeNodeFlags_DefaultOpen)) {
			comp->Editor_ImGuiUpdate();
		}

		ImGui::PopID();
	}

	// �R���|�[�l���g�ǉ��{�^��
	ImGui::Dummy(ImVec2(0, 2));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 5));

	ImGui::Button(u8"�R���|�[�l���g�ǉ�");
	// ���̃{�^�����N���b�N����ƃ|�b�v�A�b�v���j���[�\��
	if (ImGui::BeginPopupContextItem("AddComponentPopup", 0)) {
		// ��
		if (ImGui::Selectable("ModelComponent")) {
			// �R���|�[�l���g����
			auto comp = std::make_shared<ModelComponent>();
			// �e��o�^
			comp->SetOwner(shared_from_this());
			// ���X�g�ɐڑ�
			m_components.push_back(comp);
		}

		ImGui::EndPopup();
	}

}

void GameObject::Editor_ImGuiTree()
{
	ImGui::PushID(this);

	// TreeNode�t���O
	int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

	// �I�𒆁H
	if (GAMESYS.m_editorData.SelectObj.lock() == shared_from_this()) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	// GameObject
	bool bOpen = ImGui::TreeNodeEx(m_name.c_str(), flags);

	// �N���b�N����
	if (ImGui::IsItemClicked()) {
		// �I�𕨂Ƃ��ċL��
		GAMESYS.m_editorData.SelectObj = shared_from_this();
	}


	//--------------------------------------------
	// �E�N���b�N�ŃR���e�L�X�g���j���[��\��
	//--------------------------------------------
	if (ImGui::BeginPopupContextItem("GameObjectPopupID", 1)) {

		//------------------------
		// Prefab�ۑ�
		//------------------------
		if (ImGui::Selectable(u8"Prefab�t�@�C���֕ۑ�")) {

			std::string filepath;
			if (KdWindow::SaveFileDialog(filepath, "Prefab�ۑ�", "pref�t�@�C��\0*.pref\0", "pref")) {
				// �ۑ�
				// GameObject���V���A���C�Y
				json11::Json::object serial;
				Serialize(serial);
				// JSON�I�u�W�F�N�g�𕶎���
				json11::Json jsn = serial;
				std::string text = jsn.dump(true);	// ������
				// �t�@�C���֕ۑ�
				std::ofstream ofs(filepath);
				if (ofs) {
					ofs.write(text.c_str(), text.size());
				}
				// ���O�\��
				GAMESYS.m_editorData.LogWindow.AddLog(u8"Prefab�ۑ�[%s]", filepath.c_str());
			}
		}

		//------------------------
		// Prefab�ǂݍ���
		//------------------------
		if (ImGui::Selectable(u8"Prefab�t�@�C����ǂݍ���")) {

			std::string filepath;
			if (KdWindow::OpenFileDialog(filepath, "Prefab�J��", "pref�t�@�C��\0*.pref\0")) {
				// JSON�t�@�C���ǂݍ���
				json11::Json jsonObj = KdLoadJSONFile(filepath);
				if (jsonObj.is_null() == false) {
					// GameObject�쐬
					KdSptr<GameObject> obj = std::make_shared<GameObject>();

					// �f�V���A���C�Y
					obj->Deserialize(jsonObj);

					// ������e�Ƃ��Đݒ�
					obj->SetParent(shared_from_this());
				}
			}
		}

		//------------------------
		// �V�K�쐬
		//------------------------
		if (ImGui::Selectable(u8"�V�KGameObject���쐬")) {
			KdSptr<GameObject> obj = std::make_shared<GameObject>();
			// ������e�Ƃ����ݒ�
			obj->SetParent(shared_from_this());
		}

		// �K���Ō�ɌĂ�
		ImGui::EndPopup();
	}
	//-----------�����܂Ń��j���[-----------------------------------

	//-----------------------
	// �q��ImGuiTree����
	//-----------------------
	if (bOpen) {
		for (auto&& child : m_children) {
			child->Editor_ImGuiTree();
		}

		// TreeNodeEx�̍Ō�ɂ͐�΂Ɏ��s����
		ImGui::TreePop();
	}

	// 
	ImGui::PopID();
}
