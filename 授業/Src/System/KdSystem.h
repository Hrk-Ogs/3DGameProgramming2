#pragma once

// 開発Version
#define DEVELOPMENT_MODE

//====================================================
//
// システム系のヘッダーファイル
//
//====================================================

// 便利機能
#include "Utility/KdUtility.h"

// 数学
#include "Math/KdMath.h"

// バッファ
#include "Direct3D/KdBuffer.h"
// Direct3D
#include "Direct3D/KdDirect3D.h"

// アニメーション
#include "Animation/KdAnimation.h"

// テクスチャ
#include "Direct3D/KdTexture.h"
// メッシュ
#include "Direct3D/KdMesh.h"
// モデル
#include "Direct3D/KdModel.h"
// 板ポリゴン
#include "Direct3D/KdSquarePolygon.h"
// 連結ポリゴン
#include "Direct3D/KdTrailPolygon.h"

// シェーダ
#include "Shader/KdShaderManager.h"

// デバッグ描画
#include "Direct3D/KdDebugLineDraw.h"

// ウィンドウ
#include "Window/KdWindow.h"

// オーディオ
#include "Audio/KdAudio.h"

// リソース管理
#include "Utility/KdResourceFactory.h"

// 当たり判定
#include "Math/KdCollision.h"

// JSONヘルパー
#include "Utility/KdJsonHelper.h"
