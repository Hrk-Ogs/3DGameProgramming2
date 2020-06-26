#include "../../GameSystem.h"

#include "SphereColliderComponent.h"

#include "CollisionManager.h"

bool SphereColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// 球 vs 球
	return CollisionFunctions::SphereToSphere(*this, *sph);
}

bool SphereColliderComponent::HitTest(RayColliderComponent* ray)
{
	// 球 vs レイ
	return CollisionFunctions::SphereToRay(*this, *ray);
}

bool SphereColliderComponent::HitTest(BoxColliderComponent* box)
{
	// 球 vs Box
	return CollisionFunctions::SphereToBox(*this, *box);
}

bool SphereColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// 球 vs メッシュ
	return CollisionFunctions::SphereToMesh(*this, *mesh);
}

void SphereColliderComponent::UpdateCollisionData()
{
	// 更新しないフラグの時はスキップ
	if (m_updateCollisionData == false)return;

	//--------------------------------
	// 判定データ更新
	//--------------------------------
	if (GetOwner())
	{
		auto& worldMat = GetOwner()->Transform()->GetMatrix();

		//-----------------------------------------------
		// ローカルの球データを、行列でワールドへ変換する
		//-----------------------------------------------

		//ローカル座標をワールド座標に変換
		m_localSphere.Transform(m_worldSphere, worldMat);
	}
}

void SphereColliderComponent::Update()
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

void SphereColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// 無効時はスキップ
	if (IsEnable() == false)return;

	// デバッグ表示
	if (ColliderComponent::s_showDebugLine)
	{
		// 球
		GAMESYS.DebugLine().AddSphereLine(m_worldSphere.Center, m_worldSphere.Radius, ColliderComponent::s_debugLineColor);

		// ヒット情報
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

	ImGui::Text(u8"球データ");

	// 中心座標
	ImGui::DragFloat3("CenterPos", &m_localSphere.Center.x, 0.01f);

	// 半径
	ImGui::DragFloat("Radius", &m_localSphere.Radius, 0.01f, 0, 1000);

	// メッシュ判定詳細設定
	if (ImGui::TreeNodeEx(u8"メッシュ判定時の詳細設定", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(u8"詳細なヒット位置取得", &m_meshDetailHitPos);

		ImGui::TreePop();
	}

	// デバッグ表示
	GAMESYS.DebugLine().AddSphereLine(m_worldSphere.Center, m_worldSphere.Radius, ColliderComponent::s_debugSelectLineColor);

}
