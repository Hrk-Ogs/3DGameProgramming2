#include "../../GameSystem.h"

#include "RayColliderComponent.h"

#include "CollisionManager.h"


bool RayColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// ���C vs ��
	return CollisionFunctions::SphereToRay(*sph, *this);
}

bool RayColliderComponent::HitTest(RayColliderComponent* ray)
{
	// ���C���m�̓q�b�g���Ȃ�
	return false;
}

bool RayColliderComponent::HitTest(BoxColliderComponent* box)
{
	// ���C vs Box
	return CollisionFunctions::RayToBox(*this, *box);
}

bool RayColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// ���C vs ���b�V��
	return CollisionFunctions::RayToMesh(*this, *mesh);
}

void RayColliderComponent::UpdateCollisionData()
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
		// ���[�J���̃��C�f�[�^���A�s��Ń��[���h�֕ϊ�����
		//-----------------------------------------------
		m_rayPos = m_localPos;
		m_rayDir = m_localDir;
		m_rayPos.TransformCoord(worldMat);
		m_rayDir.TransformNormal(worldMat);
		m_rayLen = m_rayDir.Length();

		m_rayDir.Normalize();
	}
}

void RayColliderComponent::Update()
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

void RayColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// �������̓X�L�b�v
	if (IsEnable() == false)return;

	//--------------------------------
	// ����f�[�^�X�V
	//--------------------------------
	// �f�o�b�O�\��
	if (ColliderComponent::s_showDebugLine)
	{
		// ���C
		GAMESYS.DebugLine().AddLine(m_rayPos, m_rayPos + m_rayDir * m_rayLen, ColliderComponent::s_debugLineColor);

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

void RayColliderComponent::Editor_ImGuiUpdate()
{
	// 
	ColliderComponent::Editor_ImGuiUpdate();

	ImGui::Text(u8"���C�f�[�^");

	// ���S���W
	ImGui::DragFloat3(u8"���C���W", &m_localPos.x, 0.01f);
	// �����E����
	ImGui::DragFloat3(u8"���C�x�N�g��", &m_localDir.x, 0.01f);



	// �f�o�b�O�\��
	GAMESYS.DebugLine().AddLine(m_rayPos, m_rayPos + m_rayDir * m_rayLen, ColliderComponent::s_debugSelectLineColor);
}
