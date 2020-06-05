#pragma once

//======================================================
//
// �}�E�X�ł��肮�蓮������J����
//
//======================================================
class EditorCamera 
{
public:
	// �����_���W��ݒ�
	void SetBasePos(const KdVec3& pos) { m_vCamBasePos = pos; }
	//
	EditorCamera()
	{
		// �����J�����s��
		m_mCam.CreateTranslation(0, 0, -5);
		m_mCam.RotateX(20 * KdToRadians);
		m_mCam.RotateY(10 * KdToRadians);
		// �����ˉe�s��
		m_mProj.CreateProjection_PerspectiveFov(60 * KdToRadians, D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 5000);
	}

	// ����
	void Update();
	
	// �r���[�E�ˉe�s����Z�b�g
	void SetToShader();

private:

	// �����_���W
	KdVec3 m_vCamBasePos;
	// �J�����s��
	KdMatrix m_mCam;
	// �ˉe�s��
	KdMatrix m_mProj;

	POINT m_prevMousePos = { 0, 0 }; // �O��̃}�E�X�ʒu
};