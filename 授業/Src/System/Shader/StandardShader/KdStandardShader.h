#pragma once

//============================================================
//
// 基本シェーダ
//
//============================================================
class KdStandardShader
{
public:

	//================================================
	// 設定・取得
	//================================================

	// ワールド行列セット
	void SetWorldMatrix(const Math::Matrix& m)
	{
		m_cb0.Work().mW = m;
//		m_mTransform = m;
	}

	void SetBoneMatrix(int idx, const KdMatrix& m)
	{
		m_cb2_Bone.Work().BoneMatrices[idx] = m;
	}

	// UVタイリング設定
	void SetUVTiling(const Math::Vector2& tiling)
	{
		m_cb0.Work().UVTiling = tiling;
	}
	// UVオフセット設定
	void SetUVOffset(const Math::Vector2& offset)
	{
		m_cb0.Work().UVOffset = offset;
	}

	// ライト有効/無効
	void SetLightEnable(bool enable)
	{
		m_cb0.Work().LightEnable = enable;
	}

	// フォグ有効/無効
	void SetFogEnable(bool enable)
	{
		m_cb0.Work().FogEnable = enable;
	}

	// アルファテストの閾値設定
	// ・alpha	… この値以下のアルファ値のピクセルは描画されなくなる
	void SetAlphaTestThreshold(float alpha)
	{
		m_cb0.Work().AlphaTestThreshold = alpha;
	}

	// 乗算色
	void SetMulMaterialColor(const KdVec4& color)
	{
		m_cb0.Work().MulMaterialColor = color;
	}

	//---------------------
	// 材質パラメータ設定
	//---------------------
	/*
	// 基本色設定
	void SetBaseColor(const Math::Vector4& color)
	{
		m_cb1_Material.Work().BaseColor = color;
	}

	// 自己発光色設定
	void SetEmissive(const Math::Vector3& color)
	{
		m_cb1_Material.Work().Emissive = color;
	}

	// 粗さの設定
	void SetRoughness(float roughness)
	{
		m_cb1_Material.Work().Roughness = roughness;
	}

	// 金属度の設定
	void SetMetallic(float metallic)
	{
		m_cb1_Material.Work().Metallic = metallic;
	}
	*/


	// Set系でセットしたデータを、実際に定数バッファへ書き込む
	void WriteToCB()
	{
		m_cb0.Write();
	}


	//================================================
	// 描画
	//================================================

	// このシェーダをデバイスへセット
	void SetToDevice();

	// メッシュ描画
	// ・mesh			… 描画するメッシュ
	// ・materials		… 使用する材質配列
	void DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials, bool isSkinMesh);

	// 指定ノードアウトライナーを描画する
	// 　また、DrawMeshとは違い関数内でSetWorldMatrixも実行される
	// ・nodeOL			… 描画するアウトライナー
	// ・materials		… 描画にしようするマテリアル配列
	// ・worldTransform	… ワールド変換行列　各ノードはこの行列基準で変換され描画される
	void Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform);

	//================================================
	// 初期化・解放
	//================================================

	// 初期化
	bool Init();
	// 解放
	void Release();
	// 
	~KdStandardShader()
	{
		Release();
	}


private:

	// 3Dモデル用シェーダ
	ID3D11VertexShader*	m_VS = nullptr;				// 頂点シェーダー
	ID3D11InputLayout* m_inputLayout = nullptr;		// 頂点入力レイアウト

	ID3D11VertexShader* m_VS_Skin = nullptr;			// 頂点シェーダー(スキンメッシュ)
	ID3D11InputLayout* m_inputLayout_Skin = nullptr;	// 頂点入力レイアウト(スキンメッシュ)

	ID3D11PixelShader*	m_PS = nullptr;				// ピクセルシェーダー

	// 行列保持用
	Math::Matrix		m_mTransform;

	// 定数バッファ
	//  ※定数バッファは、パッキング規則というものを厳守しなければならない
	//  <パッキング規則> 参考：https://docs.microsoft.com/ja-jp/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules
	//  ・構造体のサイズは16バイトの倍数にする。
	//  ・各項目(変数)は、16バイト境界をまたぐような場合、次のベクトルに配置される。

	// 定数バッファ(オブジェクト単位更新)
	struct cbObject
	{
		Math::Matrix		mW;		// ワールド行列　行列は16バイトx4バイトの64バイトなのでピッタリ。

		// UV操作
		Math::Vector2		UVOffset = { 0,0 };
		Math::Vector2		UVTiling = { 1,1 };

		// ライト有効
		int					LightEnable = 1;
		// フォグ有効
		int					FogEnable = 1;
		// アルファテストの閾値
		float				AlphaTestThreshold = 0;
			float tmp[1];
		// 乗算色
		KdVec4				MulMaterialColor = { 1,1,1,1 };
	};
	KdConstantBuffer<cbObject>	m_cb0;

	// 定数バッファ(マテリアル単位更新)
	struct cb {
		Math::Vector4	BaseColor;
		Math::Vector3	Emissive;
		float			Metallic;
		float			Roughness;

		float			tmp[3];
	};
	KdConstantBuffer<cb>	m_cb1_Material;

	// 定数バッファ(ボーン行列)
	struct cbBone {
		std::array<Math::Matrix, 500>	BoneMatrices;
	};
	KdConstantBuffer<cbBone>	m_cb2_Bone;

	// マテリアルが存在しないときの代役
	KdMaterial	m_nullMaterial;
};


