#include "../KdSystem.h"

#include "KdAnimation.h"

// list		… クォータニオンキー配列
// time		… 時間
// 戻り値	… 次の配列のIndex
template<class T>
int BinarySearch_UpperBound(const std::vector<T>& list, float time)
{
	int low = 0;
	int high = (int)list.size();
	while (low < high)
	{
		int mid = (low + high) / 2;
		float midTime = list[mid].Time;

		if (midTime <= time) low = mid + 1;
		else high = mid;
	}
	return low;
}

bool KdKeyQuaternion::Interpolate(KdQuaternion& result, const std::vector<KdKeyQuaternion>& list, float time)
{
	if (list.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearch_UpperBound(list, time);

	// Indexが0の時は先頭のやつを使用
	if (keyIdx == 0)
	{
		result = list.front().Quat;
	}
	// Indexがリストの数より大きい場合は、最後のやつを使用
	else if (keyIdx >= list.size())
	{
		result = list.back().Quat;
	}
	// Indexが中間の場合は、補間計算
	else
	{
		auto& prev = list[keyIdx - 1];	// 前のキー
		auto& next = list[keyIdx];		// 次のキー
		//0～1間の位置を求める
		float f = (time - prev.Time) / (next.Time - prev.Time);
		// 補間
		result = DirectX::XMQuaternionSlerp(
			prev.Quat,
			next.Quat,
			f
		);
	}

	return true;
}


bool KdKeyVector3::Interpolate(KdVec3& result, const std::vector<KdKeyVector3>& list, float time)
{
	if (list.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearch_UpperBound(list, time);

	// Indexが0の時は先頭のやつを使用
	if (keyIdx == 0)
	{
		result = list.front().Vec;
		return true;
	}
	// Indexがリストの数より大きい場合は、最後のやつを使用
	else if (keyIdx >= list.size())
	{
		result = list.back().Vec;
		return true;
	}
	// Indexが中間の場合は、補間計算
	else
	{
		auto& prev = list[keyIdx - 1];	// 前のキー
		auto& next = list[keyIdx];		// 次のキー
		//0～1間の位置を求める
		float f = (time - prev.Time) / (next.Time - prev.Time);
		// 補間
		result = DirectX::XMVectorLerp(
			prev.Vec,
			next.Vec,
			f
		);
	}

	return true;
}


bool KdAnimationData::Interpolate(int nodeIdx, float time, float weight, KdVec3& s, KdQuaternion& r, KdVec3& t)
{
	if (nodeIdx >= (int)m_nodes.size())return false;
	if (weight <= 0.0f)return false;

	auto& animNode = m_nodes[nodeIdx];

	bool exist = false;

	// 回転補間
	{
		KdQuaternion rota;
		if (KdKeyQuaternion::Interpolate(rota, animNode->Rotations, time))
		{
			// 重み指定がある時は、合成先と補間する
			if (weight < 1.0f)
			{
				rota.Slerp(r, rota, weight);
			}

			r = rota;

			exist = true;
		}
	}

	// 拡縮補間
	{
		KdVec3 scale;
		if (KdKeyVector3::Interpolate(scale, animNode->Scales, time))
		{
			// 重み指定がある時は、合成先と補間する
			if (weight < 1.0f)
			{
				scale.Lerp(s, scale, weight);
			}

			s = scale;
			exist = true;
		}

	}

	// 座標補間
	{
		KdVec3 pos;
		if (KdKeyVector3::Interpolate(pos, animNode->Translations, time))
		{
			// 重み指定がある時は、合成先と補間する
			if (weight < 1.0f)
			{
				pos.Lerp(t, pos, weight);
			}

			t = pos;
			exist = true;
		}
	}

	return exist;
}

void KdAnimator::AdvanceTime(	float speed, KdNodeOutliner& nodeOL, std::function<void(const KdKeyScript&)> onScript,
								KdVec3* pScale, KdQuaternion* pRotation, KdVec3* pTranslation)
{
	// 実行するスクリプトキー(全トラックの処理のあとに実行するため)
	std::vector<const KdKeyScript*> execScripts;

	// 全トラック
	for(auto it = m_tracks.begin();it != m_tracks.end(); )
	{
		auto& track = (*it);

		// 無効時はスキップ
		if (track.Enable == false)
		{
			++it;
			continue;
		}

		// アニメが設定されていない
		if (track.Animation == nullptr)
		{
			++it;
			continue;
		}

		//=====================================
		// 全ノード アニメーション計算
		//=====================================
		for (UINT idx = 0; idx < track.Animation->GetNodes().size(); idx++)
		{
			// 参照
			auto& animNode = track.Animation->GetNodes()[idx];
			// ノードのIndex
			int nodeIdx = animNode->NodeOffset;
			// 配列数 限界チェック
			if (nodeIdx >= (int)nodeOL.GetAllNodes().size())continue;

			// Rootボーン かつ RootMotion指定があるなら、Root Motion処理
			if (animNode->IsRootBone && (pScale || pRotation || pTranslation))
			{
				KdVec3 s, t, prevS, prevT;
				KdQuaternion r, prevR;

				int exist = 0;
				// 前回の状態
				exist += track.Animation->Interpolate(idx, track.PrevTime, track.Weight * m_weight, prevS, prevR, prevT);
				// 今回の状態
				exist += track.Animation->Interpolate(idx, track.Time, track.Weight * m_weight, s, r, t);

				if (exist)
				{
					// デフォルトボーン回転の逆クォータニオン作成
					KdQuaternion invRootRotation;
					invRootRotation.CreateFromMatrix( nodeOL.GetAllNodes()[nodeIdx].Transform );
					invRootRotation.Inverse();

					// 回転を、Rootボーン空間へ変換
					r *= invRootRotation;
					prevR *= invRootRotation;

					// 変化量を求めるため、前回の回転を逆クォータニオンにする
					KdQuaternion prevInvR = prevR;
					prevInvR.Inverse();

					// 座標
					if (pTranslation)
					{
						// 前回と今回の移動量
						KdVec3 mv = t - prevT;
						// 現在の回転を考慮し、移動を変換
						mv.Transform(*pRotation * prevInvR);
						*pTranslation += mv * (*pScale);
					}

					// 回転
					if (pRotation)
					{
						// 前回の今回の回転量ぶん、回転させる
						*pRotation = (r * prevInvR) * (*pRotation);
					}

					// 拡大
//					if (pScale)
//					{
//						*pScale += s - prevS;
//					}
				}

			}
			// 通常補間処理
			else
			{
				float weight = track.Weight * m_weight;

				KdQuaternion r;
				KdVec3 s, t;
				if (weight > 0.0f)
				{
					nodeOL.GetAllNodes()[nodeIdx].LocalTransform.Decompose(s, r, t);
				}

				// 補間処理を行い、結果をtransforms[nodeIdx]に入れる
				track.Animation->Interpolate(idx, track.Time, weight, s, r, t);

				// 
				nodeOL.GetAllNodes()[nodeIdx].LocalTransform.CreateFromSRT(s, r, t);
			}
		}

		//=====================================
		// トラック重み操作
		//=====================================
		track.Weight += track.AddWeight;

		// 減算時は、Weightが0になったらトラックを消滅させる
		if (track.AddWeight < 0 && track.Weight <= 0.0f)
		{
			it = m_tracks.erase(it);
			continue;
		}
		// 加算時は、Weightが1,0になったら増加を止める
		if (track.Weight >= 1.0f)
		{
			track.Weight = 1.0f;
			track.AddWeight = 0;	// STOP
		}

		//=====================================
		// トラック時間進行
		//=====================================
		// 現在の時間を記憶しておく
		track.PrevTime = track.Time;
		// 時間を進める
		float sp = track.Speed * speed;
		track.Time += sp;

		// 順再生
		if (sp > 0)
		{
			// 最後まで行った時
			if (track.Time >= track.Animation->GetMaxLength())
			{
				// ループ再生
				if (track.Loop)
				{
					track.Time = 0;
					track.PrevTime = 0;
				}
				// 最後で停止
				else
				{
					track.Time = track.Animation->GetMaxLength();
					track.PrevTime = track.Animation->GetMaxLength();
				}
			}
		}
		// 逆再生
		else if (sp < 0)
		{
			// 最初より前に行った時
			if (track.Time < 0)
			{
				// ループ再生
				if (track.Loop)
				{
					track.Time = track.Animation->GetMaxLength();
					track.PrevTime = track.Animation->GetMaxLength();
				}
				// 最初で停止
				else
				{
					track.Time = 0;
					track.PrevTime = 0;
				}
			}
		}

		//=====================================
		// スクリプト判定
		//=====================================
		if (onScript)
		{

			auto& scripts = track.Animation->GetScripts();
			if (scripts.empty() == false)
			{
				// 全スクリプト
				for (auto&& key : scripts)
				{
					// 前回～今回の時間中にキーが存在する場合、指定関数を実行
					if (track.Time >= key.Time && track.PrevTime < key.Time)
					{
						// スクリプトキーを記憶させておく
						execScripts.push_back(&key);
						break;
					}

				}
			}
		}

		// 次のトラックへ
		++it;
	}

	// スクリプト実行
	for (auto&& key : execScripts)
	{
		onScript(*key);
	}

	//=====================================
	// アニメータ重み操作
	//=====================================
	m_weight += m_addWeight;

	if (m_weight >= 1.0f)
	{
		m_weight = 1.0f;
		m_addWeight = 0;	// STOP
	}
	else if (m_weight <= 0.0f)
	{
		m_weight = 0.0f;
		m_addWeight = 0;	// STOP
	}

}
