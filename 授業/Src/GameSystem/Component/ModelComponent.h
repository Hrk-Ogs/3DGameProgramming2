#pragma once

//===================================
// モデルコンポーネント
// ・3Dモデルの処理や描画を行う部品
//===================================
class ModelComponent :public BaseComponent
{
public:

	//===============================
	// 動作処理
	//===============================
	// 更新処理
	virtual void Update() override;
	// 描画処理
	virtual void Draw() override;

	//===============================
	// ImGui処理
	//===============================
	virtual void Editor_ImGuiUpdate() override;

private:

	// モデルデータ 3Dメッシュモデルの元データ
	// ※バグ軽減のため、必ずnullptrにならないようにする
	KdSptr<KdModel>			m_model = std::make_shared<KdModel>();

	// ノードアウトライナー　ノード（ボーン）の扱い専門
	KdNodeOutliner			m_nodeOL;

	// アニメーター
	KdAnimator				m_anim;


};
