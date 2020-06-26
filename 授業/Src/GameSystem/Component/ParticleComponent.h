#pragma once

#include "BaseComponent.h"

//=======================================================
//
// パーティクスコンポーネント
// ・大量の板ポリゴンを発生・管理・描画するエフェクト用のエミッター。
//
//=======================================================
class ParticleComponent : public BaseComponent
{
public:
	// エフェクトを最初から再生する
	void Restart()
	{
		m_time = -m_param.DelayTime;
		m_nodes.clear();
	}
	//===============================
	// コンポーネント処理
	//===============================
	// 更新処理
	virtual void Update() override;
	// 描画準備
	virtual void PrepareDraw(RenderingData& rdata) override;
	// 描画処理
	virtual void Draw(int phaseID) override;
	// (Editor用)ImGui
	virtual void Editor_ImGuiUpdate() override;
	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);
		KdJsonGet(jsonData["Param.Duration"], m_param.Duration);
		KdJsonGet(jsonData["Param.Loop"], m_param.Loop);
		KdJsonGet(jsonData["Param.PopupNum"], m_param.PopupNum);
		KdJsonGet(jsonData["Param.PopupInterval"], m_param.PopupInterval);
		KdJsonGet(jsonData["Param.LifeTime"], m_param.LifeTime);
		std::string texFilename;
		KdJsonGet(jsonData["Param.Tex"], texFilename);
		m_param.Tex = KDResFactory.GetTexture(texFilename);
		if (m_param.Tex == nullptr)
		{
			m_param.Tex = D3D.GetWhiteTex();
		}
		KdJsonGet(jsonData["Param.BillboardType"], m_param.BillboardType);
		KdJsonGet(jsonData["Param.BlendMode"], m_param.BlendMode);
		KdJsonGet(jsonData["Param.StopAction"], m_param.StopAction);
		KdJsonGet(jsonData["Param.Flags"], m_param.Flags);
		KdJsonGet(jsonData["Param.DelayTime"], m_param.DelayTime);
		KdJsonGet(jsonData["Param.Color"], m_param.Color);
		KdJsonGet(jsonData["Param.EndColor"], m_param.EndColor);
		// 基本
		KdJsonGet(jsonData["Param.PosStart"], m_param.PosStart);
		KdJsonGet(jsonData["Param.PosRandom"], m_param.PosRandom);
		KdJsonGet(jsonData["Param.SizeStart"], m_param.SizeStart);
		KdJsonGet(jsonData["Param.SizeRandom"], m_param.SizeRandom);
		KdJsonGet(jsonData["Param.AngleStart"], m_param.AngleStart);
		KdJsonGet(jsonData["Param.AngleRandom"], m_param.AngleRandom);
		// 速度
		KdJsonGet(jsonData["Param.MoveStart"], m_param.MoveStart);
		KdJsonGet(jsonData["Param.MoveRandom"], m_param.MoveRandom);
		KdJsonGet(jsonData["Param.ScaleStart"], m_param.ScaleStart);
		KdJsonGet(jsonData["Param.ScaleRandom"], m_param.ScaleRandom);
		KdJsonGet(jsonData["Param.RotateStart"], m_param.RotateStart);
		KdJsonGet(jsonData["Param.RotateRandom"], m_param.RotateRandom);
		// 加速度
		KdJsonGet(jsonData["Param.MoveAccelStart"], m_param.MoveAccelStart);
		KdJsonGet(jsonData["Param.MoveAccelRandom"], m_param.MoveAccelRandom);
		KdJsonGet(jsonData["Param.MoveAccelMultiply"], m_param.MoveAccelMultiply);
		KdJsonGet(jsonData["Param.RotateAccelStart"], m_param.RotateAccelStart);
		KdJsonGet(jsonData["Param.RotateAccelRandom"], m_param.RotateAccelRandom);
		KdJsonGet(jsonData["Param.ScaleAccelStart"], m_param.ScaleAccelStart);
		KdJsonGet(jsonData["Param.ScaleAccelRandom"], m_param.ScaleAccelRandom);
		// UVアニメ
		auto& uvAry = jsonData["Param.UVAnim_SliceNum"].array_items();
		if (uvAry.size() == 2)
		{
			m_param.UVAnim_SliceNum[0] = uvAry[0].int_value();
			m_param.UVAnim_SliceNum[1] = uvAry[1].int_value();
		}
		KdJsonGet(jsonData["Param.UVAnim_Loop"], m_param.UVAnim_Loop);
		KdJsonGet(jsonData["Param.UVAnim_Speed"], m_param.UVAnim_Speed);
		KdJsonGet(jsonData["Param.UVAnim_StartTime"], m_param.UVAnim_StartTime);
		// その他
		KdJsonGet(jsonData["Param.FollowTransform"], m_param.FollowTransform);
	}
	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseComponent::Serialize(outJsonObj);
		outJsonObj["Param.Duration"] = m_param.Duration;
		outJsonObj["Param.Loop"] = m_param.Loop;
		outJsonObj["Param.PopupNum"] = m_param.PopupNum;
		outJsonObj["Param.PopupInterval"] = m_param.PopupInterval;
		outJsonObj["Param.LifeTime"] = m_param.LifeTime;
		outJsonObj["Param.Tex"] = m_param.Tex->GetFilepath();
		outJsonObj["Param.BillboardType"] = m_param.BillboardType;
		outJsonObj["Param.BlendMode"] = m_param.BlendMode;
		outJsonObj["Param.StopAction"] = m_param.StopAction;
		outJsonObj["Param.Flags"] = m_param.Flags;
		outJsonObj["Param.DelayTime"] = m_param.DelayTime;
		outJsonObj["Param.Color"] = m_param.Color.ToArray();
		outJsonObj["Param.EndColor"] = m_param.EndColor.ToArray();
		// 基本
		outJsonObj["Param.PosStart"] = m_param.PosStart.ToArray();
		outJsonObj["Param.PosRandom"] = m_param.PosRandom.ToArray();
		outJsonObj["Param.SizeStart"] = m_param.SizeStart.ToArray();
		outJsonObj["Param.SizeRandom"] = m_param.SizeRandom.ToArray();
		outJsonObj["Param.AngleStart"] = m_param.AngleStart.ToArray();
		outJsonObj["Param.AngleRandom"] = m_param.AngleRandom.ToArray();
		// 速度
		outJsonObj["Param.MoveStart"] = m_param.MoveStart.ToArray();
		outJsonObj["Param.MoveRandom"] = m_param.MoveRandom.ToArray();
		outJsonObj["Param.RotateStart"] = m_param.RotateStart.ToArray();
		outJsonObj["Param.RotateRandom"] = m_param.RotateRandom.ToArray();
		outJsonObj["Param.ScaleStart"] = m_param.ScaleStart.ToArray();
		outJsonObj["Param.ScaleRandom"] = m_param.ScaleRandom.ToArray();
		// 加速度
		outJsonObj["Param.MoveAccelStart"] = m_param.MoveAccelStart.ToArray();
		outJsonObj["Param.MoveAccelRandom"] = m_param.MoveAccelRandom.ToArray();
		outJsonObj["Parm.MoveAccelMultiply"] = m_param.MoveAccelMultiply.ToArray();
		outJsonObj["Param.ScaleAccelStart"] = m_param.ScaleAccelStart.ToArray();
		outJsonObj["Param.ScaleAccelRandom"] = m_param.ScaleAccelRandom.ToArray();
		outJsonObj["Param.RotateAccellStart"] = m_param.RotateAccelStart.ToArray();
		outJsonObj["Param.RotateAccelRandom"] = m_param.RotateAccelRandom.ToArray();
		// UVアニメ
		outJsonObj["Param.UVAnim_SliceNum"] = m_param.UVAnim_SliceNum;
		outJsonObj["Param.UVAnim_Loop"] = m_param.UVAnim_Loop;
		outJsonObj["Param.UVAnim_Speed"] = m_param.UVAnim_Speed;
		outJsonObj["Param.UVAnim_StartTime"] = m_param.UVAnim_StartTime;
		// その他
		outJsonObj["Param.FollowTransform"] = m_param.FollowTransform;
	}
