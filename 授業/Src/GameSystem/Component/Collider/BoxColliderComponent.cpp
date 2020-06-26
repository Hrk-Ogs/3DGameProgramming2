#include "../../GameSystem.h"

#include "BoxColliderComponent.h"

#include "CollisionManager.h"

bool BoxColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// Box vs ��
	return CollisionFunctions::SphereToBox(*sph, *this);
}

bool BoxColliderComponent::HitTest(RayColliderComponent* ray)
{
	// Box vs ���C
	return CollisionFunctions::RayToBox(*ray, *this);
}

bool BoxColliderComponent::HitTest(BoxColliderComponent* box)
{
	// Box vs Box
	return CollisionFunctions::BoxToBox(*this, *box);
}

bool BoxColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// Box�ƃ��b�V������́A��₱�����̂ō���͖���
	return false;
}

void BoxColliderComponent::UpdateCollisionData()
{
	// �X�V���Ȃ��t���O�̎��̓X�L�b�v
	if (m_updateCollisionData == false)return;

	//--------------------------------
	// ����f�[�^�X�V
	//--------------------------------
	if (GetOwner())
	{
		auto& worldMat = GetOwner()->Transform()->GetMatrix();

		//-----------------------------------------------
		// ���[�J���̔��f�[�^���A�s��Ń��[���h�֕ϊ�����
		//-----------------------------------------------

		//���[�J�����W�����[���h���W�ɕϊ�
		m_localBox.Transform(m_worldBox, worldMat);
	}
}

void BoxColliderComponent::Update()
{
	// �������̓X�L�b�v
	if (IsEnable() == false)return;
	// ���s���̂�
//	if (GAMESYS.IsPlay())return;

	//--------------------------------
	// �o�^
	//--------------------------------
	Register();
}

void BoxColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// �������̓X�L�b�v
	if (IsEnable() == false)return;

	// �f�o�b�O�\��
	if (ColliderComponent::s_showDebugLine)
	{
		// ��
		GAMESYS.DebugLine().AddBoxLine(m_worldBox.Center, m_worldBox.Extents, m_worldBox.Orientation, ColliderComponent::s_debugLineColor);

		// �q�b�g���
		for (auto&& res : m_resultList)
		{
			for (auto&& pt : res.HitPosList)
			{
				GAMESYS.DebugLine().AddCrossLine(pt, 0.1f, { 1,0,0,1 });
			}
		}
	}
}

void BoxColliderComponent::Editor_ImGuiUpdate()
{
	// 
	ColliderComponent::Editor_ImGuiUpdate();

	ImGui::Text(u8"���f�[�^");

	// ���S���W
	ImGui::DragFloat3("CenterPos", &m_localBox.Center.x, 0.01f);
	// �T�C�Y
	ImGui::DragFloat3("Size", &m_localBox.Extents.x, 0.01f, 0, 1000);

	// �f�o�b�O�\��
	GAMESYS.DebugLine().AddBoxLine(m_worldBox.Center, m_worldBox.Extents, m_worldBox.Orientation, ColliderComponent::s_debugSelectLineColor);

}
