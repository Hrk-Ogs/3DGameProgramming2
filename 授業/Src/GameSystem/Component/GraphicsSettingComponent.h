#pragma once

#include "BaseComponent.h"

//============================================
// �`��̏ڍאݒ�R���|�[�l���g
//============================================
class GraphicsSettingComponent : public BaseComponent
{
public:
	// �`�揀������
	virtual void PrepareDraw(RenderingData& rData) override
	{
		// IBL�e�N�X�`���Z�b�g
		D3D.GetDevContext()->PSSetShaderResources(11, 1, m_texIBL->GetSRViewAddress());

		// �����ݒ�
		SHADER.m_cb8_Light.Work().AmbientLight = m_ambientLight;
		
		// �����t�H�O�ݒ�
		SHADER.m_cb7_Camera.Work().DistanceFogEnable = 1;
		SHADER.m_cb7_Camera.Work().DistanceFogColor = m_distanceFogColor;
		SHADER.m_cb7_Camera.Work().DistanceFogDensity = m_distanceFogDensity;
	}


	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["IBLTexFilename"], m_texIBL);
		if (m_texIBL == nullptr)
		{
			// null�͔�����
			m_texIBL = std::make_shared<KdTexture>();
		}

		KdJsonGet(jsonData["AmbientLight"], m_ambientLight);
		KdJsonGet(jsonData["DistanceFogColor"], m_distanceFogColor);
		KdJsonGet(jsonData["DistanceFogDensity"], m_distanceFogDensity);
	}
	
	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["IBLTexFilename"] = m_texIBL->GetFilepath();
		outJsonObj["AmbientLight"] = m_ambientLight.ToArray();
		outJsonObj["DistanceFogColor"] = m_distanceFogColor.ToArray();
		outJsonObj["DistanceFogDensity"] = m_distanceFogDensity;
	}
	//
	virtual void Editor_ImGuiUpdate() override
	{
		BaseComponent::Editor_ImGuiUpdate();

		ImGui::Text(u8"IBL�e�N�X�`��");
		ImGuiSelectTexture(m_texIBL);
		if (m_texIBL == nullptr)
		{
			// ���I������null�I�u�W�F�N�g�����Ă���
			m_texIBL = std::make_shared<KdTexture>();
		}
		ImGui::ColorEdit3(u8"����", &m_ambientLight.x);
		ImGui::ColorEdit3(u8"�t�H�O�F", &m_distanceFogColor.x);
		ImGui::DragFloat(u8"�t�H�O���x", &m_distanceFogDensity, 0.0001f, 0, 1, "%.4f");
	}

private:
	// IBL�e�N�X�`��
	KdSptr<KdTexture> m_texIBL = std::make_shared<KdTexture>();

	// ����
	KdVec3 m_ambientLight = { 0.1f, 0.1f, 0.1f };

	// �����t�H�O
	KdVec3 m_distanceFogColor = { 1,1,1 };
	float m_distanceFogDensity = 0.01f;
};