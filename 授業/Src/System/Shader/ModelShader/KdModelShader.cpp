#include "System/KdSystem.h"

#include "KdModelShader.h"

bool KdModelShader::Init()
{
	//--------------------------------
	// ’¸“_ƒVƒF[ƒ_[
	//--------------------------------
	{
#include "KdModelShader_VS.inc"

		// ’¸“_ƒVƒF[ƒ_[ì¬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "’¸“_ƒVƒF[ƒ_[ì¬¸”s");
			Release();
			return false;
		}

		// 1’¸“_‚ÌÚ×‚Èî•ñ
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ’¸“_ƒŒƒCƒAƒEƒgì¬
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0], // “ü—ÍƒGƒŒƒƒ“ƒgæ“ªƒAƒhƒŒƒX
			layout.size(), // “ü—ÍƒGƒŒƒƒ“ƒg”
			&compiledBuffer[0], // ’¸“_ƒoƒbƒtƒ@‚ÌƒoƒCƒiƒŠƒf[ƒ^
			sizeof(compiledBuffer), // ã‹L‚Ìƒoƒbƒtƒ@ƒTƒCƒY
			&m_inputLayout)) //
			) {
			assert(0 && "CreateInputLayout¸”s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// (ƒXƒLƒ“ƒƒbƒVƒ…)’¸“_ƒVƒF[ƒ_
	//-------------------------------------
	{
		// ƒRƒ“ƒpƒCƒ‹Ï‚İ‚ÌƒVƒF[ƒ_[ƒwƒbƒ_[ƒtƒ@ƒCƒ‹‚ğƒCƒ“ƒNƒ‹[ƒh
		#include "KdModelShader_VS_Skin.inc"

		// ’¸“_ƒVƒF[ƒ_[ì¬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS_Skin)))
		{
			assert(0 && "’¸“_ƒVƒF[ƒ_[ì¬¸”s");
			Release();
			return false;
		}

		// ‚P’¸“_‚ÌÚ×‚Èî•ñ
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SKININDEX", 0, DXGI_FORMAT_R16G16B16A16_UINT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ’¸“_“ü—ÍƒŒƒCƒAƒEƒgì¬
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0], // “ü—ÍƒGƒŒƒƒ“ƒgæ“ªƒAƒhƒŒƒX
			layout.size(), // “ü—ÍƒGƒŒƒƒ“ƒg”
			&compiledBuffer[0], // ’¸“_ƒoƒbƒtƒ@‚ÌƒoƒCƒiƒŠƒf[ƒ^
			sizeof(compiledBuffer), // ã‹L‚Ìƒoƒbƒtƒ@ƒTƒCƒY
			&m_inputLayout_Skin)) //
			) {
			assert(0 && "CreateInputLayout¸”s");
			Release();
			return false;
		}
	}

	//--------------------------------
	// ƒsƒNƒZƒ‹ƒVƒF[ƒ_[
	//--------------------------------
	{
		// ƒRƒ“ƒpƒCƒ‹Ï‚İ‚ÌƒVƒF[ƒ_[ƒwƒbƒ_[ƒtƒ@ƒCƒ‹‚ğƒCƒ“ƒNƒ‹[ƒh
#include "KdModelShader_PS.inc"
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ƒsƒNƒZƒ‹ƒVƒF[ƒ_[ì¬¸”s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// —ÖŠs—p ’¸“_ƒVƒF[ƒ_
	//-------------------------------------
	{
		// ƒRƒ“ƒpƒCƒ‹Ï‚İ‚ÌƒVƒF[ƒ_[ƒwƒbƒ_[ƒtƒ@ƒCƒ‹‚ğƒCƒ“ƒNƒ‹[ƒh
		#include "KdModelShader_OutlineVS.inc"

		// ’¸“_ƒVƒF[ƒ_[ì¬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_outlineVS)))
		{
			assert(0 && "’¸“_ƒVƒF[ƒ_[ì¬¸”s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// —ÖŠs—p ƒsƒNƒZƒ‹ƒVƒF[ƒ_
	//-------------------------------------
	{
		// ƒRƒ“ƒpƒCƒ‹Ï‚İ‚ÌƒVƒF[ƒ_[ƒwƒbƒ_[ƒtƒ@ƒCƒ‹‚ğƒCƒ“ƒNƒ‹[ƒh
#include "KdModelShader_OutlinePS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_outlinePS))) {
			assert(0 && "ƒsƒNƒZƒ‹ƒVƒF[ƒ_[ì¬¸”s");
			Release();
			return false;
		}
	}

	//--------------------------------
	// ’è”ƒoƒbƒtƒ@ì¬
	//--------------------------------
	m_cb0.Create();

	//-------------------------------------
	// ƒeƒNƒXƒ`ƒƒ“Ç‚İ‚İ
	//-------------------------------------
	m_texToon = KDResFactory.GetTexture("Data\\Texture\\toon.png");
	D3D.GetDevContext()->PSSetShaderResources(4, 1, m_texToon->GetSRViewAddress());

	return true;
}

void KdModelShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_PS);

	KdSafeRelease(m_VS_Skin);
	KdSafeRelease(m_inputLayout_Skin);

	KdSafeRelease(m_outlineVS);
	KdSafeRelease(m_outlinePS);

	m_cb0.Release();
}

void KdModelShader::SetToDevice_Outline()
{
	// ’¸“_ƒVƒF[ƒ_‚ğƒZƒbƒg
	D3D.GetDevContext()->VSSetShader(m_outlineVS, 0, 0);
	// ’¸“_ƒŒƒCƒAƒEƒg‚ğƒZƒbƒg
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// ƒsƒNƒZƒ‹ƒVƒF[ƒ_‚ğƒZƒbƒg
	D3D.GetDevContext()->PSSetShader(m_outlinePS, 0, 0);
	//---------------------
	// ’è”ƒoƒbƒtƒ@‚ğƒZƒbƒg
	//---------------------
	// ƒIƒuƒWƒFƒNƒg’è”ƒoƒbƒtƒ@
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
}

void KdModelShader::DrawMesh_Outline(const KdMesh* mesh)
{
	if (mesh == nullptr)return;
	
	// ’¸“_ƒVƒF[ƒ_ƒZƒbƒg
	if (mesh->IsSkinMesh())
	{
		// ƒXƒLƒ“ƒƒbƒVƒ…—p‚Ì’¸“_ƒVƒF[ƒ_[‚ğƒZƒbƒg
		//D3D.GetDevContext()->VSSetShader(m_VS_Skin, 0, 0);
		// ’¸“_ƒŒƒCƒAƒEƒg‚ğƒZƒbƒg
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout_Skin);
	}
	else
	{
		// ƒXƒ^ƒeƒBƒbƒNƒƒbƒVƒ…—p‚Ì’¸“_ƒVƒF[ƒ_‚ğƒZƒbƒg
		D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
		// ’¸“_ƒŒƒCƒAƒEƒg‚ğƒZƒbƒg
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	}
	

	// ƒƒbƒVƒ…î•ñ‚ğƒZƒbƒg
	mesh->SetToDevice();

	// ‘SƒTƒuƒZƒbƒg
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// –Ê‚ª‚P–‡‚à–³‚¢ê‡‚ÍƒXƒLƒbƒv
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;
		// ƒTƒuƒZƒbƒg•`‰æ
		mesh->DrawSubset(subi);
	}
}

