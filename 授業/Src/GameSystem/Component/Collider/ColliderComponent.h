#pragma once

#include "../BaseComponent.h"

class ColliderComponent;
class CollisionManager;

class SphereColliderComponent;
class RayColliderComponent;
class BoxColliderComponent;
class MeshColliderComponent;

//============================================
//
// 判定結果データ
//
//============================================
struct CollisionResult
{
	KdSptr<ColliderComponent> Collider;//ヒット相手のコライダーコンポーネント

	KdVec3 Push;	// 反発ベクトル(※現在、一部の形状でのみ正確な値になっています)
	float Dist = 0;	// 最短距離(※現在、一部の形状での正確な値になっています)

	// ヒット位置リスト
	std::list<KdVec3>	HitPosList;


	// レイ vs メッシュ専用
	KdWptr<KdMesh>	Mesh;				// メッシュ
	UINT			Mesh_FaceIdx = -1;	//面番号
};

//============================================
//
// コライダー基本クラス
//
//============================================
class ColliderComponent : public BaseComponent
{
	friend class CollisionManager;
public:

	//------------------------------------------------
	// 判定処理関数
	//------------------------------------------------
	// あたり判定実行　自分 vs obj
	// Visterパターン
	virtual bool HitTest(ColliderComponent* obj) = 0;

	virtual bool HitTest(SphereColliderComponent* sph) = 0;
	virtual bool HitTest(RayColliderComponent* ray) = 0;
	virtual bool HitTest(BoxColliderComponent* box) = 0;
	virtual bool HitTest(MeshColliderComponent* mesh) = 0;

	// 判定データ更新
	virtual void UpdateCollisionData() = 0;

	//------------------------------------------------
	// 設定データ
	//------------------------------------------------

	// タグ取得
	const std::string& GetStrTag() const { return m_strTag; }
	// タグ設定
	void SetStrTag(std::string_view tag) { m_strTag = tag; }

	// 判定のみ行うフラグセット
	void SetTrigger(bool enable) { m_isTrigger = enable; }
	// 判定のみ行う？(ぶつかり処理はしないか)
	bool IsTrigger() const { return m_isTrigger; }

	// CollisionManagerに登録済み？
	bool IsRegistered() const { return m_isRegistered; }
	// 現在のActiveのCollisionManagerに登録
	void Register();

	// UpdateCollisionData関数で更新処理を実行するかどうか
	void SetUpdateCollisionData(bool enable)
	{
		m_updateCollisionData = enable;
	}

	// 登録解除
	void Unregister()
	{
		m_isRegistered = false;
		m_refMgr.reset();
	}

	// 判定する側？
	bool IsDoJudge() const { return m_isDoJudge; }

	// 
	float CalcMassRatio(float mass);

	//------------------------------------------------
	// 判定結果データ
	//------------------------------------------------
	// 判定結果データ取得
	const std::vector<CollisionResult>& GetResults() const { return m_resultList; }
	// 判定結果データ追加
	void AddResult(const CollisionResult& resultData)
	{
		m_resultList.push_back(resultData);
	}

	//------------------------------------------------
	// ヒット通知関数オブジェクト
	//------------------------------------------------
	// ※今回はプログラムし易さ重視で関数オブジェクトにしましたが、Unityのようにvirtual関数で通知する方法もあります。
	// ※判定が終わるとnullになるので、毎回関数オブジェクトを入れる必要があります。
	std::function<void(ColliderComponent*)>	m_onHitEnter;	// ヒット時の初回のみ実行される
	std::function<void(ColliderComponent*)>	m_onHitStay;	// ヒット中ずっと実行される
	std::function<void(ColliderComponent*)>	m_onHitExit;	// ヒットしなくなった初回のみ実行される

	//------------------------------------------------
	// Serialize / Deserialize
	//------------------------------------------------
	virtual void Deserialize(const json11::Json& jsonData) override
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["Tag"], m_strTag);
		KdJsonGet(jsonData["IsDoJudge"], m_isDoJudge);
		KdJsonGet(jsonData["GroupFilter"], m_groupFilter);
		KdJsonGet(jsonData["MaskFilter"], m_maskFilter);
		KdJsonGet(jsonData["IsTrigger"], m_isTrigger);
	}

	// JSONデータから、初期設定を行う
	virtual void Serialize(json11::Json::object& outJsonObj) const override
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["Tag"] = m_strTag;
		outJsonObj["IsDoJudge"] = m_isDoJudge;
		outJsonObj["GroupFilter"] = m_groupFilter;
		outJsonObj["MaskFilter"] = m_maskFilter;
		outJsonObj["IsTrigger"] = m_isTrigger;
	}

	// (Editor)ImGui
	virtual void Editor_ImGuiUpdate() override;


	// Debug用設定
	static bool s_showDebugLine;
	static KdVec4 s_debugLineColor;
	static KdVec4 s_debugSelectLineColor;

protected:

	//------------------------------------------------
	// 設定データ
	//------------------------------------------------

	// 汎用文字列タグ
	std::string			m_strTag;

	// 判定する側か(通知取得可能)
	bool				m_isDoJudge = false;

	// 判定フィルタ
	int					m_groupFilter = 0;	// グループフィルタ　
	int					m_maskFilter = 0;	// マスクフィルタ

	// 判定のみ行い、ぶつかり処理はしない
	bool				m_isTrigger = false;

	// UpdateCollisionData関数で更新を実行する
	bool				m_updateCollisionData = true;

	//------------------------------------------------
	// ここから実行時のみ使用するデータ
	//------------------------------------------------

	// ヒット状態フラグ(初回ヒット時、ヒット中、ヒット解除時の判定用)
	int								m_hitState = 0;

	// 結果データ　m_isDoJudge == true の場合、ここにあたり判定の結果データが入ってくる
	std::vector<CollisionResult>	m_resultList;

	// CollisionManagerに登録済み？
	bool							m_isRegistered = false;

	// 登録マネージャへの参照
	KdWptr<CollisionManager>		m_refMgr;


};
