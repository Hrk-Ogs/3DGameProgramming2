#pragma once

class KdModelShader
{
public:
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

private:
	ID3D11VertexShader* m_VS = nullptr;				// 頂点シェーダー
	ID3D11InputLayout* m_inputLayout = nullptr;		// 頂点入力レイアウト

	ID3D11PixelShader* m_PS = nullptr;				// ピクセルシェーダー

	// 定数バッファ用の構造体
	struct cbObject
	{
		KdMatrix	mW;	// ワールド行列
	};
	// 定数バッファ
	KdConstantBuffer<cbObject> m_cb0;
};