#include "System/KdSystem.h"

#include "KdDebugLineDraw.h"

// 線を追加

void KdDebugLineDraw::AddLine(const KdVec3& p1, const KdVec3& p2, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0,0 };

	// 線
	ver.Pos = p1;
	m_debugLineVertices.push_back(ver);
	ver.Pos = p2;
	m_debugLineVertices.push_back(ver);
}

void KdDebugLineDraw::AddCrossLine(const KdVec3& p1, float halfSize, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0,0 };

	// X線
	ver.Pos = p1;
	ver.Pos.x -= halfSize;
	m_debugLineVertices.push_back(ver);
	ver.Pos = p1;
	ver.Pos.x += halfSize;
	m_debugLineVertices.push_back(ver);

	// Y線
	ver.Pos = p1;
	ver.Pos.y -= halfSize;
	m_debugLineVertices.push_back(ver);
	ver.Pos = p1;
	ver.Pos.y += halfSize;
	m_debugLineVertices.push_back(ver);

	// Z線
	ver.Pos = p1;
	ver.Pos.z -= halfSize;
	m_debugLineVertices.push_back(ver);
	ver.Pos = p1;
	ver.Pos.z += halfSize;
	m_debugLineVertices.push_back(ver);
}

void KdDebugLineDraw::AddSphereLine(const KdVec3& pos, float radius, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0,0 };

	static constexpr int kDetail = 20;	// 円の細かさ

	for (int i = 0; i < kDetail + 1; i++)
	{
		// XZ平面の円
		ver.Pos = pos;
		ver.Pos.x += cos(((float)i * (360 / kDetail)) * KdToRadians) * radius;
		ver.Pos.z += sin(((float)i * (360 / kDetail)) * KdToRadians) * radius;
		m_debugLineVertices.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos(((float)(i + 1) * (360 / kDetail)) * KdToRadians) * radius;
		ver.Pos.z += sin(((float)(i + 1) * (360 / kDetail)) * KdToRadians) * radius;
		m_debugLineVertices.push_back(ver);

		// XY平面の円
		ver.Pos = pos;
		ver.Pos.x += cos(((float)i * (360 / kDetail)) * KdToRadians) * radius;
		ver.Pos.y += sin(((float)i * (360 / kDetail)) * KdToRadians) * radius;
		m_debugLineVertices.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos(((float)(i + 1) * (360 / kDetail)) * KdToRadians) * radius;
		ver.Pos.y += sin(((float)(i + 1) * (360 / kDetail)) * KdToRadians) * radius;
		m_debugLineVertices.push_back(ver);

		// YZ平面の円
		ver.Pos = pos;
		ver.Pos.y += cos(((float)i * (360 / kDetail)) * KdToRadians) * radius;
		ver.Pos.z += sin(((float)i * (360 / kDetail)) * KdToRadians) * radius;
		m_debugLineVertices.push_back(ver);

		ver.Pos = pos;
		ver.Pos.y += cos(((float)(i + 1) * (360 / kDetail)) * KdToRadians) * radius;
		ver.Pos.z += sin(((float)(i + 1) * (360 / kDetail)) * KdToRadians) * radius;
		m_debugLineVertices.push_back(ver);
	}
}

void KdDebugLineDraw::AddBoxLine(const KdVec3& centerPos, const KdVec3& halfSize, const KdQuaternion& quat, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0,0 };

//	KdVec3 vMin = centerPos - halfSize;
//	KdVec3 vMax = centerPos + halfSize;

	// 下面
	ver.Pos = { -halfSize.x, -halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { +halfSize.x, -halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { +halfSize.x, -halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { +halfSize.x, -halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { +halfSize.x, -halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { -halfSize.x, -halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { -halfSize.x, -halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { -halfSize.x, -halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	// 上面
	ver.Pos = { -halfSize.x, +halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { +halfSize.x, +halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { +halfSize.x, +halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { +halfSize.x, +halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { +halfSize.x, +halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { -halfSize.x, +halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { -halfSize.x, +halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { -halfSize.x, +halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	// 縦線
	ver.Pos = { -halfSize.x, -halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { -halfSize.x, +halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { +halfSize.x, -halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { +halfSize.x, +halfSize.y, -halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { +halfSize.x, -halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { +halfSize.x, +halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

	ver.Pos = { -halfSize.x, -halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);
	ver.Pos = { -halfSize.x, +halfSize.y, +halfSize.z };
	ver.Pos.Transform(quat);
	ver.Pos += centerPos;
	m_debugLineVertices.push_back(ver);

}

// メッシュを追加

void KdDebugLineDraw::AddMeshLine(const KdSptr<KdMesh>& mesh, const KdMatrix& mat, const Math::Color& color)
{
	MeshData data;
	data.Mesh = mesh;
	data.Mat = mat;
	data.Color = color;
	m_debugLineMeshes.push_back(data);
}

// 描画

void KdDebugLineDraw::Draw()
{
	// シェーダーをセット
	SHADER.m_effectShader.SetToDevice();

	// 線
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());
	{
		// Z書き込みOFF
		//		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);
		SHADER.m_effectShader.SetWorldMatrix(KdMatrix());
		if (m_debugLineVertices.size() >= 1)
		{
			SHADER.m_effectShader.DrawVertices(m_debugLineVertices, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			m_debugLineVertices.clear();
		}
	}

	// ワイヤーフレームモードへ
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_Wireframe);

	std::vector<KdSptr<KdMaterial>> mates;
	mates.push_back(std::make_shared<KdMaterial>());
	mates[0]->BaseColor = { 1,0,0,1 };

	for (auto&& meshData : m_debugLineMeshes)
	{
		SHADER.m_effectShader.SetColor(meshData.Color);
		SHADER.m_effectShader.SetWorldMatrix(meshData.Mat);
		SHADER.m_effectShader.DrawMesh(meshData.Mesh.get(), mates);
	}

	SHADER.m_effectShader.SetColor({ 1,1,1,1 });

	// 通常モードへ
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);
}
