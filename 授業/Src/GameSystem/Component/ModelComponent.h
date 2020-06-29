#pragma once

//===================================
// ���f���R���|�[�l���g
// �E3D���f���̏�����`����s�����i
//===================================
class ModelComponent :public BaseComponent
{
public:

	// ���f���f�[�^���Z�b�g
	// �m�[�h�A�E�g���C�i�[��������A�j���[�^�[�������Ȃǂ��s��
	void SetModel(const KdSptr<KdModel>& model);

	// ���f���f�[�^�擾
	KdSptr<KdModel> GetModel()const { return m_model; }

	// �m�[�h�A�E�g���C�i�[�擾
	KdNodeOutliner& NodeOutliner() { return m_nodeOL; }

	// �A�j���[�^�擾
	KdAnimator& Animator() { return m_animator; }

	//===============================
	// ���쏈��
	//===============================
	// �X�V����
	virtual void Update() override;
	// �`�揀������
	virtual void PrepareDraw(RenderingData& rdate) override;
	// �`�揈��
	// phaseID�@�c�c�ǂ̃^�C�~���O�Ŏ��s����Ă��邩�̃t���O
	virtual void Draw(int phaseID) override;

	//===============================
	// �V���A���C�Y/�f�V���A���C�Y
	//===============================
	// JSON�f�[�^���畜��
	virtual void Deserialize(const json11::Json& jsonObj)override
	{
		// �p�����̊֐������s����
		BaseComponent::Deserialize(jsonObj);

		KdJsonGet(jsonObj["ModelFilename"], m_model);
		KdJsonGet(jsonObj["StartAnimeName"], m_startAnimeName);
		KdJsonGet(jsonObj["StartAnimatioinLoop"], m_startAnimationLoop);
		KdJsonGet(jsonObj["AnimationSpeed"], m_animationSpeed);

		KdJsonGet(jsonObj["Visible"], m_visible);

		// ���f���f�[�^�̃Z�b�g
		SetModel(m_model);
	}

	// JSON�f�[�^��
	virtual void Serialize(json11::Json::object& outJsonObj)const override
	{
		// �p�����̊֐������s����
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["ModelFilename"] = m_model->GetFilepath();

		outJsonObj["StartAnimeName"] = m_startAnimeName;
		outJsonObj["StartAnimatioinLoop"] = m_startAnimationLoop;
		outJsonObj["AnimationSpeed"] = m_animationSpeed;
		outJsonObj["Visible"] = m_visible;
	}

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
	KdAnimator				m_animator;
	std::string				m_startAnimeName;
	bool					m_startAnimationLoop = true;
	float					m_animationSpeed = 1.0f;

	// �`��ݒ�
	bool					m_visible = true;

};
