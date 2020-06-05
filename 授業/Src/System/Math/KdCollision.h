#pragma once

// 点とOBBの最近接点を求める
// ・point			: 点の座標
// ・obb			: OBB
// ・outNearestPos	: (out)BOX上の最も近い座標が入る
void KdPointToBox(const KdVec3& point, const DirectX::BoundingOrientedBox& obb, KdVec3& outNearestPos);

// レイのヒット情報
struct KdRayHit
{
	float Distance = -1;	// レイの座標からヒットした位置までの距離が入る
	UINT FaceIdx = 0;		// ヒットした面のインデックスが入る
};

// レイ vs メッシュ判定
// ・rayPos		… レイの座標
// ・rayDir		… レイの方向
// ・maxDistance… レイがヒットする最大距離
// ・mesh		… メッシュ
// ・matrix		… メッシュの行列
// ・hit		… ヒット情報が入る
bool KdRayToMesh(const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir, float maxDistance, const KdMesh& mesh, const DirectX::XMMATRIX& matrix, KdRayHit& hit);

// 点 vs 三角形との最近接点を求める
// ・p			… 点の座標
// ・a			… 三角形の点１
// ・b			… 三角形の点２
// ・c			… 三角形の点３
// ・outPt		… 最近接点の座標
void KdPointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPt);

// 球のヒット情報
struct KdSphereHit
{
	// ヒット位置座標取得
	UINT MaxHitPositions = 0xFFFFFFFF;		// 最大何点まで取得するか？
	std::list<KdVec3> HitPositions;			// ヒット位置座標リスト　ここにヒット位置座標が入っていく

	// 判定で押し戻される計算を行い、ここに最終に押し戻された球の座標が入る。
	// nullptr指定なら押し戻し計算はしない。
	KdVec3* MovedSpherePos = nullptr;
};

// 球 vs メッシュ判定
// ・spherePos		… 球の座標
// ・radius			… 半径
// ・mesh			… メッシュ
// ・matrix			… メッシュの行列
// ・hitPositions	… 三角形ごとのヒットした座標が入る
// ・maxHitPositions… hitPositions
// ・movedSpherePos	… 判定で押し戻される計算を行い、ここに最終に押し戻された球の座標が入る。nullptr指定なら押し戻し計算はしない。
bool KdSphereToMesh(const KdVec3& spherePos, float radius, const KdMesh& mesh, const DirectX::XMMATRIX& matrix, KdSphereHit& hit);
