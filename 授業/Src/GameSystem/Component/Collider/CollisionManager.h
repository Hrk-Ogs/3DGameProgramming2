#pragma once

#include "ColliderComponent.h"
#include "SphereColliderComponent.h"
#include "RayColliderComponent.h"
#include "BoxColliderComponent.h"
#include "MeshColliderComponent.h"

#include "CollisionFunctions.h"

// ヒット状態フラグ
namespace HitStates
{
	enum
	{
		ENTER = 0x00000001,	// HITした1回目
		STAY = 0x00000002,	// HIT中ずっと
		EXIT = 0x00000004,	// HITしなくなった1回目
	};
}

//===============================================
// 
// コリジョン管理クラス
// ・ColliderComponent系の登録・判定・結果通知のすべてを管理する。
//
//===============================================
class CollisionManager : public std::enable_shared_from_this<CollisionManager>
{
public:

	//-------------------------------------------------------
	// 判定オブジェクトを登録
	//-------------------------------------------------------
	void Register(const KdSptr<ColliderComponent>& obj)
	{
		if (obj == nullptr)return;

		// 登録済み？
		if (obj->IsRegistered())return;

		//判定する側？
		if (obj->IsDoJudge())
		{
			m_doJudgeList.push_back(obj);	// 登録
		}
		else
		{
			m_passiveList.push_back(obj);	// 登録
		}

		// 登録済みにする
		obj->m_isRegistered = true;
		obj->m_refMgr = shared_from_this();
	}

	//-------------------------------------------------------
	// コライダーの更新や判定の実行
	//-------------------------------------------------------
	// onlyUpdateCollider	… コライダーの更新のみ実行し、判定は実行しない。
	void Run(bool onlyUpdateCollider);

	// comp vs m_passiveListの判定
	void HitTestToPassiveList(ColliderComponent& comp);

	// 全解除
	void Release();

	//-------------------------------------------------------
	// 動作設定・取得
	//-------------------------------------------------------
	// マルチスレッドでの判定 On/Off
	void SetMultiThread(bool enable)
	{
		m_isMultiThread = enable;
	}

	// マルチスレッド動作か？
	bool IsMultiThread() const { return m_isMultiThread; }

	// このマネージャをデフォルトにする
	void SetManagerToDefault()
	{
		s_nowManager = shared_from_this();
	}

	// (static関数)現在セットされているマネージャを返す
	static KdSptr<CollisionManager> sGetNowManager()
	{
		return s_nowManager.lock();
	}

	// フィルターリストの１項目のデータ
	struct FilterNode
	{
		std::string Name;	// フィルター名
		int Flag = 0;		// ビットフラグ
	};


	// 判定フィルタ設定
	static std::vector<FilterNode>	s_filterType1;
	static std::vector<FilterNode>	s_filterType2;


	// ImGui
	void ImGuiUpdate();

private:

	// 判定する側リスト
	std::list< KdSptr<ColliderComponent> >	m_doJudgeList;
	// 判定される側リスト
	std::list< KdSptr<ColliderComponent> >	m_passiveList;

	// Run関数のあたり判定をマルチスレッドで行う
	bool m_isMultiThread = true;

	// 現在使用しているマネージャ
	// コライダーコンポーネントは、ここにセットされているCollisionManagerに登録することになる。
	static KdWptr<CollisionManager>	s_nowManager;

	// (デバッグ用)判定にかかった時間
	long long m_debug_ElapsedTime = 0;

};
