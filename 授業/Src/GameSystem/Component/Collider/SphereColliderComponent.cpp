#include "../../GameSystem.h"

#include "SphereColliderComponent.h"

#include "CollisionManager.h"

bool SphereColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// �� vs ��
	return CollisionFunctions::SphereToSphere(*this, *sph);
}

bool SphereColliderComponent::HitTest(RayColliderComponent* ray)
{
	// �� vs ���C
	return CollisionFunctions::SphereToRay(*this, *ray);
}

bool SphereColliderComponent::HitTest(BoxColliderComponent* box)
{
	// �� vs Box
	return CollisionFunctions::SphereToBox(*this, *box);
}

bool SphereColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// �� vs ���b�V��
	return CollisionFunctions::SphereToMesh(*this, *mesh);
}

void SphereColliderComponent::UpdateCollisionData()
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
		// ���[�J���̋��f�[�^���A�s��Ń��[���h�֕ϊ�����
		//-----------------------------------------------

		//���[�J�����W�����[���h���W�ɕϊ�
		m_localSphere.Transform(m_worldSphere, worldMat);
	}
}

void SphereColliderComponent::Update()
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

void SphereColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// �������̓X�L�b�v
	if (IsEnable() == false)return;

	// �f�o�b�O�\��
	if (ColliderComponent::s_showDebugLine)
	{
		// ��
		GAMESYS.DebugLine().AddSphereLine(m_worldSphere.Center, m_worldSphere.Radius, ColliderComponent::s_debugLineColor);

		// �q�b�g���
		for (auto&& res : m_resultList)
		{
			for (auto&& pt : res.HitPosList)
			{
				//			GAMESYS.DebugLine().AddSphereLine(pt, 0.05f, { 1,0,0,1 });
				GAMESYS.DebugLine().AddCrossLine(pt, 0.1f, { 1,0,0,1 });
			}

			GAMESYS.DebugLine().AddLine(res.HitPosList.front(), res.HitPosList.front() + res.Push, { 1,0,0,1 });
		}
	}
}

void SphereColliderComponent::Editor_ImGuiUpdate()
{
	// 
	ColliderComponent::Editor_ImGuiUpdate();

	ImGui::Text(u8"���f�[�^");

	// ���S���W
	ImGui::DragFloat3("CenterPos", &m_localSphere.Center.x, 0.01f);

	// ���a
	ImGui::DragFloat("Radius", &m_localSphere.Radius, 0.01f, 0, 1000);

	// ���b�V������ڍאݒ�
	if (ImGui::TreeNodeEx(u8"���b�V�����莞�̏ڍאݒ�", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(u8"�ڍׂȃq�b�g�ʒu�擾", &m_meshDetailHitPos);

		ImGui::TreePop();
	}

	// �f�o�b�O�\��
	GAMESYS.DebugLine().AddSphereLine(m_worldSphere.Center, m_worldSphere.Radius, ColliderComponent::s_debugSelectLineColor);

}
