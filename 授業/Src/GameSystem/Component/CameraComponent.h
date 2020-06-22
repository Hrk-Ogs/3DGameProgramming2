#pragma once

//===========================================
//
// �J�����R���|�[�l���g
//
//===========================================
class CameraComponent :public BaseComponent
{
public:

	// ����p�擾
	float GetProjFOV()const { return m_proj_FOV; }
	// �ŋߐڋ����ƍŉ��������擾
	const KdVec2& GetProjNearFar()const { return m_proj_NearAndFar; }

	// �ˉe�s��擾
	void GetProjectionMatrix(KdMatrix& out);

	// �J���������V�F�[�_�[�փZ�b�g����
	void SetCamera();

	//============================

	// �`�揀������
	virtual void PrepareDraw(RenderingData& rdata)override;

	// (Editor�p)ImGui
	virtual void Editor_ImGuiUpdate()override;

	//============================
	// Serialize/Deserialize
	//============================
	//JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["FOV"], m_proj_FOV);
		KdJsonGet(jsonData["NearAndFar"], m_proj_NearAndFar);
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["FOV"] = m_proj_FOV;
		outJsonObj["NearAndFar"] = m_proj_NearAndFar.ToArray();
	}

private:

	// �ˉe�s��p�f�[�^
	float	m_proj_FOV = 60;		// ����p
	KdVec2	m_proj_NearAndFar = { 0.01f,5000 };	// �ŋߐڋ����ƍŉ�������
};