void KdModelShader::Draw_Outline(const KdNodeOutliner* nodeOL, const KdMatrix& worldTransform)
{
	if (nodeOL == nullptr)return;

	//--------------------------
	// ƒ{[ƒ“ƒXƒLƒjƒ“ƒOs—ñ—p‚Ì\‘¢‰»ƒoƒbƒtƒ@‚ğƒZƒbƒg‚·‚é
	//--------------------------
	if (nodeOL->GetBoneStructuredBuffer().GetWork().size() >= 1)
	{
		D3D.GetDevContext()->VSSetShaderResources(100, 1, nodeOL->GetBoneStructuredBuffer().GetSRViewAddress());
	}

	//--------------------------
	// ‘Sƒm[ƒh(ƒƒbƒVƒ…)‚ğ•`‰æ
	//--------------------------
	// ƒƒbƒVƒ…‚ª“ü‚Á‚Ä‚¢‚éƒm[ƒh‚ÌIndex‘S‚Ä
	for (auto&& meshIdx : nodeOL->GetMeshNodeIndices())
	{
		// ÀÛ‚ÌƒƒbƒVƒ…‚Ì“ü‚Á‚Ä‚¢‚éƒm[ƒh
		auto& node = nodeOL->GetAllNodes()[meshIdx];
		
		// ’è”ƒoƒbƒtƒ@‚Éƒf[ƒ^‚ğ‘‚«‚Ş(‚Ü‚¾GPU‘¤‚Ö‚Í‘‚«‚Ü‚ê‚Ä‚¢‚Ü‚¹‚ñ)
		m_cb0.Work().mW = node.Transform * worldTransform;
		// ‚±‚ê‚ÅÀÛ‚ÉGPU‘¤‚Ìƒoƒbƒtƒ@‚Ö‘‚«‚Ü‚êƒVƒF[ƒ_[‚Åg—p‚Å‚«‚é‚æ‚¤‚É‚È‚é
		m_cb0.Write();
		
		// ƒƒbƒVƒ…•`‰æ
		DrawMesh_Outline(node.Mesh.get());
	}
}

