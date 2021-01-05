#include "System/KdSystem.h"
#include "KdPostProcessShader.h"

void KdPostProcessShader::ColorDraw(const KdTexture* tex, const KdVec4& color)
{
	if (tex == nullptr)return;

	// ���݃Z�b�g����Ă���DS���L�����Ă���
	ComPtr<ID3D11DepthStencilState> saveDS;
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	// �萔�o�b�t�@
	m_cb0_Color.Work().Color = color; // �l�Z�b�g
	m_cb0_Color.Write(); // ��������(GPU���֓]��)
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0_Color.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Color.GetAddress());

	// �e�N�X�`���Z�b�g
	D3D.GetDevContext()->PSSetShaderResources(0, 1, tex->GetSRViewAddress());

	// ���_�V�F�[�_�[�Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ���̓��C�A�E�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// �s�N�Z���V�F�[�_�[���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_colorPS, 0, 0);

	// ���_�f�[�^
	Vertex v[4] = {
		{ {-1,-1,0}, {0, 1} },// 0
		{ {-1, 1,0}, {0, 0} },// 1
		{ { 1,-1,0}, {1, 1} },// 2
		{ { 1, 1,0}, {1, 0} } // 3
	};

	// ���_�`��
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // ���_���ǂ����������Ɏg�p���邩�H�̃t���O
		4, // �g�p���钸�_��
		&v[0], // ���_�z��̐擪�A�h���X
		sizeof(Vertex) // �P���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// DS�����ɖ߂�
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}

bool KdPostProcessShader::Init()
{
	//-------------------------------------
	// ���_�V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdPostProcess_VS.inc"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) 
		{
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// �P���_�̏ڍׂȏ��
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_�C���v�b�g���C�A�E�g�쐬
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0],
			layout.size(),
			compiledBuffer,
			sizeof(compiledBuffer),
			&m_inputLayout))
			) {
			assert(0 && "CreateInputLayout���s");
			Release();
			return false;
		}
	}

	// �F�`��
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdPostProcess_Color_PS.inc"
		// �s�N�Z���V�F�[�_�쐬
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_colorPS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}
	// �萔�o�b�t�@�쐬
	m_cb0_Color.Create();

	// �ڂ���
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdPostProcess_Blue_PS.inc"
		// �s�N�Z���V�F�[�_�쐬
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_blurPS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}
	// �萔�o�b�t�@�쐬
	m_cb0_Blur.Create();

	// ���P�x���o
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdPostProcess_BrightFiltering_PS.inc"
		// �s�N�Z���V�F�[�_�쐬
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_brightFilteringPS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}

	// �g�D�[���}�b�s���O
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdPostProcess_ToneMapping_PS.inc"
		// �s�N�Z���V�F�[�_�쐬
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_toneMappingPS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}

	// �g
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdPostProcess_WaveHeight_PS.inc"
		// �s�N�Z���V�F�[�_�쐬
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_waveHeightPS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "KdPostProcess_WaveNormal_PS.inc"

		// �s�N�Z���V�F�[�_�쐬
		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_waveNormalPS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}
	// �萔�o�b�t�@�쐬
	m_cb0_Wave.Create();

	return true;
}

void KdPostProcessShader::Release()
{
	KdSafeRelease(m_VS);
	KdSafeRelease(m_inputLayout);

	KdSafeRelease(m_colorPS);
	m_cb0_Color.Release();

	KdSafeRelease(m_blurPS);
	m_cb0_Blur.Release();

	KdSafeRelease(m_brightFilteringPS);

	KdSafeRelease(m_waveHeightPS);
	KdSafeRelease(m_waveNormalPS);
	m_cb0_Wave.Release();
}

