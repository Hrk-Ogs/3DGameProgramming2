#pragma once

//==========================================================
// メッシュ用 頂点データ
//==========================================================
struct KdMeshVertexData
{
	// 頂点データリスト
	std::vector<KdVec3>			PosList;			// 座標リスト
	std::vector<KdVec2>			UVList;				// UV座標リスト
	std::vector<KdVec3>			TangentList;		// 接線リスト
	std::vector<KdVec3>			NormalList;			// 法線リスト
	std::vector<UINT>			ColorList;			// 色リスト

	std::vector<std::array<short, 4>>	SkinIndexList;	// スキニングIndexリスト
	std::vector<std::array<float, 4>>	SkinWeightList;	// スキニングウェイトリスト

	// KdMeshVertexDataを結合する
	void Combine(const KdMeshVertexData& vertex)
	{
		PosList.insert(PosList.end(), vertex.PosList.begin(), vertex.PosList.end());
		UVList.insert(UVList.end(), vertex.UVList.begin(), vertex.UVList.end());
		TangentList.insert(TangentList.end(), vertex.TangentList.begin(), vertex.TangentList.end());
		NormalList.insert(NormalList.end(), vertex.NormalList.begin(), vertex.NormalList.end());
		ColorList.insert(ColorList.end(), vertex.ColorList.begin(), vertex.ColorList.end());

		SkinIndexList.insert(SkinIndexList.end(), vertex.SkinIndexList.begin(), vertex.SkinIndexList.end());
		SkinWeightList.insert(SkinWeightList.end(), vertex.SkinWeightList.begin(), vertex.SkinWeightList.end());
	}
};

//==========================================================
// メッシュ用 面情報
//==========================================================
struct KdMeshFace
{
	UINT Idx[3];				// 三角形を構成する頂点のIndex
};

//==========================================================
// メッシュ用 サブセット情報
//==========================================================
struct KdMeshSubset
{
	UINT		MaterialNo = 0;		// マテリアルNo

	UINT		FaceStart = 0;		// 面Index　このマテリアルで使用されている最初の面のIndex
	UINT		FaceCount = 0;		// 面数　FaceStartから、何枚の面が使用されているかの
};

//==========================================================
//
// メッシュクラス
//
//==========================================================
class KdMesh
{
public:

	//=================================================
	// 取得・設定
	//=================================================

	// 作成済み？
	bool								IsCreated() const { return m_vb_Pos.IsCreated(); }

	// サブセット情報配列を取得
	const std::vector<KdMeshSubset>&	GetSubsets() const { return m_subsets; }

	// 頂点の座標配列を取得
	const std::vector<KdVec3>&			GetVertexPositions() const { return m_positions; }
	// 面の配列を取得
	const std::vector<KdMeshFace>&		GetFaces() const { return m_faces; }

	// 軸平行境界ボックス取得
	const DirectX::BoundingBox&			GetBoundingBox() const { return m_aabb; }
	// 境界球取得
	const DirectX::BoundingSphere&		GetBoundingSphere() const { return m_bs; }

	// スキンメッシュ？
	bool IsSkinMesh() const { return m_vb_SkinIndex.IsCreated(); }

	// メッシュデータをデバイスへセットする
	void SetToDevice() const;


	//=================================================
	// 作成・解放
	//=================================================

	// メッシュ作成
	// ・vertexData		… 頂点データ
	// ・faces			… 面インデックス情報配列
	// ・subsets		… サブセット情報配列
	// 戻り値			… 成功：true
	bool Create(const KdMeshVertexData& vertexData,
				const std::vector<KdMeshFace>& faces,
				const std::vector<KdMeshSubset>& subsets);

	// 解放
	void Release()
	{
		m_vb_Pos.Release();
		m_vb_UV.Release();
		m_vb_Tangent.Release();
		m_vb_Normal.Release();
		m_vb_Color.Release();
		m_vb_SkinIndex.Release();
		m_vb_SkinWeight.Release();

		m_stride.clear();
		m_offset.clear();

		m_ib.Release();
		m_subsets.clear();
		m_positions.clear();
		m_faces.clear();
	}

	~KdMesh()
	{
		Release();
	}

	//=================================================
	// 処理
	//=================================================

	// 指定サブセットを描画
	void DrawSubset(int subsetNo) const;

	// 
	KdMesh() {}

private:

	//--------------------------
	// 頂点バッファ
	// ・要素ごとに頂点バッファを用意する
	//--------------------------
	KdBuffer						m_vb_Pos;		// 座標バッファ
	KdBuffer						m_vb_UV;		// UV座標バッファ
	KdBuffer						m_vb_Tangent;	// 接線バッファ
	KdBuffer						m_vb_Normal;	// 法線バッファ
	KdBuffer						m_vb_Color;		// 色バッファ
	// ・スキンメッシュ用の時のみ作成する
	KdBuffer						m_vb_SkinIndex;	// スキニングIndexバッファ
	KdBuffer						m_vb_SkinWeight;// スキニングウェイトバッファ

	// 各バッファごとの１項目のサイズ(例：PosならVector3なので12バイト)
	std::vector<UINT>				m_stride;
	std::vector<UINT>				m_offset;

	// インデックスバッファ
	KdBuffer						m_ib;

	// サブセット情報
	std::vector<KdMeshSubset>		m_subsets;

	// 境界データ
	DirectX::BoundingBox			m_aabb;	// 軸平行境界ボックス
	DirectX::BoundingSphere			m_bs;	// 境界球

	// 座標のみの配列
	std::vector<KdVec3>				m_positions;
	// 面情報のみの配列
	std::vector<KdMeshFace>			m_faces;

private:
	// コピー禁止用
	KdMesh(const KdMesh& src) = delete;
	void operator=(const KdMesh& src) = delete;
};

