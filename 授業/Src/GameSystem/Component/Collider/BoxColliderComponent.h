#pragma once

#include "ColliderComponent.h"

//========================================================
//
// 箱コライダーコンポーネント
//
//========================================================
class BoxColliderComponent : public ColliderComponent
{
public:

	// 未変換BOX情報取得
	const DirectX::BoundingOrientedBox& GetLocalBox() const { return m_localBox; }
	// 変換済みBOX情報取得
	const DirectX::BoundingOrientedBox& GetWorldBox() const { return m_worldBox; }

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const KdVec3& pos, const KdVec3& size, const KdMatrix& rotaMat, bool isDoJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = isDoJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_localBox.Center = pos;
		m_localBox.Extents = size;

		m_localBox.Transform(m_worldBox, rotaMat);
	}

	// 
	void MoveBox(const KdVec3& v)
	{
		(KdVec3&)m_worldBox.Center += v;
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

		KdJsonGet(jsonData["Center"], (KdVec3&)m_localBox.Center);
		KdJsonGet(jsonData["Extents"], (KdVec3&)m_localBox.Extents);
	}

	// JSONデータから、初期設定を行う
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["Center"] = KdVec3(m_localBox.Center).ToArray();
		outJsonObj["Extents"] = KdVec3(m_localBox.Extents).ToArray();
	}

private:

	// 箱設定データ
	DirectX::BoundingOrientedBox	m_localBox;

	// 実際に判定で使用する球データ(m_localPosとm_radiusをTransformの行列で変換したもの)
	// なのでリアルタイムに変化する
	DirectX::BoundingOrientedBox	m_worldBox;

};
