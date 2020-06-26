#pragma once

//===============================================
//
// コライダー同士の判定関数をまとめたクラス
//
//===============================================

class CollisionFunctions
{
public:
	// 球Collider vs 球Collider
	static bool SphereToSphere(SphereColliderComponent& sph1, SphereColliderComponent& sph2);

	// 球Collider vs レイCollider
	static bool SphereToRay(SphereColliderComponent& sph, RayColliderComponent& ray);

	// 球Collider vs 箱Collider
	static bool SphereToBox(SphereColliderComponent& sph, BoxColliderComponent& box);

	// 球Collider vs メッシュCollider
	static bool SphereToMesh(SphereColliderComponent& sph, MeshColliderComponent& mesh);



	// レイCollider vs 箱Collider
	static bool RayToBox(RayColliderComponent& ray, BoxColliderComponent& box);

	// レイCollider vs メッシュCollider
	static bool RayToMesh(RayColliderComponent& ray, MeshColliderComponent& mesh);



	// 箱Collider vs 箱Collider
	static bool BoxToBox(BoxColliderComponent& box1, BoxColliderComponent& box2);
};
