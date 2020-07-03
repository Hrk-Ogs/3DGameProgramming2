#include "GameSystem.h"

#include "GameObject.h"

void GameObject::Collect(std::vector<GameObject*>& gameObjectList)
{
	// �������͑ΏۊO
	if (m_enable == false)return;

	// �����Ώۂɓo�^����
	gameObjectList.push_back(this);

	// �q���ċA�Ŏ��s���Ă����i�C�e���[�^�Ń��[�v���Ă�̂́A�폜�@�\����Ɏ������邽�߁j
	for (auto it = m_children.begin(); it != m_children.end();) {
		// �폜�t���O��On�H
		if ((*it)->IsDestory()) {
			// ���X�g�������
			it = m_children.erase(it);
			continue;
		}

		// �o�^
		(*it)->Collect(gameObjectList);

		// ����
		++it;
	}
}

void GameObject::Init()
{
	// TransformComponent�̒ǉ�
	m_components.clear();
	auto trans = std::make_shared<TransformComponent>();
	AddComponent(trans);
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
	// �N���X������C���X�^���X�𐶐�
	KdSptr<BaseComponent> comp = ComponentClassMaker::GetInstance().MakeShared(className);
	/*
	if (className == "ModelComponent") {
		comp = std::make_shared<ModelComponent>();
	}
	if (className == "ModelComponent") {
		comp = std::make_shared<TransformComponent>();
	}
	*/

	if (comp == nullptr)return nullptr;

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

KdSptr<GameObject> GameObject::Find(std::string_view name)
{
	// ���O�𔻒�
	if (m_name == name)return shared_from_this();

	// �q������
	for (auto&& child : m_children) {
		auto ret = child->Find(name);
		// ���������炻�̂܂ܕԂ�
		if (ret)return ret;
	}

	return nullptr;
}

void GameObject::Deserialize(const json11::Json& jsonObj)
{
	// �L���t���O
	KdJsonGet(jsonObj["Enable"], m_enable);
	// ���O
	KdJsonGet(jsonObj["Name"], m_name);
	// ����
	KdJsonGet(jsonObj["Mass"], m_mass);

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
	outJsonObj["Mass"] = m_mass;

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

	// ����
	ImGui::InputFloat(u8"����", &m_mass);

	// �S�R���|�[�l���g�𓮍�
	//for (auto&& comp : m_components) {
	for(auto&& it=m_components.begin();it!=m_components.end();)
	{
		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 2));

		auto& comp = (*it);

		ImGui::PushID(comp.get());

		bool bOpen = ImGui::CollapsingHeader(&typeid(*comp).name()[6], ImGuiTreeNodeFlags_DefaultOpen);

		// ���j���[
		if (ImGui::BeginPopupContextItem("ComponentMenuPopup")) {
			if (ImGui::Selectable(u8"�폜")) {
				it = m_components.erase(it);
				bOpen = false;
			}

			ImGui::EndPopup();
		}

		if (bOpen) {
			comp->Editor_ImGuiUpdate();
		}

		ImGui::PopID();

		if (it == m_components.end())break;
		++it;
	}

	// �R���|�[�l���g�ǉ��{�^��
	ImGui::Dummy(ImVec2(0, 2));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, 5));

	ImGui::Button(u8"�R���|�[�l���g�ǉ�");
	// ���̃{�^�����N���b�N����ƃ|�b�v�A�b�v���j���[�\��
	if (ImGui::BeginPopupContextItem("AddComponentPopup", 0)) {
		// ComponentClassMaker�ɓo�^����Ă���N���X�����ꗗ�Ŏ擾���A���X�g�\������
		std::vector<std::string> classList = ComponentClassMaker::GetInstance().GetClassNameList();
		for (auto&& className : classList) {
			if (ImGui::Selectable(className.c_str())) {
				AddComponent(className);
			}
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

	//---------------------
	// Drag & Drop
	//---------------------
	//Drag
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		// GameObject�̃A�h���X�𐔒l�Ƃ��ċL�����Ă���
		intptr_t n = (intptr_t)this;
		ImGui::SetDragDropPayload("GameObjectDragDrop", &n, sizeof(n));
		ImGui::Text(u8"�e�ƂȂ�GameObject��I�����Ă�������");
		ImGui::EndDragDropSource();
	}
	// Drop
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObjectDragDrop")) {
			IM_ASSERT(payload->DataSize == sizeof(intptr_t));
			// int�^�ŋL�����Ă����A�h���X��Ojbect�ɕ���
			GameObject* srcObj = (GameObject*)(*(intptr_t*)payload->Data);

			// �e�q�\���`�F�b�N�i�e����q�ւ̃h���b�v�_�E���͂܂����̂ł�����`�F�b�N!�j
			GameObject* o = this;
			while (o != nullptr) {
				if (o == srcObj)break;
				// ���̐e��
				o = o->GetParent().get();
			}
			if (o == nullptr) {
				// �����̎q�ɂȂ�
				srcObj->SetParent(shared_from_this());
			}
		}
		ImGui::EndDragDropTarget();
	}


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
		// �V�K�쐬
		//------------------------
		if (ImGui::Selectable(u8"�V�KGameObject���쐬")) {
			KdSptr<GameObject> obj = std::make_shared<GameObject>();
			obj->Init();
			// ������e�Ƃ����ݒ�
			obj->SetParent(shared_from_this());

			// �I����Ԃɂ���
			GAMESYS.m_editorData.SelectObj = obj;

		}

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
		// ����
		//------------------------
		if (ImGui::Selectable(u8"����")) {
			// �����̐e
			auto parent = GetParent();
			if (parent) {

				// �������V���A���C�Y
				json11::Json::object serial;
				Serialize(serial);

				// GameObject���쐬�i�������m�ہj
				KdSptr<GameObject> obj = std::make_shared<GameObject>();

				// JSON�f�[�^�𗬂�����
				obj->Deserialize(serial);

				// �Z��Ƃ��Ď����̐e�ɓo�^
				obj->SetParent(parent);

				// �I����Ԃɂ���
				GAMESYS.m_editorData.SelectObj = obj;
			}
		}



		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 5));

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

					// �I����Ԃɂ���
					GAMESYS.m_editorData.SelectObj = obj;

				}
			}
		}

		ImGui::Dummy(ImVec2(0, 2));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 5));

		//------------------------
		// �폜
		//------------------------
		if (ImGui::Selectable(u8"�폜")) {
			Destroy();
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
