#pragma once

//===================================
// ���f���R���|�[�l���g
// �E3D���f���̏�����`����s�����i
//===================================
class ModelComponent :public BaseComponent
{
public:

	//===============================
	// ���쏈��
	//===============================
	// �X�V����
	virtual void Update() override;
	// �`�揈��
	virtual void Draw() override;

	//===============================
	// ImGui����
	//===============================
	virtual void Editor_ImGuiUpdate() override;

private:

	// ���f���f�[�^ 3D���b�V�����f���̌��f�[�^
	// ���o�O�y���̂��߁A�K��nullptr�ɂȂ�Ȃ��悤�ɂ���
	KdSptr<KdModel>			m_model = std::make_shared<KdModel>();

	// �m�[�h�A�E�g���C�i�[�@�m�[�h�i�{�[���j�̈������
	KdNodeOutliner			m_nodeOL;

	// �A�j���[�^�[
	KdAnimator				m_anim;


};
