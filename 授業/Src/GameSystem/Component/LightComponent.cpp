#include "../GameSystem.h"

#include "LightComponent.h"

//====================================================================
// �J�����̏�񂩂�A�҂�������܂�r���[�s��Ɛ��ˉe�s������߂�
// mView : �J�����̃r���[�s��
// mProj : �J�����̎ˉe�s��
// lightDir : ���C�g�̕���
// outView : [�o��]�Z�o�����r���[�s��
// outProj : [�o��]�Z�o�����ˉe�s��
//====================================================================
static void CreateLightViewProj(const KdMatrix& mView, const KdMatrix& mProj, const KdVec3& lightDir, KdMatrix& outView, KdMatrix& outProj)
{
	//-------------------------------------------
	// ���݂̎�����̊e�_(8���W)�����߂�
	// �ˉe���W�n���烏�[���h���W�n�֋t�ϊ�
	//-------------------------------------------
	KdMatrix camInvProj = mProj;
	camInvProj.Inverse();
	KdMatrix mCam = mView;
	mCam.Inverse();
	KdMatrix camInvPV = camInvProj * mCam;

	// -1�`1(Z��0�`1)�̎ˉe���W�n��BOX�f�[�^
	std::vector<KdVec3> PtTbl;
	PtTbl.push_back(KdVec3(1, 1, 0));
	PtTbl.push_back(KdVec3(-1, 1, 0));
	PtTbl.push_back(KdVec3(-1, -1, 0));
	PtTbl.push_back(KdVec3(1, -1, 0));
	PtTbl.push_back(KdVec3(1, 1, 1));
	PtTbl.push_back(KdVec3(-1, 1, 1));
	PtTbl.push_back(KdVec3(-1, -1, 1));
	PtTbl.push_back(KdVec3(1, -1, 1));
	// �ˉe���W�n�̎�����W���W�����[���h���W�n�֕ϊ�
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].TransformCoord(camInvPV);
	}
	// ���̃r���[�s��쐬(���W�͂܂�����Ȃ̂ŕ����̂�)
	KdMatrix lightView;
	lightView.LookTo(lightDir, { 0, 1, 0 });
	lightView.Inverse();

	// ������̂W�_�̍��W�����̃r���[���W�n�֕ϊ�
	for (UINT i = 0; i < PtTbl.size(); i++) {
		PtTbl[i].TransformCoord(lightView);
	}
	// �������AABB���Z�o
	KdVec3 vMin, vMax;
	vMin = PtTbl[0];
	vMax = PtTbl[0];
	for (UINT i = 1; i < PtTbl.size(); i++) {
		if (vMin.x > PtTbl[i].x)vMin.x = PtTbl[i].x;
		if (vMin.y > PtTbl[i].y)vMin.y = PtTbl[i].y;
		if (vMin.z > PtTbl[i].z)vMin.z = PtTbl[i].z;
		if (vMax.x < PtTbl[i].x)vMax.x = PtTbl[i].x;
		if (vMax.y < PtTbl[i].y)vMax.y = PtTbl[i].y;
		if (vMax.z < PtTbl[i].z)vMax.z = PtTbl[i].z;
	}

	// AABB����A�҂�������܂�傫���̐��ˉe�s��쐬
	outProj.CreateProjection_Orthographic(vMax.x - vMin.x, vMax.y - vMin.y, 0, vMax.z - vMin.z);
	// AABB�̒��S(X��Y)�̃r���[�s����쐬(������Z=0�ɂ���)
	KdVec3 vCenter = vMin + (vMax - vMin) * 0.5f;
	vCenter.z = vMin.z;
	lightView.Inverse();
	vCenter.TransformCoord(lightView);
	outView.CreateTranslation(vCenter);
	outView.LookTo(lightDir, { 0, 1, 0 });
	outView.Inverse();
}

