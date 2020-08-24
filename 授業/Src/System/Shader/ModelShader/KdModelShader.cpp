#include "System/KdSystem.h"

#include "KdModelShader.h"

bool KdModelShader::Init()
{
	//--------------------------------
	// ���_�V�F�[�_�[
	//--------------------------------
	{
		#include "KdModelShader_VS.inc"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// 1���_�̏ڍׂȏ��
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_���C�A�E�g�쐬
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

	//--------------------------------
	// �s�N�Z���V�F�[�_�[
	//--------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdModelShader_PS.inc"
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}

	//--------------------------------
	// �萔�o�b�t�@�쐬
	//--------------------------------
	m_cb0.Create();
	m_cb1_Material.Create();

	return true;
}

void KdModelShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);
	KdSafeRelease(m_PS);

	m_cb0.Release();
	m_cb1_Material.Release();
}

void KdModelShader::SetToDevice()
{
	// ���_�V�F�[�_���Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ���_���C�A�E�g���Z�b�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// �s�N�Z���V�F�[�_���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_PS, 0, 0);

	//--------------------------
	// �萔�o�b�t�@���Z�b�g
	//--------------------------
	// �I�u�W�F�N�g�萔�o�b�t�@�i0�ԖڂɃZ�b�g�j
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0.GetAddress());
	// �}�e���A���萔�o�b�t�@
	D3D.GetDevContext()->VSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(1, 1, m_cb1_Material.GetAddress());
}

void KdModelShader::DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials)
{
	if (mesh == nullptr)return;

	// ���b�V�������Z�b�g
	mesh->SetToDevice();

	// �S�T�u�Z�b�g
	for (UINT subi = 0; subi < mesh->GetSubsets().size(); subi++)
	{
		// �ʂ��P���������ꍇ�̓X�L�b�v
		if (mesh->GetSubsets()[subi].FaceCount == 0)continue;

		// �}�e���A��
		const KdMaterial& material = *materials[mesh->GetSubsets()[subi].MaterialNo];

		//---------------------------
		// �}�e���A������萔�o�b�t�@�֏�������
		//---------------------------
		// ��{�F
		m_cb1_Material.Work().BaseColor = material.BaseColor;
		// �����F
		m_cb1_Material.Work().Emissive = material.Emissive;
		// ������
		m_cb1_Material.Work().Matallic = material.Metallic;
		// �e��
		m_cb1_Material.Work().Roughness = material.Roughness;
		// GPU�֓]��
		m_cb1_Material.Write();

		//------------------------------
		// �e�N�X�`���Z�b�g
		//------------------------------
		// �������̃e�N�X�`������C�ɓn���悤�ɁA�z���p��
		std::array<ID3D11ShaderResourceView*, 4> srvs = {};

		// BaseColor�e�N�X�`��
		srvs[0] = material.BaseColorTex->GetSRView();
		// Emissive�e�N�X�`��
		srvs[1] = material.EmissiveTex->GetSRView();
		// Metallic Roughness
		srvs[2] = material.MetallicRoughnessTex->GetSRView();
		// NormalMap
		srvs[3] = material.NormalTex->GetSRView();

		// �Z�b�g 0�Ԗڂ̃X���b�g����4�����Z�b�g����
		D3D.GetDevContext()->PSSetShaderResources(0, srvs.size(), &srvs[0]);

		// �T�u�Z�b�g�`��
		mesh->DrawSubset(subi);
	}
}

void KdModelShader::Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform)
{
	if (nodeOL == nullptr)return;

	//--------------------------
	// �S�m�[�h(���b�V��)��`��
	//--------------------------
	// ���b�V���������Ă���m�[�h��Index�S��
	for (auto&& meshIdx : nodeOL->GetMeshNodeIndices())
	{
		// ���ۂ̃��b�V���̓����Ă���m�[�h
		auto& node = nodeOL->GetAllNodes()[meshIdx];

		// �萔�o�b�t�@�Ƀf�[�^����������
		m_cb0.Work().mW = node.Transform * worldTransform;
		// �R���Ŏ��ۂ�GPU���̃o�b�t�@�֏������܂�A�V�F�[�_�[�Ŏg�p�o����悤�ɂȂ�
		m_cb0.Write();

		// ���b�V���`��
		DrawMesh(node.Mesh.get(), materials);
	}
}
