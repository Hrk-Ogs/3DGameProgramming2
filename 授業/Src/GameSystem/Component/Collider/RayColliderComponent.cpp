#include "../../GameSystem.h"

#include "RayColliderComponent.h"

#include "CollisionManager.h"


bool RayColliderComponent::HitTest(SphereColliderComponent* sph)
{
	// レイ vs 球
	return CollisionFunctions::SphereToRay(*sph, *this);
}

bool RayColliderComponent::HitTest(RayColliderComponent* ray)
{
	// レイ同士はヒットしない
	return false;
}

bool RayColliderComponent::HitTest(BoxColliderComponent* box)
{
	// レイ vs Box
	return CollisionFunctions::RayToBox(*this, *box);
}

bool RayColliderComponent::HitTest(MeshColliderComponent* mesh)
{
	// レイ vs メッシュ
	return CollisionFunctions::RayToMesh(*this, *mesh);
}

void RayColliderComponent::UpdateCollisionData()
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
		// ローカルのレイデータを、行列でワールドへ変換する
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
	// 無効時はスキップ
	if (IsEnable() == false)return;
	// 実行時のみ
//	if (GAMESYS.IsPlay())return;

	//--------------------------------
	// 登録
	//--------------------------------
	Register();
}

void RayColliderComponent::PrepareDraw(RenderingData& rdata)
{
	// 無効時はスキップ
	if (IsEnable() == false)return;

	//--------------------------------
	// 判定データ更新
	//--------------------------------
	// デバッグ表示
	if (ColliderComponent::s_showDebugLine)
	{
		// レイ
		GAMESYS.DebugLine().AddLine(m_rayPos, m_rayPos + m_rayDir * m_rayLen, ColliderComponent::s_debugLineColor);

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

void RayColliderComponent::Editor_ImGuiUpdate()
{
	// 
	ColliderComponent::Editor_ImGuiUpdate();

	ImGui::Text(u8"レイデータ");

	// 中心座標
	ImGui::DragFloat3(u8"レイ座標", &m_localPos.x, 0.01f);
	// 方向・長さ
	ImGui::DragFloat3(u8"レイベクトル", &m_localDir.x, 0.01f);



	// デバッグ表示
	GAMESYS.DebugLine().AddLine(m_rayPos, m_rayPos + m_rayDir * m_rayLen, ColliderComponent::s_debugSelectLineColor);
}
