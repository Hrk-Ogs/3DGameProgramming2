#include "GameSystem.h"

#include "EditorCamera.h"

void EditorCamera::Update()
{
	//============================
	// ビュー行列
	//============================
	POINT nowPos;
	GetCursorPos(&nowPos);

	// Shiftキーで比率を大きく
	float ratio = 1.0f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		ratio = 5;
	}

	// Ctrlキーで比率を小さく
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		ratio = 0.2f;
	}

	// 注視点からの距離により、操作量を変化
	float distanceRatio = m_mCam.Translation().Length() * 0.02f;

	// マウスの右ボタン
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		// マウスの前回からの移動量
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * 0.5f * ratio;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * 0.5f * ratio;

		// カメラ回転 Y軸回転
		m_mCam.RotateY(deltaX * KdToRadians);
		// カメラ回転 X軸回転
		m_mCam.RotateAxis(m_mCam.Right(), deltaY * KdToRadians);
	}

	// ホイールドラッグ
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
	{
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * 0.05f * ratio * distanceRatio;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * 0.05f * ratio * distanceRatio;
		m_vCamBasePos -= m_mCam.Right() * deltaX;
		m_vCamBasePos += m_mCam.Up() * deltaY;
	}

	// ホイール回転
	if (KdWindow::sGetMouseWheelVal())
	{
		m_mCam.Move(m_mCam.Forward() * (float)KdWindow::sGetMouseWheelVal() * 0.01f * ratio * distanceRatio);
	}
	// 現在の座標を記憶しておく
	m_prevMousePos = nowPos;
}


void EditorCamera::SetToShader()
{
	// カメラ行列からビュー行列へ変換
	Math::Matrix mCam = m_mCam;
	mCam *= DirectX::XMMatrixTranslation(m_vCamBasePos.x, m_vCamBasePos.y, m_vCamBasePos.z);

	// カメラの定数バッファにデータをセット
	SHADER.m_cb7_Camera.Work().CamPos = mCam.Translation();
	SHADER.m_cb7_Camera.Work().mV = mCam.Invert();
	SHADER.m_cb7_Camera.Work().mP = m_mProj;

	// セットカメラ情報(ビュー行列、射影行列など)をGPUバッファへ書き込む
	SHADER.m_cb7_Camera.Write();
}