private:

	// 経過時間
	int m_time = 0;

	// パーティクルノード
	// ・１つの板ポリゴンデータ
	struct Node
	{
		// 四角形ポリゴン
		KdSquarePolygon Polygon;
		// 基本データ
		KdVec3 Pos; // 現在の座標
		KdVec3 Scale = { 1,1,1 }; // 現在の拡大
		KdVec3 Rotate; // 現在の回転角度(度)
		// 速度データ
		KdVec3 MoveSpeed; // 移動速度
		KdVec3 RotateSpeed; // 角速度
		KdVec3 ScaleSpeed; // 拡大速度
		// 加速度データ
		KdVec3 MoveAccelSpeed; // 移動加速度
		KdVec3 RotateAccelSpeed; // 角加速度
		KdVec3 ScaleAccelSpeed; // 拡大加速度
		int Time = 0; // 経過時間
	};
	// パーティクルノード配列
	std::list<Node> m_nodes;
	//-------------------
	// 動作設定
	//-------------------
	struct Parameter
	{
		int Duration = 300; // 継続時間
		bool Loop = true; // ループ再生
		int PopupNum = 3; // 発生数
		int PopupInterval = 5; // 発生間隔
		int LifeTime = 60; // 寿命
		
		// テクスチャ
		KdSptr<KdTexture> Tex = D3D.GetWhiteTex();
		
		// UVアニメーション
		std::array<int, 2> UVAnim_SliceNum = { 1, 1 };
		float UVAnim_Speed = 0.0f;
		float UVAnim_StartTime = 0;
		bool UVAnim_Loop = false;
		
		// ビルボードタイプ
		int BillboardType = 0; // 0:なにもしない 1:カメラに向ける 2:Y軸固定

		// ブレンドタイプ
		int BlendMode = 0; // 0:半透明合成 1:加算合成

		// 色
		KdVec4 Color = { 1,1,1,1 };

		// 色(終了)
		KdVec4 EndColor = { 1,1,1,1 };

		// 停止時の処理
		int StopAction = 0;

		// その他フラグ
		int Flags = 0;

		// 遅延時間
		int DelayTime = 0;

		// TransformComponentに追従
		bool FollowTransform = true;

		// 基本設定
		KdVec3 PosStart; // 初期 座標
		KdVec3 PosRandom; // PosStartからのランダム値
		KdVec3 SizeStart = { 1.0f, 1.0f, 1.0f }; // 初期 拡大
		KdVec3 SizeRandom; // SizeStartからのランダム値
		KdVec3 AngleStart; // 初期 回転角度
		KdVec3 AngleRandom; // AngleRandomからのランダム値
		// 速度
		KdVec3 MoveStart; // 初期 移動速度
		KdVec3 MoveRandom = { 0.1f, 0.1f, 0.1f }; // MoveStartからのランダム値
		KdVec3 RotateStart; // 回転加速
		KdVec3 RotateRandom; // RotateStartからのランダム値
		KdVec3 ScaleStart; // 拡大加速
		KdVec3 ScaleRandom; // ScaleStartからのランダム値
		// 加速設定
		KdVec3 MoveAccelStart; // 初期 移動加速
		KdVec3 MoveAccelRandom; // MoveAccelStartからのランダム値
		KdVec3 MoveAccelMultiply = { 1,1,1 };	// 移動乗算
		KdVec3 RotateAccelStart; // 初期 回転加速
		KdVec3 RotateAccelRandom; // RotateAccelStartからのランダム値
		KdVec3 ScaleAccelStart; // 初期 拡大加速
		KdVec3 ScaleAccelRandom; // ScaleAccelStartからのランダム値
	};
	Parameter m_param;
	
	
//-------------------------
	// 定数
	//-------------------------
	// BillboardType定数
	enum {
		kBBType_None = 0,
		kBBType_Billboard = 1, // 完全にカメラに合わせる
		kBBType_FixedYAxis = 2, // Y軸は固定して、その他をカメラに合わせる
	};
	// BlendType定数
	enum {
		kBlendType_Alpha = 0, // 半透明合成
		kBlendType_Add = 1, // 加算合成
	};
	// StopAction定数
	enum {
		kStopAction_None = 0, // 何もしない
		kStopAction_Destroy = 1, // 自信のGameObjectを削除
	};
	// その他フラグ定数
	enum {
		kEtcFlag_None = 0,
		kEtcFlag_MoveDirToYAxis = 1,
	};
};