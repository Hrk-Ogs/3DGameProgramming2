#include "System/KdSystem.h"

#include "KdMesh.h"

#include "KdGLTFLoader.h"

//=============================================================
//
// Mesh
//
//=============================================================

void KdMesh::SetToDevice() const
{
	std::vector<ID3D11Buffer*>	buffers;
	buffers.push_back(m_vb_Pos.GetBuffer());
	buffers.push_back(m_vb_UV.GetBuffer());
	buffers.push_back(m_vb_Tangent.GetBuffer());
	buffers.push_back(m_vb_Normal.GetBuffer());
	buffers.push_back(m_vb_Color.GetBuffer());
	if (m_vb_SkinIndex.GetBuffer()) {
		buffers.push_back(m_vb_SkinIndex.GetBuffer());
	}
	if (m_vb_SkinWeight.GetBuffer()) {
		buffers.push_back(m_vb_SkinWeight.GetBuffer());
	}
	D3D.GetDevContext()->IASetVertexBuffers(0, buffers.size(), &buffers[0], &m_stride[0], &m_offset[0]);

	// インデックスバッファセット
	D3D.GetDevContext()->IASetIndexBuffer(m_ib.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//プリミティブ・トポロジーをセット
	D3D.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool KdMesh::Create(
	const KdMeshVertexData& vertexData,
	const std::vector<KdMeshFace>& faces,
	const std::vector<KdMeshSubset>& subsets)
{
	Release();

	//------------------------------
	// サブセット情報
	//------------------------------
	m_subsets = subsets;

	//------------------------------
	// 頂点バッファ(座標)作成
	//------------------------------
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertexData.PosList[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_vb_Pos.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(KdVec3) * vertexData.PosList.size(), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}

		// 
		m_stride.push_back(sizeof(KdVec3));
		m_offset.push_back(0);

		// 座標のみの配列
		m_positions = vertexData.PosList;

		// AA境界データ作成
		DirectX::BoundingBox::CreateFromPoints(m_aabb, m_positions.size(), &m_positions[0], sizeof(KdVec3));
		// 境界球データ作成
		DirectX::BoundingSphere::CreateFromPoints(m_bs, m_positions.size(), &m_positions[0], sizeof(KdVec3));
	}	
	//------------------------------
	// 頂点バッファ(UV)作成
	//------------------------------
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertexData.UVList[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_vb_UV.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(KdVec2) * vertexData.UVList.size(), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}

		// 
		m_stride.push_back(sizeof(KdVec2));
		m_offset.push_back(0);
	}
	//------------------------------
	// 頂点バッファ(Tangent)作成
	//------------------------------
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertexData.TangentList[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_vb_Tangent.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(KdVec3) * vertexData.TangentList.size(), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}

		// 
		m_stride.push_back(sizeof(KdVec3));
		m_offset.push_back(0);
	}
	//------------------------------
	// 頂点バッファ(Normal)作成
	//------------------------------
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertexData.NormalList[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_vb_Normal.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(KdVec3) * vertexData.NormalList.size(), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}

		// 
		m_stride.push_back(sizeof(KdVec3));
		m_offset.push_back(0);
	}
	//------------------------------
	// 頂点バッファ(Color)作成
	//------------------------------
	{
		std::vector<UINT> tmpBuffer;

		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertexData.ColorList[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_vb_Color.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(vertexData.ColorList[0]) * vertexData.ColorList.size(), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}

		// 
		m_stride.push_back(sizeof(vertexData.ColorList[0]));
		m_offset.push_back(0);
	}
	//------------------------------
	// 頂点バッファ(SkinIndexList)作成
	//------------------------------
	if (vertexData.SkinIndexList.size() > 0)
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertexData.SkinIndexList[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_vb_SkinIndex.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(vertexData.SkinIndexList[0]) * vertexData.SkinIndexList.size(), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}
		// 
		m_stride.push_back(sizeof(vertexData.SkinIndexList[0]));
		m_offset.push_back(0);
	}
	//------------------------------
	// 頂点バッファ(SkinWeightList)作成
	//------------------------------
	if (vertexData.SkinWeightList.size() > 0)
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &vertexData.SkinWeightList[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_vb_SkinWeight.Create(D3D11_BIND_VERTEX_BUFFER, sizeof(vertexData.SkinWeightList[0]) * vertexData.SkinWeightList.size(), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}
		// 
		m_stride.push_back(sizeof(vertexData.SkinWeightList[0]));
		m_offset.push_back(0);
	}


	//------------------------------
	// インデックスバッファ作成
	//------------------------------
	{
		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &faces[0];				// バッファに書き込む頂点配列の先頭アドレス
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		// バッファ作成
		if (FAILED(m_ib.Create(D3D11_BIND_INDEX_BUFFER, faces.size() * sizeof(KdMeshFace), D3D11_USAGE_DEFAULT, 0, &initData)))
		{
			Release();
			return false;
		}

		// 面情報コピー
		m_faces = faces;
	}

	return true;
}


void KdMesh::DrawSubset(int subsetNo) const
{
	// 範囲外のサブセットはスキップ
	if (subsetNo >= (int)m_subsets.size())return;
	// 面数が0なら描画スキップ
	if (m_subsets[subsetNo].FaceCount == 0)return;

	// 描画
	D3D.GetDevContext()->DrawIndexed(m_subsets[subsetNo].FaceCount * 3, m_subsets[subsetNo].FaceStart * 3, 0);
}

