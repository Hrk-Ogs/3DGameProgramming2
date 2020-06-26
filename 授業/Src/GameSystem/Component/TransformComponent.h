#pragma once

//===================================
// トランスフォームコンポーネント
// ・GameObjectの姿勢を担当する
//===================================
class TransformComponent :public BaseComponent
{
public:

	// 更新処理
	virtual void Update() override;

	// ImGui処理
	virtual void Editor_ImGuiUpdate() override;

	//================================
	// シリアライズ/デシリアライズ
	//================================
	virtual void Deserialize(const json11::Json& jsonObj);
	virtual void Serialize(json11::Json::object& outJsonObj)const;

	//================================
	// ローカル
	//================================
	// ローカル行列取得
	const KdMatrix& GetLocalMatrix() const { return m_localMatrix; }
	// ローカル行列セット
	void SetLocalMatrix(const KdMatrix& mat) 
	{
		m_worldMatrix = mat;
		WorldToLocal();		// セットされたワールド行列からローカル行列を計算する
	}

	// ローカル拡縮セット
	void SetLocalScale(const KdVec3& v) 
	{
		m_localMatrix.SetScale(v);
		LocalToWorld();			// ローカル行列からワールド行列を計算
	}
	// ローカル拡縮取得
	KdVec3 GetLocalScale()
	{
		return m_localMatrix.GetScale();
	}

	// ローカル座標セット
	void SetLocalPosition(const KdVec3& v) 
	{
		m_localMatrix.Translation(v);
		LocalToWorld();
	}
	// ローカル座標取得
	KdVec3 GetLocalPosition()
	{
		return m_localMatrix.Translation();
	}

	// ローカル回転セット
	void SetLocalRotation(const KdQuaternion& rotation)
	{
		KdVec3 scale = m_localMatrix.GetScale();
		KdVec3 pos = m_localMatrix.Translation();
		// 拡大・回転・座標から行列作成
		m_localMatrix.CreateFromSRT(scale, rotation, pos);

		LocalToWorld();
	}
	// ローカル回転取得
	KdQuaternion GetlocalRotation()
	{
		return KdQuaternion().CreateFromMatrix(m_localMatrix);
	}

	// ローカル回転をXYZ軸の角度（ラジアン）で取得
	KdVec3 GetLocalRotationAngles()
	{
		return m_localMatrix.ComputeAngles();
	}
	// ローカル回転をXYZ軸の角度（ラジアン）でセット
	void setLocalRotationAngles(const KdVec3& angles)
	{
		KdVec3 scale = m_localMatrix.GetScale();
		KdQuaternion rotation = KdQuaternion().CreateRotationFromAngles(angles.x, angles.y, angles.z);
		KdVec3 pos = m_localMatrix.Translation();
		m_localMatrix.CreateFromSRT(scale, rotation, pos);

		LocalToWorld();
	}


	//================================
	// ワールド
	//================================
	// ワールド行列取得
	const KdMatrix& GetMatrix() const { return m_worldMatrix; }
	// ワールド行列セット
	void SetMatrix(const KdMatrix& mat) 
	{
		m_worldMatrix = mat;
		LocalToWorld();			// セットされたローカル行列からワールド行列を計算する
	}

	// 座標セット
	void SetPosition(const KdVec3& v)
	{
		m_worldMatrix.Translation(v);

		WorldToLocal();
	}
	// 座標取得
	KdVec3 GetPosition()
	{
		return m_worldMatrix.Translation();
	}

	// 回転セット
	void SetRotation(const KdQuaternion& rotation)
	{
		KdVec3 scale = m_worldMatrix.GetScale();
		KdVec3 pos = m_worldMatrix.Translation();
		// 拡大・回転・座標から行列作成
		m_worldMatrix.CreateFromSRT(scale, rotation, pos);

		WorldToLocal();
	}
	// 回転取得
	KdQuaternion GetRotation()
	{
		return KdQuaternion().CreateFromMatrix(m_worldMatrix);
	}

private:
	
	// ローカル行列
	KdMatrix		m_localMatrix;

	// ワールド行列
	KdMatrix		m_worldMatrix;

	// ローカル行列からワールド行列を計算する
	void LocalToWorld();
	// ワールド行列からローカル行列を計算する
	void WorldToLocal();
};
