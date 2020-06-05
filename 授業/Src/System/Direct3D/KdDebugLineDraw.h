#pragma once

//===========================================================
//
// デバッグ線描画システム
//
//===========================================================
class KdDebugLineDraw
{
public:
	
	// 線を追加
	void AddLine(const KdVec3& p1, const KdVec3& p2, const Math::Color& color = { 1,1,1,1 });

	// 十字線
	void AddCrossLine(const KdVec3& p1, float halfSize, const Math::Color& color = { 1,1,1,1 });

	// 球を追加
	void AddSphereLine(const KdVec3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

	// 箱を追加
	void AddBoxLine(const KdVec3& centerPos, const KdVec3& halfSize, const KdQuaternion& quat, const Math::Color& color = { 1,1,1,1 });

	// メッシュを追加
	void AddMeshLine(const KdSptr<KdMesh>& mesh, const KdMatrix& mat, const Math::Color& color = { 1,1,1,1 });

	// 全てリセット
	void ResetLines()
	{
		m_debugLineVertices.clear();
		m_debugLineMeshes.clear();
	}

	// 描画
	void Draw();

private:

	// デバッグ線の頂点配列(２頂点で１線で描画される)
	std::vector<KdEffectShader::Vertex>	m_debugLineVertices;

	// メッシュは線が多いので、KdMeshをそのまま登録できるようにする
	struct MeshData
	{
		KdMatrix		Mat;
		KdSptr<KdMesh>	Mesh;
		KdVec4			Color;
	};
	std::vector<MeshData>	m_debugLineMeshes;
};
