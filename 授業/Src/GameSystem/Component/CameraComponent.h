#pragma once

//===========================================
//
// カメラコンポーネント
//
//===========================================
class CameraComponent :public BaseComponent
{
public:

	// 視野角取得
	float GetProjFOV()const { return m_proj_FOV; }
	// 最近接距離と最遠方距離取得
	const KdVec2& GetProjNearFar()const { return m_proj_NearAndFar; }

	// 射影行列取得
	void GetProjectionMatrix(KdMatrix& out);

	// カメラ情報をシェーダーへセットする
	void SetCamera();

	//============================

	// 描画準備処理
	virtual void PrepareDraw(RenderingData& rdata)override;

	// (Editor用)ImGui
	virtual void Editor_ImGuiUpdate()override;

	//============================
	// Serialize/Deserialize
	//============================
	//JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["FOV"], m_proj_FOV);
		KdJsonGet(jsonData["NearAndFar"], m_proj_NearAndFar);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["FOV"] = m_proj_FOV;
		outJsonObj["NearAndFar"] = m_proj_NearAndFar.ToArray();
	}

private:

	// 射影行列用データ
	float	m_proj_FOV = 60;		// 視野角
	KdVec2	m_proj_NearAndFar = { 0.01f,5000 };	// 最近接距離と最遠方距離
};
