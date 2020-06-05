#pragma once

//===============================
// 軌跡ポリゴンクラス
//===============================
class KdTrailPolygon {
public:
	// 先頭のポイントを取得
	KdVec3* GetTopPoint() {
		if (m_pointList.size() == 0)return nullptr;
		return &m_pointList.front();
	}

	// ポイントを追加
	void AddPoint(const KdVec3& pos) {
		m_pointList.push_front(pos);
	}

	// 最後尾のポイントを削除
	void DelPoint_Back()
	{
		m_pointList.pop_back();
	}

	// リストの数を取得
	int GetNumPoints() const
	{
		return (int)m_pointList.size();
	}

	// 画像をセット
	void SetTexture(const KdSptr<KdTexture>& tex)
	{
		m_texture = tex;
	}

	// ビルボード描画　SHADERのカメラ情報をもとに、カメラの方に向いたポリゴンを作成し描画する
	// ・width	… ポリゴンの幅
	void DrawBillboard(float width);

	// ポイントをそのままストリップ描画する
	void DrawStrip(int setTextureSlotNo = 0);

private:

	// １頂点の形式
	struct Vertex
	{
		KdVec3 Pos;
		KdVec2 UV;
		KdVec4 Color = { 1,1,1,1 };
	};

	// 軌跡の位置などを記憶する行列配列
	std::deque<KdVec3>	m_pointList;

	// テクスチャ
	KdSptr<KdTexture>	m_texture;
};
