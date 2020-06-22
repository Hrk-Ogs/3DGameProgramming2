#include "../GameSystem.h"

#include "LightComponent.h"

void LightComponent::PrepareDraw(RenderingData& rdata)
{
	if (m_enable == false)return;

	m_myLightIndex = -1;

	//-----------------
	// ���C�g�f�[�^�o�^
	//-----------------
	// �f�B���N�V���i�����C�g
	if (m_lightType == LightTypes::Directional) {
		// ���݂̓o�^��
		int idx = SHADER.m_cb8_Light.Work().DL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().DL.size()) {
			m_myLightIndex = idx;	// �����̃��C�g�ԍ�

			KdMatrix m = GetOwner()->Transform()->GetMatrix();

			// ���C�g�f�[�^��萔�o�b�t�@�֏�������
			SHADER.m_cb8_Light.Work().DL[idx].Dir = m.Forward();
			SHADER.m_cb8_Light.Work().DL[idx].Dir.Normalize();
			SHADER.m_cb8_Light.Work().DL[idx].Color = m_color * m_intensity;

			// ���C�g�������Z
			SHADER.m_cb8_Light.Work().DL_Cnt++;
		}
	}
	// �|�C���g���C�g
	else if (m_lightType == LightTypes::Point) {
		KdMatrix m = GetOwner()->Transform()->GetMatrix();

		// ���݂̓o�^��
		int idx = SHADER.m_cb8_Light.Work().PL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().PL.size())	// ���E���𒴂��Ă���ꍇ�́A���̃��C�g�͎g�p���Ȃ�
		{
			m_myLightIndex = idx;

			// ���C�g�f�[�^��萔�o�b�t�@�֏�������
			SHADER.m_cb8_Light.Work().PL[idx].Pos = m.Translation();
			SHADER.m_cb8_Light.Work().PL[idx].Radius = m_range;
			SHADER.m_cb8_Light.Work().PL[idx].Color = m_color * m_intensity;

			// ���C�g�������Z
			SHADER.m_cb8_Light.Work().PL_Cnt++;
		}
	}
	else if(m_lightType==LightTypes::Spot){
		KdMatrix m = GetOwner()->Transform()->GetMatrix();

		// ���݂̓o�^��
		int idx = SHADER.m_cb8_Light.Work().SL_Cnt;
		if (idx < (int)SHADER.m_cb8_Light.Work().SL.size()) {
			m_myLightIndex = idx;		// �����̃��C�g�ԍ�

			// ���C�g�f�[�^��萔�o�b�t�@�֏�������
			SHADER.m_cb8_Light.Work().SL[idx].Pos = m.Translation();
			SHADER.m_cb8_Light.Work().SL[idx].Dir = m.Forward();
			SHADER.m_cb8_Light.Work().SL[idx].Dir.Normalize();
			SHADER.m_cb8_Light.Work().SL[idx].Range = m_range;
			SHADER.m_cb8_Light.Work().SL[idx].Color = m_color * m_intensity;
			SHADER.m_cb8_Light.Work().SL[idx].OuterCorn = m_spotAngle * KdToRadians;

			// ���C�g�������Z
			SHADER.m_cb8_Light.Work().SL_Cnt++;
		}
	}
}

void LightComponent::Editor_ImGuiUpdate()
{
	// �p���O�̏��������s
	BaseComponent::Editor_ImGuiUpdate();

	// ���C�g�̎��
	std::vector<char*> lightTypes = { u8"Directional",u8"Point",u8"Spot" };
	ImGui::Combo(u8"���", (int*)&m_lightType, &lightTypes[0], lightTypes.size());
	// �F
	ImGui::ColorEdit3(u8"Color", &m_color.x);
	// ���x
	ImGui::DragFloat(u8"Intensity", &m_intensity, 0.01f, 0, 100);

	// �͈́i�_���ƏW�����݂̂̐ݒ�)
	if (m_lightType == LightTypes::Point || m_lightType == LightTypes::Spot) {
		ImGui::DragFloat(u8"Range", &m_range, 0.01f, 0, 10000);
	}
	// �p�x�i�W�����݂̂̐ݒ�j
	if (m_lightType == LightTypes::Spot) {
		ImGui::DragFloat(u8"Angle", &m_spotAngle, 0, 0, 360);
	}

	//================
	// �f�o�b�O��
	//================
	// �f�B���N�V���i�����C�g
	KdMatrix m = GetOwner()->Transform()->GetMatrix();
	if (m_lightType == LightTypes::Directional) {
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
		m = KdMatrix().CreateTranslation(0.1f, 0, 0) * m;
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
		m = KdMatrix().CreateTranslation(-0.2f, 0, 0) * m;
		GAMESYS.DebugLine().AddLine(m.Translation(), m.Translation() + m.Forward() * 0.5f, { 1,1,0,1 });
	}
	else if (m_lightType == LightTypes::Point) {
		// �f�o�b�O��
		GAMESYS.DebugLine().AddSphereLine(m.Translation(), m_range, m_color.ToVec4(0.5f));
	}

}
