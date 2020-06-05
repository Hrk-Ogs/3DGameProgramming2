#pragma once


//============================================================
//
// エフェクトシェーダ
//
//============================================================
class KdEffectShader
{
public:

	// １頂点の形式
	struct Vertex
	{
		KdVec3 Pos;
		KdVec2 UV;
		KdVec4 Color;
	};

	//================================================
	// 取得・設定
	//================================================

	// テクスチャセット
	void SetTexture(ID3D11ShaderResourceView* srv);

	// ワールド行列セット
	void SetWorldMatrix(const KdMatrix& m)
	{
		m_cb0.Work().mW = m;
	}

	// UV Offsetセット
	void SetUVOffset(const KdVec2& offset)
	{
		m_cb0.Work().UVOffset = offset;
	}

	// UV Tilingセット
	void SetUVTiling(const KdVec2& tiling)
	{
		m_cb0.Work().UVTiling = tiling;
	}

	// 乗算色設定
	void SetColor(const KdVec4& color)
	{
		m_cb0.Work().Color = color;
	}

	// Set系でセットしたデータを、実際に定数バッファへの書き込む
	void WriteToCB()
	{
		m_cb0.Write();
	}

	//================================================
	// 描画関係
	//================================================

	// このシェーダをデバイスへセット
	void SetToDevice();

	// 3D線描画
	void DrawLine(const KdVec3& p1, const KdVec3& p2, const Math::Color& color);

	// 3D線描画
	void DrawVertices(const std::vector<Vertex>& vertices, D3D_PRIMITIVE_TOPOLOGY topology);

	// メッシュ描画
	// ・mesh			… 描画するメッシュ
	// ・materials		… 使用する材質配列
	void DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials);

	//================================================
	// 
	//================================================

	// 初期化
	bool Init();

	// 解放
	void Release();

	// 
	~KdEffectShader()
	{
		Release();
	}


private:

	// 3Dモデル用シェーダ
	ID3D11VertexShader*	m_VS = nullptr;				// 頂点シェーダー
	ID3D11InputLayout*	m_inputLayout = nullptr;		// 頂点入力レイアウト
	ID3D11InputLayout*	m_inputLayout_Model = nullptr;	// Model用頂点入力レイアウト

	ID3D11PixelShader*	m_PS = nullptr;				// ピクセルシェーダー

	// 定数バッファ
	//  ※定数バッファは、パッキング規則というものを厳守しなければならない
	//  <パッキング規則> 参考：https://docs.microsoft.com/ja-jp/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules
	//  ・構造体のサイズは16バイトの倍数にする。
	//  ・各項目(変数)は、16バイト境界をまたがないようにする。

	// 定数バッファ(オブジェクト単位更新)
	struct cbObject
	{
		KdMatrix		mW;		// ワールド行列　行列は16バイトx4バイトの64バイトなのでピッタリ。

		KdVec4			Color = { 1,1,1,1 };

		KdVec2			UVOffset = { 0, 0 };
		KdVec2			UVTiling = { 1, 1 };
	};
	KdConstantBuffer<cbObject>	m_cb0;

	// マテリアルが存在しないときの代役
	KdMaterial	m_nullMaterial;
};


