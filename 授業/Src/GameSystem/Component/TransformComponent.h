#pragma once

//===================================
// �g�����X�t�H�[���R���|�[�l���g
// �EGameObject�̎p����S������
//===================================
class TransformComponent :public BaseComponent
{
public:

	// �X�V����
	virtual void Update() override;

	// ImGui����
	virtual void Editor_ImGuiUpdate() override;

	//================================
	// �V���A���C�Y/�f�V���A���C�Y
	//================================
	virtual void Deserialize(const json11::Json& jsonObj);
	virtual void Serialize(json11::Json::object& outJsonObj)const;

	//================================
	// ���[�J��
	//================================
	// ���[�J���s��擾
	const KdMatrix& GetLocalMatrix() const { return m_localMatrix; }
	// ���[�J���s��Z�b�g
	void SetLocalMatrix(const KdMatrix& mat) 
	{
		m_worldMatrix = mat;
		WorldToLocal();		// �Z�b�g���ꂽ���[���h�s�񂩂烍�[�J���s����v�Z����
	}

	// ���[�J���g�k�Z�b�g
	void SetLocalScale(const KdVec3& v) 
	{
		m_localMatrix.SetScale(v);
		LocalToWorld();			// ���[�J���s�񂩂烏�[���h�s����v�Z
	}
	// ���[�J���g�k�擾
	KdVec3 GetLocalScale()
	{
		return m_localMatrix.GetScale();
	}

	// ���[�J�����W�Z�b�g
	void SetLocalPosition(const KdVec3& v) 
	{
		m_localMatrix.Translation(v);
		LocalToWorld();
	}
	// ���[�J�����W�擾
	KdVec3 GetLocalPosition()
	{
		return m_localMatrix.Translation();
	}

	// ���[�J����]�Z�b�g
	void SetLocalRotation(const KdQuaternion& rotation)
	{
		KdVec3 scale = m_localMatrix.GetScale();
		KdVec3 pos = m_localMatrix.Translation();
		// �g��E��]�E���W����s��쐬
		m_localMatrix.CreateFromSRT(scale, rotation, pos);

		LocalToWorld();
	}
	// ���[�J����]�擾
	KdQuaternion GetlocalRotation()
	{
		return KdQuaternion().CreateFromMatrix(m_localMatrix);
	}

	// ���[�J����]��XYZ���̊p�x�i���W�A���j�Ŏ擾
	KdVec3 GetLocalRotationAngles()
	{
		return m_localMatrix.ComputeAngles();
	}
	// ���[�J����]��XYZ���̊p�x�i���W�A���j�ŃZ�b�g
	void setLocalRotationAngles(const KdVec3& angles)
	{
		KdVec3 scale = m_localMatrix.GetScale();
		KdQuaternion rotation = KdQuaternion().CreateRotationFromAngles(angles.x, angles.y, angles.z);
		KdVec3 pos = m_localMatrix.Translation();
		m_localMatrix.CreateFromSRT(scale, rotation, pos);

		LocalToWorld();
	}


	//================================
	// ���[���h
	//================================
	// ���[���h�s��擾
	const KdMatrix& GetMatrix() const { return m_worldMatrix; }
	// ���[���h�s��Z�b�g
	void SetMatrix(const KdMatrix& mat) 
	{
		m_worldMatrix = mat;
		LocalToWorld();			// �Z�b�g���ꂽ���[�J���s�񂩂烏�[���h�s����v�Z����
	}

	// ���W�Z�b�g
	void SetPosition(const KdVec3& v)
	{
		m_worldMatrix.Translation(v);

		WorldToLocal();
	}
	// ���W�擾
	KdVec3 GetPosition()
	{
		return m_worldMatrix.Translation();
	}

	// ��]�Z�b�g
	void SetRotation(const KdQuaternion& rotation)
	{
		KdVec3 scale = m_worldMatrix.GetScale();
		KdVec3 pos = m_worldMatrix.Translation();
		// �g��E��]�E���W����s��쐬
		m_worldMatrix.CreateFromSRT(scale, rotation, pos);

		WorldToLocal();
	}
	// ��]�擾
	KdQuaternion GetRotation()
	{
		return KdQuaternion().CreateFromMatrix(m_worldMatrix);
	}

private:
	
	// ���[�J���s��
	KdMatrix		m_localMatrix;

	// ���[���h�s��
	KdMatrix		m_worldMatrix;

	// ���[�J���s�񂩂烏�[���h�s����v�Z����
	void LocalToWorld();
	// ���[���h�s�񂩂烍�[�J���s����v�Z����
	void WorldToLocal();
};
