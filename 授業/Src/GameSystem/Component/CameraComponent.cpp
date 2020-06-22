#include "../GameSystem.h"

#include "CameraComponent.h"

void CameraComponent::GetProjectionMatrix(KdMatrix& out)
{
	// ���݂̃r���[�{�[�h�擾
	Math::Viewport vp;
	D3D.GetViewport(vp);
	// �������e�s��i�ˉe�s��j�쐬
	out.CreateProjection_PerspectiveFov(m_proj_FOV * KdToRadians, vp.AspectRatio(), m_proj_NearAndFar.x, m_proj_NearAndFar.y);
}

void CameraComponent::SetCamera()
{
	// �ˉe�s��쐬�E�Z�b�g
	GetProjectionMatrix(SHADER.m_cb7_Camera.Work().mP);

	// �r���[�s����쐬�E�Z�b�g
	KdMatrix mCam = GetOwner()->Transform()->GetMatrix();	// Transform�̍s����J�����Ƃ��Ďg�p
	mCam.NormalizeScale();	// �g�傪����Ƃ܂����̂Ő��K�����Ă���
	SHADER.m_cb7_Camera.Work().CamPos = mCam.Translation();
	SHADER.m_cb7_Camera.Work().mV = mCam;
	SHADER.m_cb7_Camera.Work().mV.Inverse();

	// �萔�o�b�t�@��������
	SHADER.m_cb7_Camera.Write();
}

void CameraComponent::PrepareDraw(RenderingData& rdata)
{
	if (m_enable == false)return;

	// �o�^
	rdata.m_camera = this;
}

void CameraComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	if (ImGui::CollapsingHeader(u8"����ݒ�", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat(u8"����p", &m_proj_FOV, 1, 1, 179);

		ImGui::DragFloat2(u8"�͈�", &m_proj_NearAndFar.x, 0.1f, 0.001f, 5000);
		// �͈̓`�F�b�N
		if (m_proj_NearAndFar.x <= 0)m_proj_NearAndFar.x = 0.0001f;
		if (m_proj_NearAndFar.y < m_proj_NearAndFar.x) {
			m_proj_NearAndFar.y = m_proj_NearAndFar.x + 0.01f;
		}
	}

	// ������̃f�o�b�O��
	{
		KdVec3 points[] = {
			{-1,-1,0},
			{-1, 1,0},
			{ 1, 1,0},
			{ 1,-1,0},

			{-1,-1,1},
			{-1, 1,1},
			{ 1, 1,1},
			{ 1,-1,1},
		};

		KdMatrix mInvProj;
		GetProjectionMatrix(mInvProj);
		mInvProj.Inverse();

		// �ˉe���W�n���烏�[���h���W�n�֋t�ϊ�����
		KdMatrix m = mInvProj * GetOwner()->Transform()->GetMatrix();
		for (auto&& p : points) {
			p.TransformCoord(m);
		}

		KdVec4 color = { 1,1,0,0.3f };
		GAMESYS.DebugLine().AddLine(points[0],points[1],color);
		GAMESYS.DebugLine().AddLine(points[1],points[2],color);
		GAMESYS.DebugLine().AddLine(points[2],points[3],color);
		GAMESYS.DebugLine().AddLine(points[3],points[4],color);

		GAMESYS.DebugLine().AddLine(points[4],points[5],color);
		GAMESYS.DebugLine().AddLine(points[5],points[6],color);
		GAMESYS.DebugLine().AddLine(points[6],points[7],color);
		GAMESYS.DebugLine().AddLine(points[7],points[4],color);

		GAMESYS.DebugLine().AddLine(points[0],points[4],color);
		GAMESYS.DebugLine().AddLine(points[1],points[5],color);
		GAMESYS.DebugLine().AddLine(points[2],points[6],color);
		GAMESYS.DebugLine().AddLine(points[3],points[7],color);
	}
}
