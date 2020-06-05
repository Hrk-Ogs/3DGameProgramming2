#pragma once

//==========================================================
//
// ノードアウトライナー
// ・Blenderのアウトライナーウィンドウのように、複数のノードを保持するクラス。階層構造にもなっている。
// ・Blenderでメッシュが付いているノードには、同じくKdMeshが付いている。
//
//==========================================================
class KdNodeOutliner
{
public:

	//---------------------
	// １つのノード
	//---------------------
	struct Node
	{
		// ノード名(ボーン名)
		std::string				Name;

		// 先頭から何番目のノードか？
		int						NodeIndex = -1;
		// ボーンノードの時、先頭から何番目のボーンか？
		int						BoneIndex = -1;

		// 変換行列
		KdMatrix				LocalTransform;
		KdMatrix				Transform;
		KdMatrix				BoneInverseBindMatrix;	// ボーンオフセット行列

		// メッシュ
		KdSptr<KdMesh>			Mesh;

		// 子Indexリスト
		std::vector<int>		Children;
		// 親Index
		int						Parent = -1;
	};

	// 全ノードの配列取得
	const std::vector<Node>&	GetAllNodes() const { return m_allNodes; }
	std::vector<Node>&			GetAllNodes() { return m_allNodes; }

	// RootノードのみのノードIndex配列取得
	const std::vector<int>&		GetRootNodeIndices() const { return m_rootNodeIndices; }

	// ボーンのみのノードIndex配列取得
	const std::vector<int>&		GetBoneNodeIndices() const { return m_boneNodeIndices; }

	// メッシュが存在するノードIndex配列取得
	const std::vector<int>&		GetMeshNodeIndices() const { return m_meshNodeIndices; }

	// ノードをセット
	void SetNodes(const std::vector<Node>& nodes);

	// 
	const KdStructuredBuffer<KdMatrix>& GetBoneStructuredBuffer() const { return m_sbBones; }

	// 解放
	void Release()
	{
		m_allNodes.clear();
		m_rootNodeIndices.clear();
		m_boneNodeIndices.clear();
		m_meshNodeIndices.clear();

		m_sbBones.Release();
	}

	// 指定名のノードを検索
	Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_allNodes)
		{
			if (node.Name == name)return &node;
		}
		return nullptr;
	}

	// ノード行列を計算(指定nodeから計算)
	void CalcNodeMatrices(Node* node);
	// ノード行列を計算(Rootから計算)
	// ・親から子へと、どんどん計算していく
	void CalcNodeMatrices();

	// ボーンノードの全描画用行列をバッファに書き込む
	void WriteToBoneBuffer()
	{
		m_sbBones.Write();
	}

	// 
	KdNodeOutliner() = default;

	//=================================
	// コピー
	//=================================
	void operator=(const KdNodeOutliner& src)
	{
		SetNodes(src.GetAllNodes());
	}
	KdNodeOutliner(const KdNodeOutliner& src)
	{
		SetNodes(src.GetAllNodes());
	}

private:

	//----------------------------
	// 全てのノード配列
	//----------------------------
	std::vector<Node>				m_allNodes;

		// 全ノード中、RootノードのみのIndex配列
		std::vector<int>				m_rootNodeIndices;
		// 全ノード中、ボーンノードのみのIndex配列
		std::vector<int>				m_boneNodeIndices;
		// 全ノード中、メッシュが存在するノードのみのIndexn配列
		std::vector<int>				m_meshNodeIndices;

	// ボーン変換行列バッファ
	KdStructuredBuffer<KdMatrix>		m_sbBones;
};

//==========================================================
// マテリアル(glTFベースのPBRマテリアル)
//==========================================================
struct KdMaterial
{
	//---------------------------------------
	// 材質データ
	//---------------------------------------

	// 名前
	std::string					Name;

	// 基本色
	std::shared_ptr<KdTexture>	BaseColorTex;				// 基本色テクスチャ
	KdVec4						BaseColor = { 1,1,1,1 };	// 基本色のスケーリング係数(RGBA)

	// 金属性、粗さ
	std::shared_ptr<KdTexture>	MetallicRoughnessTex;		// B:金属製 G:粗さ
	float						Metallic = 0.0f;			// 金属性のスケーリング係数
	float						Roughness = 1.0f;			// 粗さのスケーリング係数

	// 自己発光
	std::shared_ptr<KdTexture>	EmissiveTex;				// 自己発光テクスチャ
	KdVec3						Emissive = { 0,0,0 };		// 自己発光のスケーリング係数(RGB)

	// 法線マップ
	std::shared_ptr<KdTexture>	NormalTex;
	std::shared_ptr<KdTexture>	ForceUseNormalTex;

	KdMaterial()
	{
		BaseColorTex			= D3D.GetWhiteTex();
		MetallicRoughnessTex	= D3D.GetWhiteTex();
		EmissiveTex				= D3D.GetWhiteTex();
		NormalTex				= D3D.GetDefNormalTex();
	}
};

//==========================================================
//
// ノード情報やマテリアル配列を組み合わせ、１つのモデルデータとしたクラス
// ・Blenderのように、ノードは複数存在する。また階層構造にもなっている(m_nodeOutliner)。
// ・Blenderでメッシュが付いているノードには、同じくKdMeshが付いている。
// ・マテリアルの配列を持っている。
// ・アニメーションの配列を持っている。
//
//==========================================================
class KdModel
{
public:

	//==========================================================
	// 取得
	//==========================================================

	// 有効なメッシュか？
	bool IsValid() const { return m_nodeOutliner.GetAllNodes().empty() == false; }

	// ノードアウトライナー取得
	const KdNodeOutliner& GetNodeOutlinear() const { return m_nodeOutliner; }

	// マテリアル配列取得
	const std::vector<KdSptr<KdMaterial>>& GetMaterials() const { return m_materials; }


	// アニメーションリスト取得
	const std::vector<KdSptr<KdAnimationData>>& GetAnimations() const { return m_animations; }

	// アニメーションを名前から取得
	const KdSptr<KdAnimationData> GetAnimation(const std::string& animeName) const
	{
		for (auto&& anim : m_animations)
		{
			if (anim->GetName() == animeName)return anim;
		}
		return nullptr;
	}

	// ファイルパス取得
	const std::string& GetFilepath() const { return m_filepath; }

	//==========================================================
	// 作成・解放
	//==========================================================

	// glTF形式の3Dモデルをロードする
	// ・filename		… ファイル名
	// 戻り値			… 成功：true
	bool Load(const std::string& filename);

	// 解放
	void Release()
	{
		m_materials.clear();
		m_filepath = "";
		m_animations.clear();
		m_nodeOutliner.Release();
	}

	~KdModel() {
		Release();
	}

	// 
	KdModel() {}

private:

	// ノードアウトライナー
	// ・全ノードを管理している。メッシュもこの中のいずれかに入っている。
	KdNodeOutliner							m_nodeOutliner;

	// マテリアル配列
	std::vector<KdSptr<KdMaterial>>			m_materials;

	// アニメーションデータリスト
	std::vector<KdSptr<KdAnimationData>>	m_animations;

	// ファイルパス
	std::string								m_filepath;

private:
	// コピー禁止用
	KdModel(const KdModel& src) = delete;
	void operator=(const KdModel& src) = delete;
};
