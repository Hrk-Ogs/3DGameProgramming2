#include "../GameSystem.h"

#include "UIComponent.h"

void UISubjectComponent::Start()
{
	// �SUIObserverComponent������
	std::vector<KdSptr<UIObserverComponent>> results;
	GAMESYS.GetLevel()->GetRootObject()->GetComponents<UIObserverComponent>(results, true);

	// �S���L�����Ă���
	m_observers.clear();
	for (auto&& comp : results)
	{
		m_observers.push_back(comp);
	}
}

void UIButtonComponent::Update()
{
	// ����\�`�F�b�N
	if (IsEnable() == false)return;
	if (GAMESYS.IsPlay() == false)return;

	// �����SpriteComponent���{�^���摜�Ƃ��Ďg�p����
	auto sprite = GetOwner()->GetComponent<SpriteComponent>();
	if (sprite == nullptr)
	{
		GAMESYS.m_editorData.LogWindow.AddLog(u8"[UIButton]Sprite������܂���");
		return;
	}

	// �}�E�X���摜�̏�ɏ���Ă���H
	if (sprite->IsHovered())
	{
		// �N���b�N����
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			// �P��̂݃N���b�N����悤�ɐ���
			if (m_clickFlag == false)
			{
				m_clickFlag = true;
				// �S�I�u�U�[�o�[�֒ʒm
				for (auto&& o : m_observers)
				{
					auto obs = o.lock();
					if (obs)
					{
						// Observer�֒ʒm
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
