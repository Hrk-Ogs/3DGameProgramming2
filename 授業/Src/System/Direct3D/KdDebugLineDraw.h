#pragma once

//===========================================================
//
// �f�o�b�O���`��V�X�e��
//
//===========================================================
class KdDebugLineDraw
{
public:
	
	// ����ǉ�
	void AddLine(const KdVec3& p1, const KdVec3& p2, const Math::Color& color = { 1,1,1,1 });

	// �\����
	void AddCrossLine(const KdVec3& p1, float halfSize, const Math::Color& color = { 1,1,1,1 });

	// ����ǉ�
	void AddSphereLine(const KdVec3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

	// ����ǉ�
	void AddBoxLine(const KdVec3& centerPos, const KdVec3& halfSize, const KdQuaternion& quat, const Math::Color& color = { 1,1,1,1 });

	// ���b�V����ǉ�
	void AddMeshLine(const KdSptr<KdMesh>& mesh, const KdMatrix& mat, const Math::Color& color = { 1,1,1,1 });

	// �S�ă��Z�b�g
	void ResetLines()
	{
		m_debugLineVertices.clear();
		m_debugLineMeshes.clear();
	}

	// �`��
	void Draw();

private:

	// �f�o�b�O���̒��_�z��(�Q���_�łP���ŕ`�悳���)
	std::vector<KdEffectShader::Vertex>	m_debugLineVertices;

	// ���b�V���͐��������̂ŁAKdMesh�����̂܂ܓo�^�ł���悤�ɂ���
	struct MeshData
	{
		KdMatrix		Mat;
		KdSptr<KdMesh>	Mesh;
		KdVec4			Color;
	};
	std::vector<MeshData>	m_debugLineMeshes;
};
