#include "../../GameSystem.h"

#include "CollisionManager.h"

std::vector<CollisionManager::FilterNode>	CollisionManager::s_filterType1 =
{
	{u8"ぶつかり",		0x00000001},
	{u8"攻撃",			0x00000002},
	{u8"ギミック発動",	0x00000004},
};

std::vector<CollisionManager::FilterNode>	CollisionManager::s_filterType2 =
{
	{u8"背景物",		0x00010000},
	{u8"味方",			0x00020000},
	{u8"敵",			0x00040000},
	{u8"ギミック",		0x00080000},
	{u8"プレイヤー",	0x00100000},
};

KdWptr<CollisionManager> CollisionManager::s_nowManager;

void CollisionManager::Run(bool onlyUpdateCollider)
{
	//-------------------------------
	//
	// コライダーのデータ更新
	//
	//-------------------------------
	// 判定する側リストのコライダーデータを更新
	std::for_each(
		std::execution::par,						// 並列で実行
		m_doJudgeList.begin(), m_doJudgeList.end(), // 配列の最初から最後まで
		[this](KdSptr<ColliderComponent>& doJudge)	// 実行する処理関数
		{
			doJudge->UpdateCollisionData();
		}
	);
	// 判定される側リストのコライダーデータを更新
	std::for_each(
		std::execution::par,						// 並列で実行
		m_passiveList.begin(), m_passiveList.end(), // 配列の最初から最後まで
		[this](KdSptr<ColliderComponent>& passive)	// 実行する処理関数
		{
			passive->UpdateCollisionData();
		}
	);

	//-------------------------------
	//
	// 衝突判定・通知
	//
	//-------------------------------
	// コライダー更新だけの場合は、衝突判定は実行しない
	if (onlyUpdateCollider == false)
	{
		//-------------------------------
		// 衝突判定処理
		//-------------------------------
		// 時間計測開始
		auto startTime = std::chrono::system_clock::now();

		// シングルスレッドでの判定
		if (m_isMultiThread == false)
		{
			// 全「判定する側」から「判定される側」へあたり判定
			for (auto&& doJudge : m_doJudgeList)
			{
				// 有効じゃない
				if (doJudge->IsEnable() == false)continue;
				// すでに登録フラグがOFF
				if (doJudge->IsRegistered() == false)continue;
				// 持ち主いない
				if (doJudge->ExistOwner() == false)continue;

				// 判定実行
				HitTestToPassiveList(*doJudge);
			}
		}
		// マルチスレッドでの判定
		else
		{
			// 並列forC++17から使用可能)
			// 指定した配列の各要素が並列で実行される
			std::for_each(
				std::execution::par,						// 並列で実行
				m_doJudgeList.begin(), m_doJudgeList.end(), // 配列の最初から最後まで
				[this](KdSptr<ColliderComponent>& doJudge)	// 実行する処理関数
				{
					// 有効じゃない
					if (doJudge->IsEnable() == false)return;
					// すでに登録フラグがOFF
					if (doJudge->IsRegistered() == false)return;
					// 持ち主いない
					if (doJudge->ExistOwner() == false)return;

					// 判定実行
					HitTestToPassiveList(*doJudge);
				}
			);

		}

		auto endTime = std::chrono::system_clock::now();
		// 経過時間計測
		m_debug_ElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

		//-------------------------------
		//
		// 結果通知関数実行
		//
		//-------------------------------]
		for (auto&& doJudge : m_doJudgeList)
		{
			//----------------
			// 通知関数実行
			//----------------

			// 有効じゃない
			if (doJudge->IsEnable() == false)continue;

			// 初回ヒット時のみ
			if (doJudge->m_hitState & HitStates::ENTER)
			{
				if (doJudge->m_onHitEnter)doJudge->m_onHitEnter(doJudge.get());
			}
			// ヒット中
			if (doJudge->m_hitState & HitStates::STAY)
			{
				if (doJudge->m_onHitStay)doJudge->m_onHitStay(doJudge.get());
			}
			// ヒットしなくなった最初の1回目のみ
			if (doJudge->m_hitState & HitStates::EXIT)
			{
				if (doJudge->m_onHitExit)doJudge->m_onHitExit(doJudge.get());
			}

			// バグ防止のため、毎回nullにする
			doJudge->m_onHitEnter = nullptr;
			doJudge->m_onHitStay = nullptr;
			doJudge->m_onHitExit = nullptr;
		}
	}

	//-------------------------------
	//
	// コライダーのチェック
	//　・不必要なコライダーはリストから解除する
	//
	//-------------------------------

	// 判定する側リスト
	for (auto it = m_doJudgeList.begin(); it != m_doJudgeList.end();)
	{
		// 参照カウンタが１つ 又は
		// 判定される側 又は
		// 登録フラグがOFF 又は
		// オーナーが消えている
		// のものは、登録解除する
		if ((*it).use_count() == 1 || (*it)->IsDoJudge() == false || (*it)->m_isRegistered == false || (*it)->ExistOwner() == false)
		{
			// 登録解除
			(*it)->m_isRegistered = false;
			(*it)->m_refMgr.reset();

			it = m_doJudgeList.erase(it);
		}
		else
		{
			++it;
		}
	}

	// 判定される側リスト
	for (auto it = m_passiveList.begin(); it != m_passiveList.end();)
	{
		// 参照カウンタが１つ 又は
		// 判定する側 又は
		// 登録フラグがOFF 又は
		// オーナーが消えている
		// のものは、登録解除する
		if ((*it).use_count() == 1 || (*it)->IsDoJudge() == true || (*it)->m_isRegistered == false || (*it)->ExistOwner() == false)
		{
			// 登録解除
			(*it)->m_isRegistered = false;
			(*it)->m_refMgr.reset();

			it = m_passiveList.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CollisionManager::HitTestToPassiveList(ColliderComponent& comp)
{
	// 判定する側じゃないなら、スキップ
	if (comp.IsDoJudge() == false)return;
	// 判定結果データをクリアする
	comp.m_resultList.clear();

	// PassiveListと判定
	for (auto&& passive : m_passiveList)
	{
		//------------------------
		// 事前チェック
		//------------------------
		if (passive->IsEnable() == false)continue;
		if (passive->ExistOwner() == false)continue;

		// 自分 vs 相手 フィルタ判定
		if ((comp.m_groupFilter & passive->m_maskFilter) == 0 || (comp.m_maskFilter & passive->m_groupFilter) == 0)continue;

		// 同じ持ち主は無視
		if (comp.GetOwner() == passive->GetOwner())continue;

		//------------------------
		// 判定
		//------------------------
		comp.HitTest(passive.get());
	}

	//-------------------------
	// ヒット状態フラグ操作
	//-------------------------
	// 初回HITフラグとHIT終了フラグをOFF
	comp.m_hitState &= ~HitStates::ENTER;// OFF
	comp.m_hitState &= ~HitStates::EXIT;// OFF

	// ヒットしている
	if (comp.GetResults().size() > 0)
	{
		// 前回ヒット中ではない
		if (!(comp.m_hitState & HitStates::STAY))
		{
			comp.m_hitState |= HitStates::ENTER;// ON
			comp.m_hitState |= HitStates::STAY;// ON
		}
	}
	//ヒットしていない
	else
	{
		//前回ヒット中
		if (comp.m_hitState & HitStates::STAY)
		{
			comp.m_hitState |= HitStates::EXIT;// ON
			comp.m_hitState &= ~HitStates::STAY;// OFF
		}
	}
}

void CollisionManager::Release()
{
	// 登録されているオブジェクトを解除していく
	for (auto&& n : m_doJudgeList)
	{
		n->m_isRegistered = false;
		n->m_refMgr.reset();
	}
	for (auto&& n : m_passiveList)
	{
		n->m_isRegistered = false;
		n->m_refMgr.reset();
	}
	m_doJudgeList.clear();
	m_passiveList.clear();
}

void CollisionManager::ImGuiUpdate()
{

	ImGui::Checkbox(u8"マルチスレッド", &m_isMultiThread);
	ImGui::Checkbox(u8"全Collisionデバッグ表示", &ColliderComponent::s_showDebugLine);

	// 色
	if (ImGui::CollapsingHeader(u8"色", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4(u8"線", &ColliderComponent::s_debugLineColor.x);
		ImGui::ColorEdit4(u8"選択物", &ColliderComponent::s_debugSelectLineColor.x);
	}

	// 情報
	if (ImGui::CollapsingHeader(u8"情報", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text(u8"判定する側 　%d 個", m_doJudgeList.size());
		ImGui::Text(u8"判定される側 %d 個", m_passiveList.size());
		ImGui::Text(u8"判定にかかった時間\n  %.3f ms", m_debug_ElapsedTime / 1000.0f);
	}
}