void LightComponent::GenerateShadowMap(std::function<void()> proc)
{
	if (m_lightType == LightTypes::Spot)
	{
		
		// �L��
		ComPtr<ID3D11RenderTargetView> saveRTV;
		ComPtr<ID3D11DepthStencilView> saveDSV;
		D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

		// �[�x�N���b�s���O�𖳌��ɂ���
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack_NoZClip);

		// �����_�[�^�[�Q�b�g��Z�o�b�t�@��ύX
		SHADER.m_genShadowMapShader.SetRenderTarget_SpotShadowMap(m_myLightIndex);

		// �r���[�s��
		KdMatrix mV = GetOwner()->Transform()->GetMatrix();
		mV.Inverse();
		// �ˉe�s��
		KdMatrix mP;
		mP.CreateProjection_PerspectiveFov(m_spotAngle * KdToRadians, 1, 0.01f, m_range);
		SHADER.m_genShadowMapShader.SetLightVP(mV * mP);

		SHADER.m_cb8_Light.Work().SL[m_myLightIndex].mLightVP = mV * mP;
		SHADER.m_cb8_Light.Write();

		// �`����s
		SHADER.m_genShadowMapShader.SetToDevice();
		proc();

		// ���낢�댳�ɖ߂�
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack); // RS
		D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get()); // RT��Z�o�b�t�@
		D3D.SetViewportFromRTV(saveRTV.Get()); // �r���[�|�[�g
	}
	else if (m_lightType == LightTypes::Directional)
	{
		// �L��
		ComPtr<ID3D11RenderTargetView> saveRTV;
		ComPtr<ID3D11DepthStencilView> saveDSV;
		D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

		// �[�x�N���b�s���O�𖳌��ɂ���
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack_NoZClip);

		for (int ci = 0; ci < m_cascadeNum; ci++)
		{
			//�f�[�^�`�F�b�N
			if (m_cascadeDist[ci] <= 0.0001f)m_cascadeDist[ci] = 0.0001f;

			// �����_�[�^�[�Q�b�g��Z�o�b�t�@��ύX
			SHADER.m_genShadowMapShader.SetRenderTarget_DirShadowMap(m_myLightIndex, ci);

			// ���݃J�����̎ˉe�s�񂩂�A����p�A�A�X�y�N�g��A�ŋߐڋ����ȂǎZ�o����
			const KdMatrix& camP = SHADER.m_cb7_Camera.GetWork().mP;
			float aspect = camP._22 / camP._11; // �A�X�y�N�g��
			float fNear = camP._43 / -camP._33; // Near�N���b�v
			float fov = 2 * atan(1.0f / camP._22); // ����p
			// �P�Ԗڈȍ~�́A1�O�̋������g�p
			if (ci >= 1)
			{
				fNear = m_cascadeDist[ci - 1];
			}
			// Far�̕����������Ƃ܂���
			if (fNear >= m_cascadeDist[ci])m_cascadeDist[ci] = fNear + 0.01f;

			KdMatrix mTmpProj;
			// fNear �` 50m�܂ł̎ˉe�s����쐬
			mTmpProj.CreateProjection_PerspectiveFov(fov, aspect, fNear, m_cascadeDist[ci]);

			// ���̃J�����͈͂����S�ɕ����͂ރr���[�s��Ɛ��ˉe�s������߂�
			KdMatrix mV, mP;
			CreateLightViewProj(SHADER.m_cb7_Camera.GetWork().mV, mTmpProj, GetOwner()->Transform()->GetRotation().Forward(), mV, mP);

			SHADER.m_genShadowMapShader.SetLightVP(mV * mP);

			SHADER.m_cb8_Light.Work().DL[m_myLightIndex].CascadeDist = m_cascadeDist;
			SHADER.m_cb8_Light.Work().DL[m_myLightIndex].CascadeNum = m_cascadeNum;
			SHADER.m_cb8_Light.Work().DL[m_myLightIndex].mLightVP[ci] = mV * mP;
			SHADER.m_cb8_Light.Write();

			// �`����s
			SHADER.m_genShadowMapShader.SetToDevice();
			proc();
		}

		// ���낢�댳�ɖ߂�
		D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack); // RS
		D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get()); // RT��Z�o�b�t�@
		D3D.SetViewportFromRTV(saveRTV.Get()); // �r���[�|�[�g
	}
}

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
			SHADER.m_cb8_Light.Work().DL[idx].EnableShadow = m_isGenerateShadowMap ? 1.0f : 0.0f;

			// ���C�g�������Z
			SHADER.m_cb8_Light.Work().DL_Cnt++;

			// �V���h�E�}�b�v�o�^
			if (m_isGenerateShadowMap)
			{
				rdata.m_shadowMapLight.push_back(this);
			}
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
	// �X�|�b�g���C�g
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
			SHADER.m_cb8_Light.Work().SL[idx].EnableShadow = m_isGenerateShadowMap ? 1.0f : 0.0f;

			// ���C�g�������Z
			SHADER.m_cb8_Light.Work().SL_Cnt++;

			// �V���h�E�}�b�v�o�^
			if (m_isGenerateShadowMap)
			{
				rdata.m_shadowMapLight.push_back(this);
			}
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

	// �e����
	ImGui::Checkbox(u8"�e����", &m_isGenerateShadowMap);

	if (m_isGenerateShadowMap)
	{
		// ���s���̎��̓J�X�P�[�h�ݒ�
		if (m_lightType == LightTypes::Directional)
		{
			ImGui::SliderInt(u8"�J�X�P�[�h������", &m_cascadeNum, 1, 4);
			ImGui::DragFloat4(u8"�J�X�P�[�h��������", &m_cascadeDist.x, 0.01f, 0.01f, 10000);
		}
	}

	// �V���h�E�}�b�v�摜
	if (m_lightType == LightTypes::Directional && m_isGenerateShadowMap)
	{
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 0), ImVec2(200, 200));
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 1), ImVec2(200, 200));
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 2), ImVec2(200, 200));
		ImGui::Image((ImTextureID)SHADER.m_genShadowMapShader.GetDirShadowMap()->GetSRView(m_myLightIndex * 4 + 3), ImVec2(200, 200));
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

