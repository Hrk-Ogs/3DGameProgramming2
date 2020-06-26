#include "../../GameSystem.h"

#include "MeshColliderComponent.h"

#include "CollisionManager.h"

void MeshColliderComponent::LoadModel(const std::string& filename)
{
	// ���f���f�[�^�Z�b�g
	m_model = KDResFactory.GetModel(filename);
	// ���f�����Ȃ��Ƃ���
	if (m_model == nullptr)
	{
		m_model = std::make_shared<KdModel>();
	}
}

bool MeshColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// ���b�V�� vs ��
	return CollisionFunctions::SphereToMesh(*sph, *this);
}

bool MeshColliderComponent::HitTest(RayColliderComponent* ray)
{
	// ���b�V�� vs ���C
	return CollisionFunctions::RayToMesh(*ray, *this);
}

bool MeshColliderComponent::HitTest(BoxColliderComponent* box)
{
	// ����͖���
	return false;
}

bool MeshColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// ���b�V�����m�͖���
	return false;
}

void MeshColliderComponent::UpdateCollisionData()
{
	// �X�V���Ȃ��t���O�̎��̓X�L�b�v
	if (m_updateCollisionData == false)return;

	//--------------------------------
	// ����f�[�^�X�V
	//--------------------------------
	if (GetOwner())
	{
		auto& worldMat = GetOwner()->Transform()->GetMatrix();

		// Transform�̍s����擾���Ă���
		m_transform = GetOwner()->Transform()->GetMatrix();
	}
}

void MeshColliderComponent::Update()
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

void MeshColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// �������̓X�L�b�v
	if (IsEnable() == false)return;

	// �f�o�b�O�\��
	if (ColliderComponent::s_showDebugLine)
	{
		// ���b�V��
		for (auto&& meshIdx : m_model->GetNodeOutlinear().GetMeshNodeIndices())
		{
			const KdNodeOutliner::Node& node = m_model->GetNodeOutlinear().GetAllNodes()[meshIdx];

			GAMESYS.DebugLine().AddMeshLine(node.Mesh, node.LocalTransform * m_transform, ColliderComponent::s_debugLineColor);
		}

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

void MeshColliderComponent::Editor_ImGuiUpdate()
{
	// 
	ColliderComponent::Editor_ImGuiUpdate();

	ImGui::Text(u8"���b�V���f�[�^");

	// ���f���f�[�^
	if (ImGuiSelectModel(m_model))
	{

	}

	// �f�o�b�O�\��
	for (auto&& meshIdx : m_model->GetNodeOutlinear().GetMeshNodeIndices())
	{
		const KdNodeOutliner::Node& node = m_model->GetNodeOutlinear().GetAllNodes()[meshIdx];

		GAMESYS.DebugLine().AddMeshLine(node.Mesh, node.LocalTransform * m_transform, ColliderComponent::s_debugSelectLineColor);
	}

}