void KdModelShader::SetToDevice()
{
	// ’¸“_ƒVƒF[ƒ_‚ğƒZƒbƒg
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ’¸“_ƒŒƒCƒAƒEƒg‚ğƒZƒbƒg
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// ƒsƒNƒZƒ‹ƒVƒF[ƒ_‚ğƒZƒbƒg
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	//--------------------------
	// ’è”ƒoƒbƒtƒ@‚ğƒZƒbƒg
	//--------------------------
	// ƒIƒuƒWƒFƒNƒg’è”ƒoƒbƒtƒ@i0”Ô–Ú‚ÉƒZƒbƒgj
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
}

void KdModelShader::DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials)
{
	if (mesh == nullptr)return;

	// ’¸“_ƒVƒF[ƒ_ƒZƒbƒg
	if (mesh->IsSkinMesh())
	{
		// ƒXƒLƒ“ƒƒbƒVƒ…—p‚Ì’¸“_ƒVƒF[ƒ_[‚ğƒZƒbƒg
		D3D.GetDevContext()->VSSetShader(m_VS_Skin, 0, 0);
		// ’¸“_ƒŒƒCƒAƒEƒg‚ğƒZƒbƒg
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout_Skin);
	}
	else
	{
		// ƒXƒ^ƒeƒBƒbƒNƒƒbƒVƒ…—p‚Ì’¸“_ƒVƒF[ƒ_‚ğƒZƒbƒg
		D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
		// ’¸“_ƒŒƒCƒAƒEƒg‚ğƒZƒbƒg
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	}

	// ƒƒbƒVƒ…î•ñ‚ğƒZƒbƒg
	mesh->SetToDevice();

	// ‘SƒTƒuƒZƒbƒg
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// –Ê‚ª‚P–‡‚à–³‚¢ê‡‚ÍƒXƒLƒbƒv
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

