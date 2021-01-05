#pragma once

class KdModelShader
{
public:

	// ライトOn/Off
	void SetLightEnable(bool enable)
	{
		m_cb0.Work().LightEnable = enable;
	}
	// フォグ有効/無効
	void SetFogEnable(bool enable)
	{
		m_cb0.Work().FogEnable = enable;
	}
	// 乗算色
	void SetMulMaterialColor(const KdVec4& color)
	{
		m_cb0.Work().MulMaterialColor = color;
	}

	// UVオフセット設定
	void SetUVOffset(const KdVec2& uvOffset)
	{
		m_cb0.Work().UVOffset = uvOffset;
	}
	// UVタイリング設定
	void SetUVTiling(const KdVec2& uvOffset)
	{
		m_cb0.Work().UVTiling = uvOffset;
	}
	// 屈折率設定
	void SetRefractiveIndex(float refractiveIndex)
	{
		m_cb0.Work().RefractiveIndex = refractiveIndex;
	}


	// 初期化
	bool Init();
	// 解放
	void Release();
	~KdModelShader() { Release(); }

	//================================================
	// 描画
	//================================================
	
	// このシェーダを使用できるようにデバイスへセット
	void SetToDevice();

	// １つのメッシュを描画
	// ・mesh … 描画するメッシュ
	// ・materials … 使用する材質配列
	void DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials);
	
	// ノードアウトライナー内の全てのメッシュを描画する
	// また、DrawMeshとは違い関数内でSetWorldMatrixも実行される
	// ・nodeOL … 描画するアウトライナー
	// ・materials … 描画にしようするマテリアル配列
	// ・worldTransform … ワールド変換行列 各ノードはこの行列基準で変換され描画される
	void Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform);

	//================================================
	// 輪郭描画
	//================================================
	// 輪郭描画としてデバイスへセット
	void SetToDevice_Outline();
	// １つのメッシュを描画
	void DrawMesh_Outline(const KdMesh* mesh);
	// ノードアウトライナー内の全てのメッシュを描画する
	void Draw_Outline(const KdNodeOutliner* nodeOL, const KdMatrix& worldTransform);

private:
	ID3D11VertexShader* m_VS = nullptr;				// 頂点シェーダー
	ID3D11InputLayout* m_inputLayout = nullptr;		// 頂点入力レイアウト

	ID3D11VertexShader* m_VS_Skin = nullptr; // (スキンメッシュ)頂点シェーダー
	ID3D11InputLayout* m_inputLayout_Skin = nullptr; // (スキンメッシュ)頂点入力レイアウト

	ID3D11PixelShader* m_PS = nullptr;				// ピクセルシェーダー

	// 輪郭描画用
	ID3D11VertexShader* m_outlineVS = nullptr; // 頂点シェーダー
	ID3D11PixelShader* m_outlinePS = nullptr; // ピクセルシェーダー

	KdSptr<KdTexture> m_texToon;					// トゥーン用のテクスチャ

	// 定数バッファ用の構造体
	struct cbObject
	{
		KdMatrix	mW;	// ワールド行列
		int LightEnable = 1;	// ライト有効
		int FogEnable = 1;		// フォグ有効
		float _space_[2]; // パッキング規則を守る必要あるため、ゴミを入れておく

		KdVec4 MulMaterialColor = { 1,1,1,1 }; // 乗算色

		KdVec2 UVOffset; // UVオフセット
		KdVec2 UVTiling = { 1,1 }; // UVタイリング
		float RefractiveIndex = 1.0f;// 屈折率
		float _space2_[3]; // パッキング規則を守る必要あるため、ゴミを入れておく
	};
	// 定数バッファ
	KdConstantBuffer<cbObject> m_cb0;

	// 定数バッファ（マテリアル単位更新）
	struct cbMaterial {
		KdVec4			BaseColor;
		KdVec3			Emissive;
		float			Matallic;
		float			Roughness;

		float			tmp[3];
	};
	KdConstantBuffer<cbMaterial>	m_cb1_Material;
};