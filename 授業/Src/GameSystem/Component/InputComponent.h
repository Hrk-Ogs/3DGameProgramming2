#pragma once

#include "BaseComponent.h"

//=============================
// �Q�[���L�[�萔
//=============================
// ���p�萔
enum class GameAxes
{
	L, // ���̎�
	R, // �E�̎�
};
// �{�^���p�萔
enum class GameButtons
{
	A,
	B,
	X,
	Y,
	L1,
	L2,
	R1,
	R2,
	Start,
};
//=====================================================
//
// ���̓R���|�[�l���g��{�N���X
//
//=====================================================
class InputComponent : public BaseComponent
{
public:
	// ���擾
	const KdVec2& GetAxis(GameAxes axis) const { return m_axes[(int)axis]; }
	// �{�^������
	bool IsButtonPressed(GameButtons button) const
	{
		return m_buttonFlags[(int)button] >= 1;
	}
	// �{�^������
	void PressButton(GameButtons button)
	{
		m_buttonFlags[(int)button]++;
	}
protected:
	// ��
	std::array<KdVec2, 2> m_axes;
	// �{�^���t���O
	std::array<int, 20> m_buttonFlags;
};


//=====================================================
//
// �v���C���[�̓��͂��g�p���āA����{�^����ݒ肷��R���|�[�l���g
//
//=====================================================
class PlayerInputComponent : public InputComponent
{
public:
	// �X�V����
	virtual void Update() override
	{
		// ���Z�b�g
		m_axes.fill({ 0, 0 });
		m_buttonFlags.fill(0);
		// �����̏ꍇ�̓X�L�b�v
		if (m_enable == false)return;
		// �㉺���E�L�[���玲��ݒ�
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) m_axes[(int)GameAxes::L].x = -1;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) m_axes[(int)GameAxes::L].x = 1;
		if (GetAsyncKeyState(VK_UP) & 0x8000) m_axes[(int)GameAxes::L].y = 1;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) m_axes[(int)GameAxes::L].y = -1;
		// AWDS�L�[���E��
		// ���}�E�X�̈ړ��ʂƂ��ɕύX���Ă��ǂ�
		if (GetAsyncKeyState('A') & 0x8000) m_axes[(int)GameAxes::R].x = -1;
		if (GetAsyncKeyState('D') & 0x8000) m_axes[(int)GameAxes::R].x = 1;
		if (GetAsyncKeyState('W') & 0x8000) m_axes[(int)GameAxes::R].y = 1;
		if (GetAsyncKeyState('S') & 0x8000) m_axes[(int)GameAxes::R].y = -1;
		// �L�[��Ԃ���{�^���t���O��ݒ�
		if (GetAsyncKeyState('Z') & 0x8000) PressButton(GameButtons::A);
		if (GetAsyncKeyState('X') & 0x8000) PressButton(GameButtons::B);
		if (GetAsyncKeyState('C') & 0x8000) PressButton(GameButtons::X);
		if (GetAsyncKeyState('V') & 0x8000) PressButton(GameButtons::Y);
		if (GetAsyncKeyState('Q') & 0x8000) PressButton(GameButtons::L1);
		if (GetAsyncKeyState('1') & 0x8000) PressButton(GameButtons::L2);
		if (GetAsyncKeyState('E') & 0x8000) PressButton(GameButtons::R1);
		if (GetAsyncKeyState('2') & 0x8000) PressButton(GameButtons::R2);
	}
};

//=====================================================
//
// ���o�J����AI���̓R���|�[�l���g
//
//=====================================================
class SimpleAIInputComponent : public InputComponent
{
public:
	virtual void Update() override
	{
		// �{�^�����Z�b�g
		m_buttonFlags.fill(0);
		if (m_enable == false)return;
		// �����_���ŕ����L�[���������藣������
		if (KdMTRand::s_rnd.GetInt(200) == 0)
		{
			m_axes[(int)GameAxes::L].y = 1;
		}

		if (KdMTRand::s_rnd.GetInt(50) == 0)
		{
			m_axes[(int)GameAxes::L].y = 0;
		}
		if (KdMTRand::s_rnd.GetInt(200) == 0)
		{
			m_axes[(int)GameAxes::L].y = -1;
		}
		if (KdMTRand::s_rnd.GetInt(200) == 0)
		{
			m_axes[(int)GameAxes::L].x = 1;
		}
		if (KdMTRand::s_rnd.GetInt(200) == 0)
		{
			m_axes[(int)GameAxes::L].x = -1;
		}
		// �����_���Ń{�^��������
		if (KdMTRand::s_rnd.GetInt(100) == 0)
		{
			PressButton(GameButtons::A);
		}
		if (KdMTRand::s_rnd.GetInt(100) == 0)
		{
			PressButton(GameButtons::B);
		}
	}
};