<<<<<<< HEAD
		// ƒ}ƒeƒŠƒAƒ‹
		const KdMaterial& material = *materials[mesh->GetSubsets()[subi].MaterialNo];

		//---------------------------
		// ƒ}ƒeƒŠƒAƒ‹î•ñ‚ğ’è”ƒoƒbƒtƒ@‚Ö‘‚«‚Ş
		//---------------------------
		// Šî–{F
		m_cb1_Material.Work().BaseColor = material.BaseColor;
		// ”­ŒõF
		m_cb1_Material.Work().Emissive = material.Emissive;
		// ‹à‘®«
		m_cb1_Material.Work().Matallic = material.Metallic;
		// ‘e‚³
		m_cb1_Material.Work().Roughness = material.Roughness;
		// GPU‚Ö“]‘—
		m_cb1_Material.Write();

		//------------------------------
		// ƒeƒNƒXƒ`ƒƒƒZƒbƒg
		//------------------------------
		// •¡”–ˆ‚ÌƒeƒNƒXƒ`ƒƒ‚ğˆê‹C‚É“n‚·‚æ‚¤‚ÉA”z—ñ‚ğ—pˆÓ
		std::array<ID3D11ShaderResourceView*, 4> srvs = {};

		// BaseColorƒeƒNƒXƒ`ƒƒ
		srvs[0] = material.BaseColorTex->GetSRView();
		// EmissiveƒeƒNƒXƒ`ƒƒ
		srvs[1] = material.EmissiveTex->GetSRView();
		// Metallic Roughness
		srvs[2] = material.MetallicRoughnessTex->GetSRView();
		// NormalMap
		if (material.ForceUseNormalTex)
		{
			srvs[3] = material.ForceUseNormalTex->GetSRView();
		}
		else
		{
			srvs[3] = material.NormalTex->GetSRView();
		}
		// ƒZƒbƒg 0”Ô–Ú‚ÌƒXƒƒbƒg‚©‚ç4–‡•ªƒZƒbƒg‚·‚é
		D3D.GetDevContext()->PSSetShaderResources(0, srvs.size(), &srvs[0]);

=======
>>>>>>> parent of fcd683a... ã€3Dpro2ã€‘
		// ƒTƒuƒZƒbƒg•`‰æ
		mesh->DrawSubset(subi);
	}
}

void KdModelShader::Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform)
{
	if (nodeOL == nullptr)return;

	//--------------------------
	// ƒ{[ƒ“ƒXƒLƒjƒ“ƒOs—ñ—p‚Ì\‘¢‰»ƒoƒbƒtƒ@‚ğƒZƒbƒg‚·‚é
	//--------------------------
	if (nodeOL->GetBoneStructuredBuffer().GetWork().size() >= 1)
	{
		D3D.GetDevContext()->VSSetShaderResources(100, 1, nodeOL->GetBoneStructuredBuffer().GetSRViewAddress());
	}

	//--------------------------
	// ‘Sƒm[ƒh(ƒƒbƒVƒ…)‚ğ•`‰æ
	//--------------------------
	// ƒƒbƒVƒ…‚ª“ü‚Á‚Ä‚¢‚éƒm[ƒh‚ÌIndex‘S‚Ä
	for (auto&& meshIdx : nodeOL->GetMeshNodeIndices())
	{
		// ÀÛ‚ÌƒƒbƒVƒ…‚Ì“ü‚Á‚Ä‚¢‚éƒm[ƒh
		auto& node = nodeOL->GetAllNodes()[meshIdx];

		// ’è”ƒoƒbƒtƒ@‚Éƒf[ƒ^‚ğ‘‚«‚Ş
		m_cb0.Work.mW = node.Transform * worldTransform;
		// ƒRƒŒ‚ÅÀÛ‚ÉGPU‘¤‚Ìƒoƒbƒtƒ@‚Ö‘‚«‚Ü‚êAƒVƒF[ƒ_[‚Åg—po—ˆ‚é‚æ‚¤‚É‚È‚é
		m_cb0.Write();

		// ƒƒbƒVƒ…•`‰æ
		DrawMesh(node.Mesh.get(), materials);
	}
}
