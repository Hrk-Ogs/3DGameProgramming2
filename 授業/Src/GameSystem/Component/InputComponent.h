#pragma once

#include "BaseComponent.h"

//=============================
// ゲームキー定数
//=============================
// 軸用定数
enum class GameAxes
{
	L, // 左の軸
	R, // 右の軸
};
// ボタン用定数
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
// 入力コンポーネント基本クラス
//
//=====================================================
class InputComponent : public BaseComponent
{
public:
	// 軸取得
	const KdVec2& GetAxis(GameAxes axis) const { return m_axes[(int)axis]; }
	// ボタン判定
	bool IsButtonPressed(GameButtons button) const
	{
		return m_buttonFlags[(int)button] >= 1;
	}
	// ボタン押す
	void PressButton(GameButtons button)
	{
		m_buttonFlags[(int)button]++;
	}
protected:
	// 軸
	std::array<KdVec2, 2> m_axes;
	// ボタンフラグ
	std::array<int, 20> m_buttonFlags;
};


//=====================================================
//
// プレイヤーの入力を使用して、軸やボタンを設定するコンポーネント
//
//=====================================================
class PlayerInputComponent : public InputComponent
{
public:
	// 更新処理
	virtual void Update() override
	{
		// リセット
		m_axes.fill({ 0, 0 });
		m_buttonFlags.fill(0);
		// 無効の場合はスキップ
		if (m_enable == false)return;
		// 上下左右キーから軸を設定
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) m_axes[(int)GameAxes::L].x = -1;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) m_axes[(int)GameAxes::L].x = 1;
		if (GetAsyncKeyState(VK_UP) & 0x8000) m_axes[(int)GameAxes::L].y = 1;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) m_axes[(int)GameAxes::L].y = -1;
		// AWDSキーが右軸
		// ※マウスの移動量とかに変更しても良い
		if (GetAsyncKeyState('A') & 0x8000) m_axes[(int)GameAxes::R].x = -1;
		if (GetAsyncKeyState('D') & 0x8000) m_axes[(int)GameAxes::R].x = 1;
		if (GetAsyncKeyState('W') & 0x8000) m_axes[(int)GameAxes::R].y = 1;
		if (GetAsyncKeyState('S') & 0x8000) m_axes[(int)GameAxes::R].y = -1;
		// キー状態からボタンフラグを設定
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
// おバカさんAI入力コンポーネント
//
//=====================================================
class SimpleAIInputComponent : public InputComponent
{
public:
	virtual void Update() override
	{
		// ボタンリセット
		m_buttonFlags.fill(0);
		if (m_enable == false)return;
		// ランダムで方向キーを押したり離したり
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
		// ランダムでボタンを押す
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