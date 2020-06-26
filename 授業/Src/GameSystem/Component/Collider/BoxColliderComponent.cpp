#include "../../GameSystem.h"

#include "BoxColliderComponent.h"

#include "CollisionManager.h"

bool BoxColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// Box vs 球
	return CollisionFunctions::SphereToBox(*sph, *this);
}

bool BoxColliderComponent::HitTest(RayColliderComponent* ray)
{
	// Box vs レイ
	return CollisionFunctions::RayToBox(*ray, *this);
}

bool BoxColliderComponent::HitTest(BoxColliderComponent* box)
{
	// Box vs Box
	return CollisionFunctions::BoxToBox(*this, *box);
}

bool BoxColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// Boxとメッシュ判定は、ややこしいので今回は無視
	return false;
}

void BoxColliderComponent::UpdateCollisionData()
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
		// ローカルの箱データを、行列でワールドへ変換する
		//-----------------------------------------------

		//ローカル座標をワールド座標に変換
		m_localBox.Transform(m_worldBox, worldMat);
	}
}

void BoxColliderComponent::Update()
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

void BoxColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// 無効時はスキップ
	if (IsEnable() == false)return;

	// デバッグ表示
	if (ColliderComponent::s_showDebugLine)
	{
		// 箱
		GAMESYS.DebugLine().AddBoxLine(m_worldBox.Center, m_worldBox.Extents, m_worldBox.Orientation, ColliderComponent::s_debugLineColor);

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

void BoxColliderComponent::Editor_ImGuiUpdate()
{
	// 
	ColliderComponent::Editor_ImGuiUpdate();

	ImGui::Text(u8"球データ");

	// 中心座標
	ImGui::DragFloat3("CenterPos", &m_localBox.Center.x, 0.01f);
	// サイズ
	ImGui::DragFloat3("Size", &m_localBox.Extents.x, 0.01f, 0, 1000);

	// デバッグ表示
	GAMESYS.DebugLine().AddBoxLine(m_worldBox.Center, m_worldBox.Extents, m_worldBox.Orientation, ColliderComponent::s_debugSelectLineColor);

}
