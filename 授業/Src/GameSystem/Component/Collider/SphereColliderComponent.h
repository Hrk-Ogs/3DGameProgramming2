#pragma once

#include "ColliderComponent.h"

//========================================================
//
// 球コライダーコンポーネント
//
//========================================================
class SphereColliderComponent : public ColliderComponent
{
public:

	// 未変換球情報取得
	const DirectX::BoundingSphere& GetLocalSphere() const { return m_localSphere; }
	// 変換済み球情報取得
	const DirectX::BoundingSphere& GetWorldSphere() const { return m_worldSphere; }

	// メッシュとの判定時、詳細なヒット位置情報を取得するか
	bool IsMeshDetailHitPos() const { return m_meshDetailHitPos; }
	void MeshDetailHitPos(bool enable) { m_meshDetailHitPos = enable; }

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const KdVec3& pos, float radius, bool doJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = doJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		// 球情報
		m_localSphere.Center = pos;
		m_worldSphere.Center = pos;
		m_localSphere.Radius = radius;
		m_worldSphere.Radius = radius;
	}

	// 
	void MoveSphere(const KdVec3& v)
	{
		(KdVec3&)m_worldSphere.Center += v;
	}

	//------------------------------------------------
	// 判定処理関数
	//------------------------------------------------

	// あたり判定　自分 vs obj
	// Visterパターン
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

		KdJsonGet( jsonData["Center"], (KdVec3&)m_localSphere.Center);
		KdJsonGet(jsonData["Radius"], m_localSphere.Radius);
		KdJsonGet(jsonData["MeshDetailHitPos"], m_meshDetailHitPos);
	}

	// JSONデータから、初期設定を行う
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["Center"] = KdVec3(m_localSphere.Center).ToArray();
		outJsonObj["Radius"] = m_localSphere.Radius;
		outJsonObj["MeshDetailHitPos"] = m_meshDetailHitPos;
	}

private:

	// 球設定用データ
	DirectX::BoundingSphere m_localSphere;

	// 実際に判定で使用する球データ(m_localPosとm_radiusをTransformの行列で変換したもの)
	// なのでリアルタイムに変化する
	DirectX::BoundingSphere m_worldSphere;

	// メッシュコライダーと判定時、全ヒット位置を取得するか
	bool					m_meshDetailHitPos = false;
};
