﻿#pragma once

#include "EffectShader/KdEffectShader.h"
#include "StandardShader/KdStandardShader.h"
#include "SpriteShader/KdSpriteShader.h"

#include "ModelShader/KdModelShader.h"
#include "PostProcessShader/KdPostProcessShader.h"
#include "GenShadowMapShader/KdGenShadowMapShader.h"

//==========================================================
//
// シェーダ関係をまとめたクラス
//
//==========================================================
class KdShaderManager
{
public:

	//==============================================================
	//
	// 初期化・解放
	//
	//==============================================================

	// 初期化
	void Init();

	// 解放
	void Release();

	//==========================
	//
	// シェーダ
	//
	//==========================
	KdStandardShader		m_standardShader;		// 3Dモデル描画シェーダ
	KdEffectShader			m_effectShader;			// エフェクト描画シェーダ
	KdSpriteShader			m_spriteShader;			// 2Dテクスチャ描画シェーダ
	KdModelShader			m_modelShader;			// モデル描画シェ-ダー
	KdPostProcessShader		m_postProcessShader;
	KdGenShadowMapShader	m_genShadowMapShader;

	//==========================
	//
	// カメラ関係
	//
	//==========================

	// カメラ定数バッファ用構造体
	struct cbCamera
	{
		KdMatrix			mV;			// ビュー行列
		KdMatrix			mP;			// 射影行列
		KdVec3				CamPos;		// カメラワールド座標

		int					DistanceFogEnable = 0;			// 距離フォグ有効
		KdVec3				DistanceFogColor = { 1, 1, 1 };	// 距離フォグ色
		float				DistanceFogDensity = 0.001f;	// 距離フォグ減衰率

			// ミラー描画用
		int					CullEnable = 0;
		KdVec3				CullDir;
		KdVec3				CullPos;
		float				tmp;
	};

	// カメラ定数バッファ
	KdConstantBuffer<cbCamera>	m_cb7_Camera;


	//==========================
	//
	// ライト関係
	//
	//==========================

	// ライト定数バッファ用構造体
	struct cbLight
	{
		int DL_Cnt = 0;	// 使用する平行光の数
		int PL_Cnt = 0;	// 使用する点光の数
		int SL_Cnt = 0;	// 使用する集中光の数
		float tmp;

		//--------------------------
		// 環境光
		//--------------------------
		KdVec3				AmbientLight = {0.2f, 0.2f, 0.2f};
		float					tmp2;

		//--------------------------
		// 平行光
		//--------------------------
		struct DirectionalLight
		{
			KdVec3	Dir = { 0,-1,0 };    // 光の方向
				float	tmp2;
			KdVec3	Color = { 3,3,3 };  // 光の色
			float EnableShadow; // シャドウマッピング有効
			KdMatrix mLightVP[4]; // ライトカメラのビュー*射影行列
			KdVec4 CascadeDist; // カスケード分割距離
			int CascadeNum = 4; // カスケード分割数
			float _space_[3];
		};
		std::array<DirectionalLight, 3>	DL;

		//--------------------------
		// 点光
		//--------------------------
		struct PointLight
		{
			KdVec3	Color;			// 色
			float	Radius;			// 半径
			KdVec3	Pos;			// 座標
			float tmp;
		};
		std::array<PointLight, 100>	PL;

		//--------------------------
		// 集中光
		//--------------------------
		struct SpotLight
		{
			KdVec3 Dir;				// 光の方向
			float Range;			// 範囲
			KdVec3 Color;			// 色
			float InnerCorn;		// 内側の角度
			KdVec3 Pos;				// 座標
			float OuterCorn;		// 外側の角度

			KdMatrix mLightVP;		// ライトカメラのビュー*射影行列
			float EnableShadow; // シャドウマッピング有効
			float _space_[3];
		};
		std::array<SpotLight, 30>	SL;

	};

	// ライト定数バッファ
	KdConstantBuffer<cbLight>		m_cb8_Light;

	//==========================
	//
	// パイプラインステート関係
	//
	//==========================

	// サンプラ
	ID3D11SamplerState* m_ss0_Anisotropic_Wrap = nullptr;
	ID3D11SamplerState* m_ss1_Anisotropic_Clamp = nullptr;

	ID3D11SamplerState* m_ss2_Linear_Wrap = nullptr;
	ID3D11SamplerState* m_ss3_Linear_Clamp = nullptr;

	ID3D11SamplerState* m_ss10_Comparison = nullptr;

	// デプスステンシル
	ID3D11DepthStencilState* m_ds_ZEnable_ZWriteEnable = nullptr;
	ID3D11DepthStencilState* m_ds_ZEnable_ZWriteDisable = nullptr;
	ID3D11DepthStencilState* m_ds_ZDisable_ZWriteDisable = nullptr;

	// ラスタライズステート
	ID3D11RasterizerState* m_rs_CullBack = nullptr;
	ID3D11RasterizerState* m_rs_CullFront = nullptr;
	ID3D11RasterizerState* m_rs_CullNone = nullptr;
	ID3D11RasterizerState* m_rs_Wireframe = nullptr;

	ID3D11RasterizerState* m_rs_CullBack_NoZClip = nullptr;

	// ブレンドステート
	ID3D11BlendState* m_bs_Alpha = nullptr;
	ID3D11BlendState* m_bs_Add = nullptr;

	//==========================
	//
	// その他
	//
	//==========================





//-------------------------------
// シングルトン
//-------------------------------
private:

	KdShaderManager() {}

public:
	static KdShaderManager &GetInstance() {
		static KdShaderManager instance;
		return instance;
	}
};

#define SHADER KdShaderManager::GetInstance()
