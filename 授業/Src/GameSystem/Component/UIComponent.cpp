#include "../GameSystem.h"

#include "UIComponent.h"

void UISubjectComponent::Start()
{
	// 全UIObserverComponentを検索
	std::vector<KdSptr<UIObserverComponent>> results;
	GAMESYS.GetLevel()->GetRootObject()->GetComponents<UIObserverComponent>(results, true);

	// 全部記憶しておく
	m_observers.clear();
	for (auto&& comp : results)
	{
		m_observers.push_back(comp);
	}
}

void UIButtonComponent::Update()
{
	// 動作可能チェック
	if (IsEnable() == false)return;
	if (GAMESYS.IsPlay() == false)return;

	// 今回はSpriteComponentをボタン画像として使用する
	auto sprite = GetOwner()->GetComponent<SpriteComponent>();
	if (sprite == nullptr)
	{
		GAMESYS.m_editorData.LogWindow.AddLog(u8"[UIButton]Spriteがありません");
		return;
	}

	// マウスが画像の上に乗っている？
	if (sprite->IsHovered())
	{
		// クリック判定
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			// １回のみクリックするように制御
			if (m_clickFlag == false)
			{
				m_clickFlag = true;
				// 全オブザーバーへ通知
				for (auto&& o : m_observers)
				{
					auto obs = o.lock();
					if (obs)
					{
						// Observerへ通知
						obs->NotifyClicked(GetOwner(), m_onClickedMessage);
					}
				}
			}
		}
		else
		{
			m_clickFlag = false;
		}
	}

}