void KdPostProcessShader::AdvanceWave(KdWaveTexture& wave)
{
	ComPtr<ID3D11RenderTargetView> saveRTV;
	ComPtr<ID3D11DepthStencilView> saveDSV;
	D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

	//---------------------------
	// �萔�o�b�t�@�X�V
	//---------------------------
	// ���ʂɃ����_���ȗ͂�������
	m_cb0_Wave.Work().AddPowerUV.x = KdMTRand::s_rnd.GetFloat(0, 1); // U���W
	m_cb0_Wave.Work().AddPowerUV.y = KdMTRand::s_rnd.GetFloat(0, 1); // V���W
	m_cb0_Wave.Work().AddPowerRadius = KdMTRand::s_rnd.GetFloat(1.0f / 512, 3.0f / 512); // ���a
	m_cb0_Wave.Work().AddPower = KdMTRand::s_rnd.GetFloat(-0.3f, 0.3f); // ��
	m_cb0_Wave.Write();
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Wave.GetAddress());

	//---------------------------
	// �����E�����}�b�v�X�V
	//---------------------------
	// RT�ύX
	D3D.GetDevContext()->OMSetRenderTargets(1, wave.m_rtHeight[0]->GetRTViewAddress(), nullptr);
	D3D.SetViewportFromRTV(wave.m_rtHeight[0]->GetRTView());

	// �e�N�X�`���Z�b�g
	D3D.GetDevContext()->PSSetShaderResources(0, 1, wave.m_rtHeight[1]->GetSRViewAddress());

	// �e�V�F�[�_���Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	D3D.GetDevContext()->PSSetShader(m_waveHeightPS, 0, 0);

	// ���_�f�[�^
	Vertex v[4] = {
{ {-1,-1,0}, {0, 1} },// 0
{ {-1, 1,0}, {0, 0} },// 1
{ { 1,-1,0}, {1, 1} },// 2
{ { 1, 1,0}, {1, 0} } // 3
	};
	// ���_�`��
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4, // �g�p���钸�_��
		&v[0], // ���_�z��̐擪�A�h���X
		sizeof(Vertex) // �P���_�̃o�C�g�T�C�Y
	);

	// [0]��[1]�����ւ���
	std::swap(wave.m_rtHeight[0], wave.m_rtHeight[1]);

	//---------------------------
	// �@���}�b�v�X�V
	//---------------------------
	// RT�ύX
	D3D.GetDevContext()->OMSetRenderTargets(1, wave.m_rtNormal->GetRTViewAddress(), nullptr);
	D3D.SetViewportFromRTV(wave.m_rtNormal->GetRTView());

	// �����}�b�v�e�N�X�`���Z�b�g
		D3D.GetDevContext()->PSSetShaderResources(0, 1, wave.m_rtHeight[1]->GetSRViewAddress());

	// �V�F�[�_���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_waveNormalPS, 0, 0);

	// ���_�`��
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		4, // �g�p���钸�_��
		&v[0], // ���_�z��̐擪�A�h���X
		sizeof(Vertex) // �P���_�̃o�C�g�T�C�Y
	);

	//---------------------------
	// ��n��
	//---------------------------
	// �e�N�X�`��������
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// RT��Z�o�b�t�@�����ɖ߂�
	D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get());
	D3D.SetViewportFromRTV(saveRTV.Get());
}

void KdPostProcessShader::BrightFiltering(const KdTexture* destRT, const KdTexture* srcTex)
{
	if (destRT == nullptr || srcTex == nullptr)return;

	// ���݂�RT��Z�o�b�t�@���L�����Ă���
	ComPtr<ID3D11RenderTargetView> saveRTV[8] = {};
	ComPtr<ID3D11DepthStencilView> saveDSV;
	D3D.GetDevContext()->OMGetRenderTargets(8, saveRTV[0].GetAddressOf(), &saveDSV);

	// RT�ύX
	D3D.GetDevContext()->OMSetRenderTargets(1, destRT->GetRTViewAddress(), nullptr);

	// �V�F�[�_�[
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	D3D.GetDevContext()->PSSetShader(m_brightFilteringPS, 0, 0);
	// ���̓��C�A�E�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);

	// �e�N�X�`���Z�b�g
	D3D.GetDevContext()->PSSetShaderResources(0, 1, srcTex->GetSRViewAddress());

	// ���_�f�[�^
	Vertex v[4] = {
	{ {-1,-1,0}, {0, 1} },// 0
	{ {-1, 1,0}, {0, 0} },// 1
	{ { 1,-1,0}, {1, 1} },// 2
	{ { 1, 1,0}, {1, 0} } // 3
	};

	// ���_�`��
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // ���_���ǂ����������Ɏg�p���邩�H�̃t���O
		4, // �g�p���钸�_��
		&v[0], // ���_�z��̐擪�A�h���X
		sizeof(Vertex) // �P���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// RT��Z�o�b�t�@�����Ƃɖ߂�
	D3D.GetDevContext()->OMSetRenderTargets(8, saveRTV->GetAddressOf(), saveDSV.Get());
}

