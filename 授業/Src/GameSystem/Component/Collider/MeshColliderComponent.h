#pragma once

#include "ColliderComponent.h"

//========================================================
//
// メッシュコライダーコンポーネント
//
//========================================================
class MeshColliderComponent : public ColliderComponent
{
public:

	// モデルデータ取得
	const KdSptr<KdModel>& GetModel() const { return m_model; }
	// Transform取得
	const KdMatrix& GetMeshTransform() const { return m_transform; }

	// モデル読み込み
	void LoadModel(const std::string& filename);

	// 情報セット(GameObjectにアタッチせず、コライダー単品で使用する場合に使用)
	void Set(const std::string& filename, const KdMatrix& mat, bool isDoJudge, int groupFilter, int maskFilter)
	{
		m_isDoJudge = isDoJudge;
		m_groupFilter = groupFilter;
		m_maskFilter = maskFilter;

		m_transform = mat;

		LoadModel(filename);
	}

	// 
	void Move(const KdVec3& v)
	{
		m_transform.Move(v);
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

		std::string filename;
		KdJsonGet(jsonData["ModelFilename"], filename);

		LoadModel(filename);
	}

	// JSONデータから、初期設定を行う
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		ColliderComponent::Serialize(outJsonObj);

		outJsonObj["ModelFilename"] = m_model->GetFilepath();
	}

private:

	// 判定モデルデータ
	KdSptr<KdModel>		m_model = std::make_shared<KdModel>();
	KdMatrix			m_transform;	// Transformの行列
};
