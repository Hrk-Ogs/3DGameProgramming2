#include "../GameSystem.h"

#include "CameraComponent.h"

void CameraComponent::GetProjectionMatrix(KdMatrix& out)
{
	// 現在のビューボード取得
	Math::Viewport vp;
	D3D.GetViewport(vp);
	// 透視投影行列（射影行列）作成
	out.CreateProjection_PerspectiveFov(m_proj_FOV * KdToRadians, vp.AspectRatio(), m_proj_NearAndFar.x, m_proj_NearAndFar.y);
}

void CameraComponent::SetCamera()
{
	// 射影行列作成・セット
	GetProjectionMatrix(SHADER.m_cb7_Camera.Work().mP);

	// ビュー行列を作成・セット
	KdMatrix mCam = GetOwner()->Transform()->GetMatrix();	// Transformの行列をカメラとして使用
	mCam.NormalizeScale();	// 拡大があるとまずいので正規化しておく
	SHADER.m_cb7_Camera.Work().CamPos = mCam.Translation();
	SHADER.m_cb7_Camera.Work().mV = mCam;
	SHADER.m_cb7_Camera.Work().mV.Inverse();

	// 定数バッファ書き込み
	SHADER.m_cb7_Camera.Write();
}

void CameraComponent::PrepareDraw(RenderingData& rdata)
{
	if (m_enable == false)return;

	// 登録
	rdata.m_camera = this;
}

void CameraComponent::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();

	if (ImGui::CollapsingHeader(u8"視野設定", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat(u8"視野角", &m_proj_FOV, 1, 1, 179);

		ImGui::DragFloat2(u8"範囲", &m_proj_NearAndFar.x, 0.1f, 0.001f, 5000);
		// 範囲チェック
		if (m_proj_NearAndFar.x <= 0)m_proj_NearAndFar.x = 0.0001f;
		if (m_proj_NearAndFar.y < m_proj_NearAndFar.x) {
			m_proj_NearAndFar.y = m_proj_NearAndFar.x + 0.01f;
		}
	}

	// 視錐台のデバッグ線
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

		// 射影座標系からワールド座標系へ逆変換する
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
