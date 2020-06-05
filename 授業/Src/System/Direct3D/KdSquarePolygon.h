#pragma once

// 四角形ポリゴンクラス
class KdSquarePolygon {
public:

	// ポリゴン設定
	// ・w		… 幅
	// ・h		… 高さ
	// ・color	… 色
	void Init(float w, float h, const KdVec4& color);

	// 描画
	void Draw(int setTextureNo);

	// 画像をセット
	void SetTexture(const KdSptr<KdTexture>& tex)
	{
		m_texture = tex;
	}

	// アニメーションの分割数を設定
	void SetAnimationInfo(int splitX, int splitY) {
		m_animSplitX = splitX;
		m_animSplitY = splitY;
	}

	// 分割設定をもとに、アニメーション位置設定
	// no	… 画像番号
	void SetAnimationPos(float no);

	// アニメーションを進行させる
	// ・speed		… 進行速度 1.0で１コマずつ
	// ・loop		… ループ再生する？
	void AdvanceTime(float speed, bool loop);

	// アニメーションの再生が終わった？
	bool IsAnimationEnd() {
		// 終了判定
		if (m_animPos >= (m_animSplitX * m_animSplitY) - 1  )
			return true;
		return false;
	}

private:

	// １頂点の形式
	struct Vertex
	{
		KdVec3 Pos;
		KdVec2 UV;
		KdVec4 Color;
	};

	// 頂点配列
	Vertex m_vertex[4];

	// アニメーション関係
	int	m_animSplitX = 1;	// 横の分割数
	int	m_animSplitY = 1;	// 縦の分割数

	float	m_animPos = 0;	// 現在のコマ位置

	KdSptr<KdTexture>	m_texture;
};
