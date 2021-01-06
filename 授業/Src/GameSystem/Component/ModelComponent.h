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

	// �A�j���[�V�����X�N���v�g���Ɏ��s�����֐����Z�b�g
	void SetAnimationScriptProc(std::function<void(const KdKeyScript&)> scriptProc)
	{
		m_onAnimeScriptProc = scriptProc;
	}

	// ���[�g���[�V����On/Off�ݒ�
	void SetEnableRootMotion(bool enable)
	{
		m_enableRootMotion = enable;
	}

	// �֊sOn/Off
	void SetOutlineEnable(bool enable)
	{
		m_enableOutline = enable;
	}

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
		KdJsonGet(jsonObj["EnableRootMotion"], m_enableRootMotion);

		KdJsonGet(jsonObj["Visible"], m_visible);
		KdJsonGet(jsonObj["LightEnable"], m_lightEnable);
		KdJsonGet(jsonObj["FogEnable"], m_fogEnable);
		KdJsonGet(jsonObj["EnableOutline"], m_enableOutline);
		KdJsonGet(jsonObj["UseWaveNormalMap"], m_useWaveNormalMap);
		KdJsonGet(jsonObj["MulMaterialColor"], m_mulMaterialColor);
		KdJsonGet(jsonObj["UvOffset"], m_uvOffset);
		KdJsonGet(jsonObj["UvTiling"], m_uvTiling);
		KdJsonGet(jsonObj["RefractiveIndex"], m_refractiveIndex);
		KdJsonGet(jsonObj["CastShadow"], m_castShadow);

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
		outJsonObj["EnableRootMotion"] = m_enableRootMotion;
		outJsonObj["Visible"] = m_visible;
		outJsonObj["LightEnable"] = m_lightEnable;
		outJsonObj["FogEnable"] = m_fogEnable;
		outJsonObj["EnableOutline"] = m_enableOutline;
		outJsonObj["UseWaveNormalMap"] = m_useWaveNormalMap;
		outJsonObj["MulMaterialColor"] = m_mulMaterialColor.ToArray();
		outJsonObj["UvOffset"] = m_uvOffset.ToArray();
		outJsonObj["UvTiling"] = m_uvTiling.ToArray();
		outJsonObj["RefractiveIndex"] = m_refractiveIndex;
		outJsonObj["CastShadow"] = m_castShadow;
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

	// �A�j���[�V�����X�N���v�g�R�[���o�b�N�֐��I�u�W�F�N�g
	std::function<void(const KdKeyScript&)> m_onAnimeScriptProc;

	// �A�j���[�^�[
	KdAnimator				m_animator;
	std::string				m_startAnimeName;
	bool					m_startAnimationLoop = true;
	float					m_animationSpeed = 1.0f;
	bool					m_enableRootMotion = false;	// ���[�g���[�V�����L��

	// �`��ݒ�
	bool					m_visible = true;
	bool					m_lightEnable = true;		// ���C�g�L��
	bool					m_fogEnable = true;			// �t�H�O�L��
	bool					m_enableOutline = false;	// �֊s�L��
	bool m_useWaveNormalMap = false; // �g�@���}�b�v���g�p����
	KdVec4 m_mulMaterialColor = { 1,1,1,1 }; // ��Z�F
	KdVec2 m_uvOffset = { 0, 0 }; // UV�I�t�Z�b�g
	KdVec2 m_uvTiling = { 1, 1 }; // UV�^�C�����O
	float m_refractiveIndex = 1.0f; // ���ܗ�
	bool m_castShadow = true; // �e�𐶐�����
};
