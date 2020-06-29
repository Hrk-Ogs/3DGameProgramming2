#pragma once

class KdNodeOutliner;

//============================
// クォータニオンキー
//============================
class KdKeyQuaternion
{
public:
	float				Time = 0;		// 時間
	KdQuaternion		Quat;			// クォータニオンデータ

	static bool Interpolate(KdQuaternion& result, const std::vector<KdKeyQuaternion>& list, float time);
};

//============================
// ベクトルキー
//============================
class KdKeyVector3
{
public:
	float				Time = 0;		// 時間
	KdVec3				Vec;			// 3Dベクトルデータ

	static bool Interpolate(KdVec3& result, const std::vector<KdKeyVector3>& list, float time);
};

//============================
// スクリプトキー
//============================
struct KdKeyScript
{
	float				Time = 0;		// 時間

	json11::Json		JsonData;		// データ
};

//============================
// アニメーションデータ
//============================
class KdAnimationData
{
public:

	// nodeIdx番目のノードの、指定時間timeの行列を補間計算し求める
	// ・nodeIdx：計算するノードのIndex
	// ・out	：補間結果の行列が入る
	// ・戻り値	：true…補間計算完了 false…キー無し
	bool Interpolate(int nodeIdx, float time, float weight, KdVec3& s, KdQuaternion& r, KdVec3& t);

	// アニメの長さ
	float GetMaxLength() const { return m_maxLength; }
	void SetMaxLength(float length) { m_maxLength = length; }

	// アニメーション名
	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	// １ノードのアニメーションデータ
	struct Node
	{
		int								NodeOffset = -1;	// 対象ノードのOffset

		bool							IsRootBone = false;	// ルートボーン？

		// 各チャンネル
		std::vector<KdKeyQuaternion>	Rotations;		// 回転キーリスト
		std::vector<KdKeyVector3>		Scales;			// 拡大キーリスト
		std::vector<KdKeyVector3>		Translations;	// 位置キーリスト
	};

	// 全ノードリスト取得
	const std::vector<std::shared_ptr<Node>>& GetNodes() const { return m_nodes; }

	// ノード追加
	void AddNode(const std::shared_ptr<Node>& node)
	{
		m_nodes.push_back(node);
	}

	// スクリプトリストセット
	void SetScripts(const std::vector<KdKeyScript>& scripts)
	{
		m_scripts = scripts;
	}
	// スクリプトリスト取得
	const std::vector<KdKeyScript>& GetScripts() const { return m_scripts; }

private:
	// アニメーション名
	std::string				m_name;

	// アニメ長さ
	float					m_maxLength = 0;

	// 全ノード用アニメーションデータ
	std::vector<std::shared_ptr<Node>>	m_nodes;

	// スクリプトキー配列
	std::vector<KdKeyScript>	m_scripts;
};

//============================
//
// アニメータークラス
// ・セットしたKdAnimationDataを再生する
//
//============================
class KdAnimator
{
	struct Track;
public:

	// アニメータの重み設定
	void SetWeight(float weight, float speed = 0)
	{
		m_weight = weight;
		m_addWeight = speed;
	}

	// 再生トラック情報
	Track& NowPlayTrack() { return m_tracks.front(); }

	// アニメ終了？
	bool IsAnimationEnd() const
	{
		if (m_tracks[0].Enable == false)return true;
		if (m_tracks[0].Animation == nullptr)return true;
		if (m_tracks[0].Time >= m_tracks[0].Animation->GetMaxLength())return true;
		return false;
	}

	// アニメーションデータをセット
	void SetAnimation(const KdSptr<KdAnimationData>& anim, bool loop, float speed = 1.0f, float weight = 1.0f, float time = 0.0f)
	{
		m_tracks[0].Enable = true;
		m_tracks[0].Animation = anim;
		m_tracks[0].Speed = speed;
		m_tracks[0].Weight = weight;
		m_tracks[0].Loop = loop;
		m_tracks[0].Time = time;

		m_tracks[0].AddWeight = 0;
		m_tracks[0].PrevTime = 0;
	}

	// アニメーションを滑らかに切り替える
	// ・anim		：新たなアニメーションデータ
	// ・duration	：変化にかかる時間　1.0で即変化
	// ・speed		：新たなアニメの再生速度
	// ・loop		：ループ再生するか
	// ・time		：再生位置
	void CrossFade(const KdSptr<KdAnimationData>& anim, float duration, bool loop, float speed = 1.0f, float weight = 1.0f, float time = 0.0f)
	{
		// 0～1にクランプ
		duration = std::clamp(duration, 0.0f, 1.0f);
		// 滑らか変化
		if (duration < 1.0f)
		{
			// トラック追加
			m_tracks.push_front(Track());

			// 新しいアニメは重み0から
			SetAnimation(anim, loop, speed, 0.0f, time);
			m_tracks[0].AddWeight = duration;

			// 古いアニメは重みを0へ近づけていく
			m_tracks[1].AddWeight = -duration;
			m_tracks[1].Weight = 1.0f;	// 今回は簡易的に[1]の重みを1に
		}
		// 即座に変化
		else
		{
			// 一度全トラック削除
			m_tracks.clear();
			// 追加し設定
			m_tracks.push_front(Track());
			SetAnimation(anim, loop, speed, weight, time);
		}
	}

	// アニメーション進行
	// ・speed			：アニメーション進行速度。
	// ・nodeOL			：ノードアウトライナー。この中のノードのLocalTransformにアニメーション結果が入る。
	// ・pScale			：指定するとRoot Motionが有効になり、Rootボーンの変化量がここに適用される。transformsのRootボーンは変化しなくなる。
	// 。onScript		：
	// ・pRotation		：指定するとRoot Motionが有効になり、Rootボーンの変化量がここに適用される。transformsのRootボーンは変化しなくなる。
	// ・pTranslation	：指定するとRoot Motionが有効になり、Rootボーンの変化量がここに適用される。transformsのRootボーンは変化しなくなる。
	// ※現在RootMotionの拡大は未実装です。
	void AdvanceTime(	float speed, KdNodeOutliner& nodeOL, std::function<void(const KdKeyScript&)> onScript,
						KdVec3* pScale = nullptr, KdQuaternion* pRotation = nullptr, KdVec3* pTranslation = nullptr);

	// 解放
	void Release()
	{
		m_tracks.clear();
		m_tracks.push_back(Track());
	}

	// 
	KdAnimator()
	{
		m_tracks.push_back(Track());
	}

private:

	// 再生トラック情報(アニメーションの再生単位)
	struct Track
	{
		bool	Enable = false;	// 有効

		float	Time = 0;		// 再生位置
		float	Speed = 1.0f;	// 再生速度
		float	Weight = 1.0f;	// 重み
		bool	Loop = true;	// ループ再生

		KdSptr<KdAnimationData>		Animation;	// 再生アニメーション

		float	AddWeight = 0;	// Weightに加算する値
		float	PrevTime = 0;	// 前回のTime(Root Motion用)
	};

	std::deque<Track>	m_tracks;

	// 重み
	float m_weight = 1.0f;
	float m_addWeight = 0;


};
