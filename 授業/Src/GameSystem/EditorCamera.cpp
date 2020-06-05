#include "GameSystem.h"

#include "EditorCamera.h"

void EditorCamera::Update()
{
	//============================
	// �r���[�s��
	//============================
	POINT nowPos;
	GetCursorPos(&nowPos);

	// Shift�L�[�Ŕ䗦��傫��
	float ratio = 1.0f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		ratio = 5;
	}

	// Ctrl�L�[�Ŕ䗦��������
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		ratio = 0.2f;
	}

	// �����_����̋����ɂ��A����ʂ�ω�
	float distanceRatio = m_mCam.Translation().Length() * 0.02f;

	// �}�E�X�̉E�{�^��
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		// �}�E�X�̑O�񂩂�̈ړ���
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * 0.5f * ratio;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * 0.5f * ratio;

		// �J������] Y����]
		m_mCam.RotateY(deltaX * KdToRadians);
		// �J������] X����]
		m_mCam.RotateAxis(m_mCam.Right(), deltaY * KdToRadians);
	}

	// �z�C�[���h���b�O
	if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
	{
		float deltaX = (float)(nowPos.x - m_prevMousePos.x) * 0.05f * ratio * distanceRatio;
		float deltaY = (float)(nowPos.y - m_prevMousePos.y) * 0.05f * ratio * distanceRatio;
		m_vCamBasePos -= m_mCam.Right() * deltaX;
		m_vCamBasePos += m_mCam.Up() * deltaY;
	}

	// �z�C�[����]
	if (KdWindow::sGetMouseWheelVal())
	{
		m_mCam.Move(m_mCam.Forward() * (float)KdWindow::sGetMouseWheelVal() * 0.01f * ratio * distanceRatio);
	}
	// ���݂̍��W���L�����Ă���
	m_prevMousePos = nowPos;
}


void EditorCamera::SetToShader()
{
	// �J�����s�񂩂�r���[�s��֕ϊ�
	Math::Matrix mCam = m_mCam;
	mCam *= DirectX::XMMatrixTranslation(m_vCamBasePos.x, m_vCamBasePos.y, m_vCamBasePos.z);

	// �J�����̒萔�o�b�t�@�Ƀf�[�^���Z�b�g
	SHADER.m_cb7_Camera.Work().CamPos = mCam.Translation();
	SHADER.m_cb7_Camera.Work().mV = mCam.Invert();
	SHADER.m_cb7_Camera.Work().mP = m_mProj;

	// �Z�b�g�J�������(�r���[�s��A�ˉe�s��Ȃ�)��GPU�o�b�t�@�֏�������
	SHADER.m_cb7_Camera.Write();
}