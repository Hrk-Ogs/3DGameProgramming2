#include "KdGenShadowMapShader.h"

bool KdGenShadowMapShader::Init()
{
	Release();

	//-------------------------------------
	// ���_�V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdGenShadowMap_VS.inc"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// �P���_�̏ڍׂȏ��
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_���̓��C�A�E�g�쐬
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0], // ���̓G�������g�擪�A�h���X
			layout.size(), // ���̓G�������g��
			&compiledBuffer[0], // ���_�o�b�t�@�̃o�C�i���f�[�^
			sizeof(compiledBuffer), // ��L�̃o�b�t�@�T�C�Y
			&m_inputLayout)) //
			) {
			assert(0 && "CreateInputLayout���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// ���_�V�F�[�_(�X�L�����b�V��)
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdGenShadowMap_VS_Skin.inc"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS_Skin))) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// �P���_�̏ڍׂȏ��
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SKININDEX", 0, DXGI_FORMAT_R16G16B16A16_UINT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_���̓��C�A�E�g�쐬
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0], // ���̓G�������g�擪�A�h���X
			layout.size(), // ���̓G�������g��
			&compiledBuffer[0], // ���_�o�b�t�@�̃o�C�i���f�[�^
			sizeof(compiledBuffer), // ��L�̃o�b�t�@�T�C�Y
			&m_inputLayout_Skin)) //
			) {
			assert(0 && "CreateInputLayout���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// �s�N�Z���V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdGenShadowMap_PS.inc"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// �萔�o�b�t�@�쐬
	//-------------------------------------
	m_cb0.Create();

	//-------------------------------------
	// �[�x�}�b�v�쐻
	//-------------------------------------
	m_spotLightShadowMap = std::make_shared<KdTexture>();
	m_spotLightShadowMap->CreateRenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, SHADER.m_cb8_Light.GetWork().PL.size());
	m_spotLightZBuffer = std::make_shared<KdTexture>();
	m_spotLightZBuffer->CreateDepthStencil(1024, 1024, DXGI_FORMAT_R32_TYPELESS, SHADER.m_cb8_Light.GetWork().PL.size());

	m_dirLightShadowMap = std::make_shared<KdTexture>();
	m_dirLightShadowMap->CreateRenderTarget(1024, 1024, DXGI_FORMAT_R32_FLOAT, SHADER.m_cb8_Light.GetWork().DL.size() * 4);
	m_dirLightZBuffer = std::make_shared<KdTexture>();
	m_dirLightZBuffer->CreateDepthStencil(1024, 1024, DXGI_FORMAT_R32_TYPELESS, SHADER.m_cb8_Light.GetWork().DL.size() * 4);

	return true;
}

void KdGenShadowMapShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_VS_Skin);
	KdSafeRelease(m_inputLayout_Skin);

	KdSafeRelease(m_PS);
	
	m_cb0.Release();
}

void KdGenShadowMapShader::SetToDevice()
{
	// ���_�V�F�[�_���Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ���_���C�A�E�g���Z�b�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	
	// �s�N�Z���V�F�[�_���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);
	
	//---------------------
	// �萔�o�b�t�@���Z�b�g
	//---------------------
	// �I�u�W�F�N�g�萔�o�b�t�@
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
}

void KdGenShadowMapShader::DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials, bool isSkinMesh)
{
	if (mesh == nullptr)return;

	// ���_�V�F�[�_�Z�b�g
	if (isSkinMesh) // �X�L�����b�V��
	{
		D3D.GetDevContext()->VSSetShader(m_VS_Skin, 0, 0);
		// ���_���C�A�E�g���Z�b�g
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout_Skin);
	}
	else // �X�^�e�B�b�N���b�V��
	{
		// ���_�V�F�[�_���Z�b�g
		D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
		// ���_���C�A�E�g���Z�b�g
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	}

	// �萔�o�b�t�@��������
	m_cb0.Write();

	// ���b�V�������Z�b�g
	mesh->SetToDevice();

	// �S�T�u�Z�b�g
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// �ʂ��P���������ꍇ�̓X�L�b�v
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		//�}�e���A��
			const KdMaterial* material = nullptr;
		material = materials[mesh->GetSubsets()[subi].MaterialNo].get();

		//-----------------------
		// �e�N�X�`���Z�b�g
		//-----------------------
		ID3D11ShaderResourceView* srvs[1] = {};

		// BaseColor Texture
		srvs[0] = material->BaseColorTex->GetSRView();

		// �Z�b�g
		D3D.GetDevContext()->PSSetShaderResources(0, _countof(srvs), srvs);

		//-----------------------
		// �T�u�Z�b�g�`��
		//-----------------------
		mesh->DrawSubset(subi);
	}
}

void KdGenShadowMapShader::Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform)
{
	if (nodeOL == nullptr)return;

	//--------------------------
	// �{�[���X�L�j���O�s��p�̃o�b�t�@���Z�b�g����
	//--------------------------
	if (nodeOL->GetBoneStructuredBuffer().GetWork().size() >= 1)
	{
		D3D.GetDevContext()->VSSetShaderResources(100, 1, nodeOL->GetBoneStructuredBuffer().GetSRViewAddress());
	}

	//--------------------------
	// �S�m�[�h(���b�V��)��`��
	//--------------------------
	for (auto&& meshIdx : nodeOL->GetMeshNodeIndices())
	{
		auto& node = nodeOL->GetAllNodes()[meshIdx];

		// �s��Z�b�g
		m_cb0.Work().mW = node.Transform * worldTransform;
		m_cb0.Write();

		// ���b�V���`��
		DrawMesh(node.Mesh.get(), materials, node.Mesh->IsSkinMesh());
	}
}