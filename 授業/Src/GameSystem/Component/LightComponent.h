#pragma once

//======================================
//
// ���C�g�R���|�[�l���g
//
//======================================
class LightComponent :public BaseComponent
{
public:

	//========================
	// �R���|�[�l���g����
	//========================

	// �`�揀��
	virtual void PrepareDraw(RenderingData& rdata)override;

	//(Editor�p�jImGui
	virtual void Editor_ImGuiUpdate()override;

	//========================
	// Serialize/Deserialize
	//========================
	//JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["LightType"], (int&)m_lightType);
		KdJsonGet(jsonData["Color"], m_color);
		KdJsonGet(jsonData["Range"], m_range);
		KdJsonGet(jsonData["SpotAngle"], m_spotAngle);
		KdJsonGet(jsonData["Intensity"], m_intensity);
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["LightType"] = (int)m_lightType;
		outJsonObj["Color"] = m_color.ToArray();
		outJsonObj["Range"] = m_range;
		outJsonObj["SpotAngle"] = m_spotAngle;
		outJsonObj["Intensity"] = m_intensity;

	}


private:

	// ���C�g�̎�ޒ萔
	enum class LightTypes {
		Directional = 0,		// ���s��
		Point=1,				// �_��
		Spot=2,					// �W����
	};
	// ���C�g�̎�ށ@0:���s���C�g�@1:�|�C���g���C�g 2�F�X�|�b�g���C�g
	LightTypes m_lightType = LightTypes::Point;

	KdVec3	m_color = { 1,1,1 };	// �F
	float	m_range = 5;			// �_���͈̔�
	float	m_spotAngle = 30;		//�i�X�|�b�g���C�g��p�j�p�x
	float	m_intensity = 2.0f;		//���x
	
	// ���s���A���݂̃��C�g�ԍ�
	int		m_myLightIndex = -1;
};