void KdPostProcessShader::BlurDraw(const KdTexture* tex, const KdVec2& dir)
{
	if (tex == nullptr)return;

	// ���݃Z�b�g����Ă���DS���L�����Ă���
	ComPtr<ID3D11DepthStencilState> saveDS;
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	//-------------------------
	// �K�E�X�ڂ����̐ݒ�
	//-------------------------
	// �e�N�Z���T�C�Y(�P�s�N�Z����UV���W�n�ł̃T�C�Y)
	KdVec2 ts;
	ts.x = 1.0f / tex->GetWidth();
	ts.y = 1.0f / tex->GetHeight();
	// �I�t�Z�b�g���W�Əd�݂̐ݒ�
	float totalWeight = 0;
	const float dispersion = 3; // ���U
	for (int i = 0; i < 31; i++)
	{
		int pt = i - 15; // -15 �` +15
		// �I�t�Z�b�g���W
		m_cb0_Blur.Work().Offset[i].x = dir.x * (pt * ts.x);
		m_cb0_Blur.Work().Offset[i].y = dir.y * (pt * ts.y);
		// �d��(�K�E�X�֐�)
		float w = exp(-(pt * pt) / (2 * dispersion * dispersion));
		m_cb0_Blur.Work().Offset[i].z = w;
		totalWeight += w;
	}
	// �d�ݐ��K��
	for (int i = 0; i < 31; i++)
	{
		m_cb0_Blur.Work().Offset[i].z /= totalWeight;
	}

	// ��������(GPU���֓]��)
	m_cb0_Blur.Write();

	// �萔�o�b�t�@�Z�b�g
	D3D.GetDevContext()->VSSetConstantBuffers(0, 1, m_cb0_Blur.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers(0, 1, m_cb0_Blur.GetAddress());

	// �e�N�X�`���Z�b�g
	D3D.GetDevContext()->PSSetShaderResources(0, 1, tex->GetSRViewAddress());

	// ���_�V�F�[�_�[�Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ���̓��C�A�E�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// �s�N�Z���V�F�[�_�[���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_blurPS, 0, 0);

	// ���_�f�[�^
	Vertex v[4] = {
	{ {-1,-1,0}, {0, 1} },// 0
	{ {-1, 1,0}, {0, 0} },// 1
	{ { 1,-1,0}, {1, 1} },// 2
	{ { 1, 1,0}, {1, 0} } // 3
	};

	// ���_�`��
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // ���_���ǂ����������Ɏg�p���邩�H�̃t���O
		4, // �g�p���钸�_��
		&v[0], // ���_�z��̐擪�A�h���X
		sizeof(Vertex) // �P���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// DS�����ɖ߂�
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}

void KdPostProcessShader::GenerateBlur(KdBlurTexture& blurTex, const KdTexture* srcTex)
{
	//---------------------
	// ���݂�RT��Z�o�b�t�@���L�����Ă���
	//---------------------
	ComPtr<ID3D11RenderTargetView> saveRTV;
	ComPtr<ID3D11DepthStencilView> saveDSV;
	D3D.GetDevContext()->OMGetRenderTargets(1, &saveRTV, &saveDSV);

	// MGF
	for (int i = 0; i < 5; i++)
	{
		//---------------------
		// �k���R�s�[
		//---------------------
		// RT�ύX
		D3D.GetDevContext()->OMSetRenderTargets(1, blurTex.m_rt[i][0]->GetRTViewAddress(), nullptr);
		// �r���[�|�[�g��ݒ�
		D3D.SetViewportFromRTV(blurTex.m_rt[i][0]->GetRTView());
		if (i == 0) {
			ColorDraw(srcTex);
		}
		else {
			ColorDraw(blurTex.m_rt[i - 1][0].get());
		}


		//---------------------
		// ���ڂ���
		//---------------------
		// RT�ύX
		D3D.GetDevContext()->OMSetRenderTargets(1, blurTex.m_rt[i][1]->GetRTViewAddress(), nullptr);
		// �r���[�|�[�g��ݒ�
		D3D.SetViewportFromRTV(blurTex.m_rt[i][1]->GetRTView());

		BlurDraw(blurTex.m_rt[i][0].get(), { 1, 0 });

		//---------------------
		// �c�ڂ���
		//---------------------
		// RT�ύX
		D3D.GetDevContext()->OMSetRenderTargets(1, blurTex.m_rt[i][0]->GetRTViewAddress(), nullptr);
		// �r���[�|�[�g��ݒ�
		D3D.SetViewportFromRTV(blurTex.m_rt[i][0]->GetRTView());

		BlurDraw(blurTex.m_rt[i][1].get(), { 0, 1 });
	}

	//---------------------
	// RT��Z�o�b�t�@�����Ƃɖ߂�
	//---------------------
	D3D.GetDevContext()->OMSetRenderTargets(1, saveRTV.GetAddressOf(), saveDSV.Get());
	D3D.SetViewportFromRTV(saveRTV.Get());
}

void KdPostProcessShader::ToneMappingDraw(const KdTexture* tex)
{
	if (tex == nullptr)return;

	// ���݃Z�b�g����Ă���DS���L�����Ă���
	ComPtr<ID3D11DepthStencilState> saveDS;
	UINT saveStencilRef = 0;
	D3D.GetDevContext()->OMGetDepthStencilState(&saveDS, &saveStencilRef);
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

	// �e�N�X�`���Z�b�g
	D3D.GetDevContext()->PSSetShaderResources(0, 1, tex->GetSRViewAddress());

	// ���_�V�F�[�_�[�Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS, 0, 0);
	// ���̓��C�A�E�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout);
	// �s�N�Z���V�F�[�_�[���Z�b�g
	D3D.GetDevContext()->PSSetShader(m_toneMappingPS, 0, 0);

	// ���_�f�[�^
	Vertex v[4] = {
	{ {-1,-1,0}, {0, 1} },// 0
	{ {-1, 1,0}, {0, 0} },// 1
	{ { 1,-1,0}, {1, 1} },// 2
	{ { 1, 1,0}, {1, 0} } // 3
	};

	// ���_�`��
	D3D.DrawVertices(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, // ���_���ǂ����������Ɏg�p���邩�H�̃t���O
		4, // �g�p���钸�_��
		&v[0], // ���_�z��̐擪�A�h���X
		sizeof(Vertex) // �P���_�̃o�C�g�T�C�Y
	);

	// �e�N�X�`��������
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D.GetDevContext()->PSSetShaderResources(0, 1, &nullSRV);

	// DS�����ɖ߂�
	D3D.GetDevContext()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
}