#include "../../GameSystem.h"

#include "CollisionManager.h"
#include "CollisionFunctions.h"


//===============================================
//
// コライダー同士の判定関数
//
//===============================================

bool CollisionFunctions::SphereToSphere(SphereColliderComponent& sph1, SphereColliderComponent& sph2)
{
	// 球 vs 球
	KdVec3 vDir = sph2.GetWorldSphere().Center - sph1.GetWorldSphere().Center;
	float dist = vDir.Length();

	// 当たってないなら終了
	if (dist >= sph1.GetWorldSphere().Radius + sph2.GetWorldSphere().Radius)
	{
		return false;
	}

	vDir.Normalize();

	// 球１側に結果データ追加
	if (sph1.IsDoJudge())
	{
		// 質量比計算
		float ratio = sph1.CalcMassRatio(sph2.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph2.shared_from_this());		// 相手のコライダー
		res.Dist = (dist - sph2.GetWorldSphere().Radius);											// 最短距離
		res.HitPosList.push_back(sph1.GetWorldSphere().Center + vDir * res.Dist);
		res.Push = -vDir * ((sph2.GetWorldSphere().Radius + sph1.GetWorldSphere().Radius) - dist) * ratio;	// 反発ベクトル
		sph1.AddResult(res);

		// 押し戻し処理
		if (sph1.IsTrigger() == false && sph2.IsTrigger() == false)
		{
			KdVec3 pos = sph1.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph1.GetOwner()->Transform()->SetPosition(pos);

			sph1.MoveSphere(res.Push);
		}

	}
	// 球２側に結果データ追加
	if (sph2.IsDoJudge())
	{
		// 質量比計算
		float ratio = sph2.CalcMassRatio(sph1.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph1.shared_from_this());		// 相手のコライダー
		res.Dist = (dist - sph1.GetWorldSphere().Radius);											// 最短距離
		res.HitPosList.push_back(sph2.GetWorldSphere().Center - vDir * res.Dist);
		res.Push = vDir * ((sph2.GetWorldSphere().Radius + sph1.GetWorldSphere().Radius) - dist) * ratio;	// 反発ベクトル
		sph2.AddResult(res);

		// 押し戻し処理
		if (sph2.IsTrigger() == false && sph1.IsTrigger() == false)
		{
			KdVec3 pos = sph2.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph2.GetOwner()->Transform()->SetPosition(pos);

			sph2.MoveSphere(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::SphereToRay(SphereColliderComponent& sph, RayColliderComponent& ray)
{
	if (ray.GetRayLen() == 0)return false;

	// レイが当たった距離
	float dist;

	// 判定
	if (sph.GetWorldSphere().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist) == false) {
		return false;
	}

	// 長さチェック
	if (dist > ray.GetRayLen())return false;

	// 球側に結果データ追加
	if (sph.IsDoJudge()) {
		// 質量比計算
		float ratio = sph.CalcMassRatio(ray.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(ray.shared_from_this());		// 相手のコライダー
		res.Dist = dist;
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);
		res.Push = ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		sph.AddResult(res);

		// 押し戻し処理
		if (sph.IsTrigger() == false && ray.IsTrigger() == false)
		{
			KdVec3 pos = sph.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph.GetOwner()->Transform()->SetPosition(pos);

			sph.MoveSphere(res.Push);
		}
	}

	// レイ側に結果データ追加
	if (ray.IsDoJudge()) {
		// 質量比計算
		float ratio = ray.CalcMassRatio(sph.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph.shared_from_this());		// 相手のコライダー
		res.Dist = dist;
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);
		res.Push = -ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		ray.AddResult(res);

		// 押し戻し処理
		if (ray.IsTrigger() == false && sph.IsTrigger() == false)
		{
			KdVec3 pos = ray.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			ray.GetOwner()->Transform()->SetPosition(pos);

			ray.MoveRay(res.Push);
		}
	}
	return true;
}

bool CollisionFunctions::SphereToBox(SphereColliderComponent& sph, BoxColliderComponent& box)
{
	// 球と箱の判定
	KdVec3 vNearPt;
	KdPointToBox((KdVec3&)sph.GetWorldSphere().Center, box.GetWorldBox(), vNearPt);

	KdVec3 vD = vNearPt - sph.GetWorldSphere().Center;

	float len = vD.Length();
	// 当たっていないなら終了
	if (len >= sph.GetWorldSphere().Radius)return false;

	vD.Normalize();

	// 球側に結果データ追加
	if (sph.IsDoJudge()) {
		// 質量比計算
		float ratio = sph.CalcMassRatio(box.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box.shared_from_this());		// 相手のコライダー
		res.Dist = len;
		res.HitPosList.push_back(vNearPt);
		res.Push = -vD * (sph.GetWorldSphere().Radius - len) * ratio;
		sph.AddResult(res);

		// 押し戻し処理
		if (sph.IsTrigger() == false && box.IsTrigger() == false)
		{
			KdVec3 pos = sph.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph.GetOwner()->Transform()->SetPosition(pos);

			sph.MoveSphere(res.Push);
		}
	}

	// 箱側に結果データ追加
	if (box.IsDoJudge()) {
		// 質量比計算
		float ratio = box.CalcMassRatio(sph.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph.shared_from_this());		// 相手のコライダー
		res.Dist = len;
		res.HitPosList.push_back(vNearPt);
		res.Push = vD * (sph.GetWorldSphere().Radius - len) * ratio;
		box.AddResult(res);

		// 押し戻し処理
		if (box.IsTrigger() == false && sph.IsTrigger() == false)
		{
			KdVec3 pos = box.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			box.GetOwner()->Transform()->SetPosition(pos);

			box.MoveBox(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::SphereToMesh(SphereColliderComponent& sph, MeshColliderComponent& mesh)
{
	bool bHit = false;

	// 球データをコピー
	auto sphData = sph.GetWorldSphere();

	// 判定設定・結果データ
	KdSphereHit hitInfo;
	// 全ての面のヒット座標を受けらない時は、１つだけ受け取る
	if (sph.IsMeshDetailHitPos() == false)
	{
		hitInfo.MaxHitPositions = 1;
	}

	// 押し戻し計算を行う場合は、押し戻された座標を受け取る
	if (sph.IsTrigger() == false && mesh.IsTrigger() == false)
	{
		hitInfo.MovedSpherePos = (KdVec3*)&sphData.Center;
	}

	//---------------------------------
	// 全メッシュと判定
	//---------------------------------
	auto& model = mesh.GetModel();
	for (auto&& meshIdx : model->GetNodeOutlinear().GetMeshNodeIndices())
	{
		auto& node = model->GetNodeOutlinear().GetAllNodes()[meshIdx];

		// 球 vs メッシュ
		if (KdSphereToMesh(	sphData.Center, sphData.Radius,								// 球の情報
							*node.Mesh, node.LocalTransform * mesh.GetMeshTransform(),	// メッシュの情報
							hitInfo
		))
		{
			bHit = true;
		}
	}

	if (bHit == false)
	{
		// 当たってないなら終了
		return false;
	}

	KdVec3 vPush = sphData.Center - sph.GetWorldSphere().Center;

	// 球側に結果データ追加
	if (sph.IsDoJudge())
	{
		// 質量比計算
		float ratio = sph.CalcMassRatio(mesh.GetOwner()->GetMass());
		// 
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(mesh.shared_from_this());		// 相手のコライダー
		res.Push = vPush * ratio;					// 押し戻しベクトル
		res.HitPosList = hitInfo.HitPositions;
		sph.AddResult(res);

		// 押し戻し処理
		if (sph.IsTrigger() == false && mesh.IsTrigger() == false)
		{
			KdVec3 pos = sph.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			sph.GetOwner()->Transform()->SetPosition(pos);

			sph.MoveSphere(res.Push);
		}
	}

	// メッシュ側に結果データ追加
	if (mesh.IsDoJudge())
	{
		// 質量比計算
		float ratio = mesh.CalcMassRatio(sph.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(sph.shared_from_this());		// 相手のコライダー
		res.Push = -vPush * ratio;
		res.HitPosList = hitInfo.HitPositions;
		mesh.AddResult(res);

		// 押し戻し処理
		if (mesh.IsTrigger() == false && sph.IsTrigger() == false)
		{
			KdVec3 pos = mesh.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			mesh.GetOwner()->Transform()->SetPosition(pos);

			mesh.Move(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::RayToBox(RayColliderComponent& ray, BoxColliderComponent& box)
{
	if (ray.GetRayLen() == 0)return false;

	// レイ vs 箱
	float dist;
	if (!box.GetWorldBox().Intersects(ray.GetRayPos(), ray.GetRayDir(), dist))return false;

	if (dist < 0 || dist > ray.GetRayLen())return false;

	// レイ側に結果データ追加
	if (ray.IsDoJudge()) {
		// 質量比計算
		float ratio = ray.CalcMassRatio(box.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box.shared_from_this());		// 相手のコライダー
		res.Dist = dist;													// 距離
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);	// 位置
		res.Push = -ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		ray.AddResult(res);

		// 押し戻し処理
		if (ray.IsTrigger() == false && box.IsTrigger() == false)
		{
			KdVec3 pos = ray.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			ray.GetOwner()->Transform()->SetPosition(pos);

			ray.MoveRay(res.Push);
		}
	}

	// 箱側に結果データ追加
	if (box.IsDoJudge()) {
		// 質量比計算
		float ratio = box.CalcMassRatio(ray.GetOwner()->GetMass());
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(ray.shared_from_this());		// 相手のコライダー
		res.Dist = dist;
		res.HitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * dist);
		res.Push = ray.GetRayDir() * (ray.GetRayLen() - dist) * ratio;
		box.AddResult(res);

		// 押し戻し処理
		if (box.IsTrigger() == false && ray.IsTrigger() == false)
		{
			KdVec3 pos = box.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			box.GetOwner()->Transform()->SetPosition(pos);

			box.MoveBox(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::RayToMesh(RayColliderComponent& ray, MeshColliderComponent& mesh)
{
	if (ray.GetRayLen() == 0)return false;

	// ヒット情報
	bool bHit = false;

	float closestDist = FLT_MAX;
	KdSptr<KdMesh> closestMesh;
	UINT closestMeshFaceIdx = 0;

	KdRayHit rayHitInfo;
	std::list<KdVec3> hitPosList;

	// 全メッシュと判定
	auto& model = mesh.GetModel();
	for (auto&& meshIdx : model->GetNodeOutlinear().GetMeshNodeIndices())
	{
		auto& node = model->GetNodeOutlinear().GetAllNodes()[meshIdx];

		// レイ vs メッシュ
		if (KdRayToMesh(ray.GetRayPos(), ray.GetRayDir(), ray.GetRayLen(), *node.Mesh, node.LocalTransform * mesh.GetMeshTransform(), rayHitInfo))
		{
			// 一番近い？
			if (rayHitInfo.Distance < closestDist)
			{
				// この情報で更新
				closestDist = rayHitInfo.Distance;
				closestMeshFaceIdx = rayHitInfo.FaceIdx;
				closestMesh = node.Mesh;

				bHit = true;
			}

			// ヒット位置計算
			hitPosList.push_back(ray.GetRayPos() + ray.GetRayDir() * rayHitInfo.Distance);
		}
	}

	// なんにも当たってないなら終了
	if (bHit == false)return false;

	// レイの長さより短いなら、ヒットしていないとする
	if (closestDist > ray.GetRayLen())return false;

	// レイ側に結果データ追加
	if (ray.IsDoJudge())
	{
		// 質量比計算
		float ratio = ray.CalcMassRatio(mesh.GetOwner() ? mesh.GetOwner()->GetMass() : 0);
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(mesh.shared_from_this());	// 相手のコライダー
		res.Dist = closestDist;																	// 距離
		res.HitPosList = hitPosList;															// ヒット位置リスト
		res.Mesh = closestMesh;
		res.Mesh_FaceIdx = closestMeshFaceIdx;													// 面のインデックス
		res.Push = -ray.GetRayDir() * (ray.GetRayLen() - closestDist) * ratio;
		ray.AddResult(res);

		// 押し戻し処理
		if (ray.IsTrigger() == false && mesh.IsTrigger() == false)
		{
			KdVec3 pos = ray.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			ray.GetOwner()->Transform()->SetPosition(pos);

			ray.MoveRay(res.Push);
		}
	}

	// メッシュに結果データ追加
	if (mesh.IsDoJudge())
	{
		// 質量比計算
		float ratio = mesh.CalcMassRatio(ray.GetOwner() ? ray.GetOwner()->GetMass() : 0);
		//
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(ray.shared_from_this());		// 相手のコライダー
		res.Dist = closestDist; 
		res.HitPosList = hitPosList;															// ヒット位置リスト
		res.Push = ray.GetRayDir() * (ray.GetRayLen() - closestDist) * ratio;
		mesh.AddResult(res);

		// 押し戻し処理
		if (mesh.IsTrigger() == false && ray.IsTrigger() == false)
		{
			KdVec3 pos = mesh.GetOwner()->Transform()->GetPosition();
			pos += res.Push;
			mesh.GetOwner()->Transform()->SetPosition(pos);

			mesh.Move(res.Push);
		}
	}

	return true;
}

bool CollisionFunctions::BoxToBox(BoxColliderComponent& box1, BoxColliderComponent& box2)
{
	// 判定　※この関数では、正確なヒット位置を求めることは出来ない
	if (!box1.GetWorldBox().Intersects(box2.GetWorldBox()))
	{
		return false;
	}

	// 箱１側に結果データ追加
	if (box1.IsDoJudge())
	{
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box2.shared_from_this());		// 相手のコライダー
		res.HitPosList.push_back(box2.GetWorldBox().Center);
		box1.AddResult(res);
	}

	// 箱２側に結果データ追加
	if (box2.IsDoJudge())
	{
		CollisionResult res;
		res.Collider = std::static_pointer_cast<ColliderComponent>(box1.shared_from_this());		// 相手のコライダー
		res.HitPosList.push_back(box1.GetWorldBox().Center);
		box2.AddResult(res);
	}

	return true;
}
