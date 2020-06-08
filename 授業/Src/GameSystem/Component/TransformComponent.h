#pragma once

//===================================
// �g�����X�t�H�[���R���|�[�l���g
// �EGameObject�̎p����S������
//===================================
class TransformComponent :public BaseComponent
{
public:

	// ImGui����
	virtual void Editor_ImGuiUpdate() override;

	//================================
	// �V���A���C�Y/�f�V���A���C�Y
	//================================
	virtual void Deserialize(const json11::Json& jsonObj);
	virtual void Serialize(json11::Json::object& outJsonObj)const;

	//================================
	// �ݒ�E�擾
	//================================

	// ���[���h�s��擾
	const KdMatrix& GetMatrix()const { return m_worldMatrix; }


private:

	// ���[���h�s��
	KdMatrix		m_worldMatrix;

};
