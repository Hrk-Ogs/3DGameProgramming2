#pragma once

//===================================
// トランスフォームコンポーネント
// ・GameObjectの姿勢を担当する
//===================================
class TransformComponent :public BaseComponent
{
public:

	// ImGui処理
	virtual void Editor_ImGuiUpdate() override;

	//================================
	// シリアライズ/デシリアライズ
	//================================
	virtual void Deserialize(const json11::Json& jsonObj);
	virtual void Serialize(json11::Json::object& outJsonObj)const;

	//================================
	// 設定・取得
	//================================

	// ワールド行列取得
	const KdMatrix& GetMatrix()const { return m_worldMatrix; }


private:

	// ワールド行列
	KdMatrix		m_worldMatrix;

};
