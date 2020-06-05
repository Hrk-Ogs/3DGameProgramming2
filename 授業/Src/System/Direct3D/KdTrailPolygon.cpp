#include "System/KdSystem.h"

#include "KdTrailPolygon.h"

void KdTrailPolygon::DrawBillboard(float width)
{
	// ポイントが２つ以下の場合は描画不可
	if (m_pointList.size() < 2)return;

	// カメラの情報
	KdMatrix mCam = SHADER.m_cb7_Camera.GetWork().mV;
	mCam.Inverse();

	// スライス数
	float sliceCount = (float)(m_pointList.size() - 1);

	// 頂点配列
	std::vector<Vertex> vertex;
	vertex.resize(m_pointList.size() * 2);

	//--------------------------
	// 頂点データ作成
	//--------------------------
	KdVec3 prevPos;
	for (UINT mi = 0; mi < m_pointList.size(); mi++)
	{
		// 登録行列の参照(ショートカット)
		KdVec3& pos = m_pointList[mi];

		// ２つの頂点の参照(ショートカット)
		Vertex& v1 = vertex[mi * 2];
		Vertex& v2 = vertex[mi * 2 + 1];

		// ラインの向き
		KdVec3 vDir;
		if (mi == 0)
		{	// 初回時のみ、次のポイントを使用
			vDir = m_pointList[1] - pos;
		}
		else
		{	// 二回目以降は、前回の座標から向きを決定する
			vDir = pos - prevPos;
		}
		// カメラからポイントへの向き
		KdVec3 v = pos - mCam.Translation();
		KdVec3 axisX = DirectX::XMVector3Cross(vDir, v);

		axisX.Normalize();

		// 座標
		v1.Pos = pos + axisX * width * 0.5f;
		v2.Pos = pos - axisX * width * 0.5f;

		// UV
		v1.UV = { 0, mi / sliceCount };
		v2.UV = { 1, mi / sliceCount };

		// 座標を記憶しておく
		prevPos = pos;
	}

	// テクスチャセット
	if (m_texture)
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, m_texture->GetSRViewAddress());
	}
	else
	{
		D3D.GetDevContext()->PSSetShaderResources(0, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));

}

void KdTrailPolygon::DrawStrip(int setTextureSlotNo)
{
	if (m_pointList.size() < 3)return;

	// 頂点配列
	std::vector<Vertex> vertex;
	vertex.resize(m_pointList.size());

	int num = m_pointList.size() / 2;

	// 頂点データ作成
	for (UINT mi = 0; mi < m_pointList.size(); mi++)
	{
		KdVec3& pos = m_pointList[mi];
		Vertex& v = vertex[mi];
		// 頂点座標
		v.Pos = pos;
		// UV
		v.UV.x = (float)(mi % 2);
		v.UV.y = (mi / 2) / (float)num;
	}

	// テクスチャセット
	if (m_texture) {
		D3D.GetDevContext()->PSSetShaderResources(setTextureSlotNo, 1, m_texture->GetSRViewAddress());
	}
	else {
		D3D.GetDevContext()->PSSetShaderResources(setTextureSlotNo, 1, D3D.GetWhiteTex()->GetSRViewAddress());
	}

	// 指定した頂点配列を描画する関数
	D3D.DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, vertex.size(), &vertex[0], sizeof(Vertex));

}
