#include "../../GameSystem.h"

#include "MeshColliderComponent.h"

#include "CollisionManager.h"

void MeshColliderComponent::LoadModel(const std::string& filename)
{
	// モデルデータセット
	m_model = KDResFactory.GetModel(filename);
	// モデルがないときは
	if (m_model == nullptr)
	{
		m_model = std::make_shared<KdModel>();
	}
}

bool MeshColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// メッシュ vs 球
	return CollisionFunctions::SphereToMesh(*sph, *this);
}

bool MeshColliderComponent::HitTest(RayColliderComponent* ray)
{
	// メッシュ vs レイ
	return CollisionFunctions::RayToMesh(*ray, *this);
}

bool MeshColliderComponent::HitTest(BoxColliderComponent* box)
{
	// 今回は無視
	return false;
}

bool MeshColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// メッシュ同士は無視
	return false;
}

void MeshColliderComponent::UpdateCollisionData()
{
	// 更新しないフラグの時はスキップ
	if (m_updateCollisionData == false)return;

	//--------------------------------
	// 判定データ更新
	//--------------------------------
	if (GetOwner())
	{
		auto& worldMat = GetOwner()->Transform()->GetMatrix();

		// Transformの行列を取得しておく
		m_transform = GetOwner()->Transform()->GetMatrix();
	}
}

void MeshColliderComponent::Update()
{
	// 無効時はスキップ
	if (IsEnable() == false)return;
	// 実行時のみ
//	if (GAMESYS.IsPlay())return;

	//--------------------------------
	// 登録
	//--------------------------------
	Register();
}

void MeshColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// 無効時はスキップ
	if (IsEnable() == false)return;

	// デバッグ表示
	if (ColliderComponent::s_showDebugLine)
	{
		// メッシュ
		for (auto&& meshIdx : m_model->GetNodeOutlinear().GetMeshNodeIndices())
		{
			const KdNodeOutliner::Node& node = m_model->GetNodeOutlinear().GetAllNodes()[meshIdx];

			GAMESYS.DebugLine().AddMeshLine(node.Mesh, node.LocalTransform * m_transform, ColliderComponent::s_debugLineColor);
		}

		// ヒット情報
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

	ImGui::Text(u8"メッシュデータ");

	// モデルデータ
	if (ImGuiSelectModel(m_model))
	{

	}

	// デバッグ表示
	for (auto&& meshIdx : m_model->GetNodeOutlinear().GetMeshNodeIndices())
	{
		const KdNodeOutliner::Node& node = m_model->GetNodeOutlinear().GetAllNodes()[meshIdx];

		GAMESYS.DebugLine().AddMeshLine(node.Mesh, node.LocalTransform * m_transform, ColliderComponent::s_debugSelectLineColor);
	}

}
