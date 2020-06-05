#pragma once

//======================================================
//
// マウスでぐりぐり動かせるカメラ
//
//======================================================
class EditorCamera 
{
public:
	// 注視点座標を設定
	void SetBasePos(const KdVec3& pos) { m_vCamBasePos = pos; }
	//
	EditorCamera()
	{
		// 初期カメラ行列
		m_mCam.CreateTranslation(0, 0, -5);
		m_mCam.RotateX(20 * KdToRadians);
		m_mCam.RotateY(10 * KdToRadians);
		// 初期射影行列
		m_mProj.CreateProjection_PerspectiveFov(60 * KdToRadians, D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 5000);
	}

	// 処理
	void Update();
	
	// ビュー・射影行列をセット
	void SetToShader();

private:

	// 注視点座標
	KdVec3 m_vCamBasePos;
	// カメラ行列
	KdMatrix m_mCam;
	// 射影行列
	KdMatrix m_mProj;

	POINT m_prevMousePos = { 0, 0 }; // 前回のマウス位置
};