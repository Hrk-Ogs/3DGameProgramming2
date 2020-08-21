#include "System/KdSystem.h"

#include "KdShaderManager.h"

void KdShaderManager::Init()
{

	//============================================
	// シェーダ
	//============================================

	// 基本シェーダ
	m_standardShader.Init();
	// エフェクトシェーダ
	m_effectShader.Init();
	// 2Dスプライトシェーダ
	m_spriteShader.Init();
	// モデルシェーダー
	m_modelShader.Init();


	//============================================
	// 定数バッファ
	//============================================

	// カメラ
	m_cb7_Camera.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());

	// 初期ビュー行列
	m_cb7_Camera.Work().mV.CreateTranslation(0, 0, -5);
	m_cb7_Camera.Work().mV.Inverse();

	// 初期射影行列
	m_cb7_Camera.Work().mP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 1000);

	m_cb7_Camera.Write();

	// ライト
	m_cb8_Light.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());

	//============================================
	// ステート作成
	//============================================

	// サンプラステート作成
	m_ss0_Anisotropic_Wrap = D3D.CreateSamplerState(KdSamplerFilterMode::Anisotropic, 4, KdSamplerAddressingMode::Wrap, false);
	m_ss1_Anisotropic_Clamp = D3D.CreateSamplerState(KdSamplerFilterMode::Anisotropic, 4, KdSamplerAddressingMode::Clamp, false);
	m_ss2_Linear_Wrap = D3D.CreateSamplerState(KdSamplerFilterMode::Linear, 0, KdSamplerAddressingMode::Wrap, false);
	m_ss3_Linear_Clamp = D3D.CreateSamplerState(KdSamplerFilterMode::Linear, 0, KdSamplerAddressingMode::Clamp, false);
	D3D.GetDevContext()->PSSetSamplers(0, 1, &m_ss0_Anisotropic_Wrap);
	D3D.GetDevContext()->PSSetSamplers(1, 1, &m_ss1_Anisotropic_Clamp);
	D3D.GetDevContext()->PSSetSamplers(2, 1, &m_ss2_Linear_Wrap);
	D3D.GetDevContext()->PSSetSamplers(3, 1, &m_ss3_Linear_Clamp);

	m_ss10_Comparison = D3D.CreateSamplerState(KdSamplerFilterMode::Linear, 0, KdSamplerAddressingMode::Clamp, true);
	D3D.GetDevContext()->PSSetSamplers(10, 1, &m_ss10_Comparison);

	// デプスステンシルステート作成
	m_ds_ZEnable_ZWriteEnable = D3D.CreateDepthStencilState(true, true);
	m_ds_ZEnable_ZWriteDisable = D3D.CreateDepthStencilState(true, false);
	m_ds_ZDisable_ZWriteDisable = D3D.CreateDepthStencilState(false, false);

	// ラスタライザステート作成
	m_rs_CullBack = D3D.CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, false);
	m_rs_CullFront = D3D.CreateRasterizerState(D3D11_CULL_FRONT, D3D11_FILL_SOLID, true, false);
	m_rs_CullNone = D3D.CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID, true, false);
	m_rs_Wireframe = D3D.CreateRasterizerState(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, true, false);
	m_rs_CullBack_NoZClip = D3D.CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, false, false);


	// ブレンドステート作成
	m_bs_Alpha = D3D.CreateBlendState(KdBlendMode::Alpha);
	m_bs_Add = D3D.CreateBlendState(KdBlendMode::Add);
}

void KdShaderManager::Release()
{
	m_standardShader.Release();
	m_effectShader.Release();
	m_spriteShader.Release();
	m_modelShader.Release();



	m_cb7_Camera.Release();
	m_cb8_Light.Release();

	KdSafeRelease(m_ss0_Anisotropic_Wrap);
	KdSafeRelease(m_ss1_Anisotropic_Clamp);
	KdSafeRelease(m_ss2_Linear_Wrap);
	KdSafeRelease(m_ss3_Linear_Clamp);
	KdSafeRelease(m_ss10_Comparison);

	KdSafeRelease(m_ds_ZEnable_ZWriteEnable);
	KdSafeRelease(m_ds_ZEnable_ZWriteDisable);
	KdSafeRelease(m_ds_ZDisable_ZWriteDisable);

	KdSafeRelease(m_rs_CullBack);
	KdSafeRelease(m_rs_CullFront);
	KdSafeRelease(m_rs_CullNone);
	KdSafeRelease(m_rs_Wireframe);
	KdSafeRelease(m_rs_CullBack_NoZClip);

	KdSafeRelease(m_bs_Alpha);
	KdSafeRelease(m_bs_Add);
}
