#pragma once

#include "ColliderComponent.h"

//========================================================
//
// レイコライダーコンポーネント
//
//========================================================
class RayColliderComponent : public ColliderComponent
{
public:

	// レイ座標取得
	const KdVec3& GetLocalRayPos() const { return m_localPos; }
	// レイ方向取得
	const KdVec3& GetLocalRayDir() const { return m_localDir; }

	// 判定に使用されるレイ座標取得
	const KdVec3& GetRayPos() const { return m_rayPos; }
	// 判定に使用されるレイ方向取得
	const KdVec3& GetRayDir() const { return m_rayDir; }
	// 判定に使用されるレイの長さ取得
	float GetRayLen() const { return m_rayLen; }

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const KdVec3& pos, const KdVec3& dir, bool isDoJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = isDoJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_rayPos = m_localPos = pos;
		m_rayDir = m_localDir = dir;
		m_rayLen = dir.Length();

		m_rayDir.Normalize();
	}

	// 
	void MoveRay(const KdVec3& v)
	{
		m_rayPos += v;
	}

	//------------------------------------------------
	// 判定処理関数
	//------------------------------------------------

	// あたり判定　自分 vs obj
	//Visterパターン
	virtual bool HitTest(ColliderComponent* obj) override
	{
		return obj->HitTest(this);
	}
	virtual bool HitTest(SphereColliderComponent* sph) override;
	virtual bool HitTest(RayColliderComponent* ray) override;
	virtual bool HitTest(BoxColliderComponent* box) override;
	virtual bool HitTest(MeshColliderComponent* mesh) override;

	// 判定データ更新
	virtual void UpdateCollisionData() override;

	//------------------------------------------------
	// コンポーネントクラスの関数実装
	//------------------------------------------------

	// 更新処理
	virtual void Update() override;
	// 描画準備処理
	virtual void PrepareDraw(RenderingData& rdata) override;

	// (Editor)
	virtual void Editor_ImGuiUpdate() override;

	//------------------------------------------------
	// Serialize / Deserialize
	//------------------------------------------------
	virtual void Deserialize(const json11::Json& jsonData) override
	{
		ColliderComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["RayPos"], m_localPos);
		KdJsonGet(jsonData["RayDir"], m_localDir);
	}

	// JSONデータから、初期設定を行う
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["RayPos"] = m_localPos.ToArray();
		outJsonObj["RayDir"] = m_localDir.ToArray();
	}

private:

	// レイ設定用データ
	KdVec3	m_localPos;		// ローカル座標
	KdVec3	m_localDir;		// ローカルベクトル(長さも使用)

	// 実際に判定で使用するレイデータ(m_localPosとm_localDirをTransformの行列で変換したもの)
	// なのでリアルタイムに変化する
	KdVec3	m_rayPos;		// 位置
	KdVec3	m_rayDir;		// 方向(正規化)
	float	m_rayLen;		// レイの長さ

};
