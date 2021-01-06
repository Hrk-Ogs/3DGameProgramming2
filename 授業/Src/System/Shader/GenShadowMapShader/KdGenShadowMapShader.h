#pragma once

//================================================
// シャドウマップ生成シェーダー
//================================================
class KdGenShadowMapShader
{
public:

	// 集中光用深度マップ取得
	KdSptr<KdTexture> GetSpotShadowMap() const { return m_spotLightShadowMap; }
	// 平行光用深度マップ取得
	KdSptr<KdTexture> GetDirShadowMap() const { return m_dirLightShadowMap; }

	// スポットライト用の深度マップをRTとしてセット(Zバッファも)
	void SetRenderTarget_SpotShadowMap(int idx)
	{
		// RT変更
		D3D.GetDevContext()->OMSetRenderTargets(1, m_spotLightShadowMap->GetRTViewAddress(idx), m_spotLightZBuffer->GetDSView(idx));
		D3D.SetViewportFromRTV(m_spotLightShadowMap->GetRTView(idx));
		// クリア
		D3D.GetDevContext()->ClearRenderTargetView(m_spotLightShadowMap->GetRTView(idx), KdVec4(1, 1, 1, 1));
		D3D.GetDevContext()->ClearDepthStencilView(m_spotLightZBuffer->GetDSView(idx), D3D11_CLEAR_DEPTH, 1, 0);
	}

	// 平行ライト用の深度マップをRTとしてセット(Zバッファも)
	void SetRenderTarget_DirShadowMap(int lightIdx, int cascadeIdx)
	{
		int idx = lightIdx * 4 + cascadeIdx;

		// RT変更
		D3D.GetDevContext()->OMSetRenderTargets(1, m_dirLightShadowMap->GetRTViewAddress(idx), m_dirLightZBuffer->GetDSView(idx));
		D3D.SetViewportFromRTV(m_dirLightShadowMap->GetRTView(idx));
		// クリア
		D3D.GetDevContext()->ClearRenderTargetView(m_dirLightShadowMap->GetRTView(idx), KdVec4(1, 1, 1, 1));
		D3D.GetDevContext()->ClearDepthStencilView(m_dirLightZBuffer->GetDSView(idx), D3D11_CLEAR_DEPTH, 1, 0);
	}

	// ライトカメラ行列(ビュー行列 * 射影行列)セット
	void SetLightVP(const KdMatrix& m)
	{
		m_cb0.Work().mLightVP = m;
	}

	//================================================
	// 描画
	//================================================
	// このシェーダをデバイスへセット
	void SetToDevice();

	// メッシュ描画
	// ・mesh … 描画するメッシュ
	// ・materials … 使用する材質配列
	void DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials, bool isSkinMesh);

	// モデル描画
	void Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform);

	//================================================
	// 初期化・解放
	//================================================
	// 初期化
	bool Init();
	// 解放
	void Release();

	~KdGenShadowMapShader()
	{
		Release();
	}

private:
	ID3D11VertexShader* m_VS = nullptr; // 頂点シェーダー
	ID3D11InputLayout* m_inputLayout = nullptr; // 頂点入力レイアウト
	ID3D11VertexShader* m_VS_Skin = nullptr; //頂点シェーダー(スキンメッシュ)
		ID3D11InputLayout* m_inputLayout_Skin = nullptr; // 頂点入力レイアウト(スキンメッシュ)

	ID3D11PixelShader* m_PS = nullptr; // ピクセルシェーダー

	// 深度マップ
	KdSptr<KdTexture> m_spotLightShadowMap; // 集中光用 深度マップ
	KdSptr<KdTexture> m_spotLightZBuffer; // 集中光用 Zバッファ
	KdSptr<KdTexture> m_dirLightShadowMap; // 平行光用 深度マップ
	KdSptr<KdTexture> m_dirLightZBuffer; // 平行光用 Zバッファ

	// 定数バッファ
	struct cbObject
	{
		KdMatrix mW; // ワールド行列
		KdMatrix mLightVP; // ライトカメラのビュー行列 * 射影行列
	};
	KdConstantBuffer<cbObject> m_cb0;
};