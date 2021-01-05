#pragma once

//=======================================
// �u���[�e�N�X�`��
//=======================================
struct KdBlurTexture
{
	// �e�N�X�`���쐬
	void Create(int w, int h)
	{
		// �����_�[�^�[�Q�b�g�e�N�X�`��(RT)�Ƃ��č쐬
		int divideValue = 2;
		for (int i = 0; i < 5; i++)
		{
			m_rt[i][0] = std::make_shared<KdTexture>();
			m_rt[i][0]->CreateRenderTarget(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			m_rt[i][1] = std::make_shared<KdTexture>();
			m_rt[i][1]->CreateRenderTarget(w / divideValue, h / divideValue, DXGI_FORMAT_R16G16B16A16_FLOAT);
			divideValue *= 2;
		}
	}

	// ��Ɨp�����_�[�^�[�Q�b�g�e�N�X�`��
	KdSptr<KdTexture> m_rt[5][2];
};

//=======================================
// �g�e�N�X�`��
//=======================================
struct KdWaveTexture
{
	// �����}�b�v
	KdSptr<KdTexture> m_rtHeight[2];
	// �@���}�b�v
	KdSptr<KdTexture> m_rtNormal;

	// �쐬
	void Create(int w, int h)
	{
		m_rtHeight[0] = std::make_shared<KdTexture>();
		m_rtHeight[1] = std::make_shared<KdTexture>();
		m_rtNormal = std::make_shared<KdTexture>();

		m_rtHeight[0]->CreateRenderTarget(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtHeight[1]->CreateRenderTarget(w, h, DXGI_FORMAT_R16G16_FLOAT);
		m_rtNormal->CreateRenderTarget(w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
		// �����}�b�v�N���A
		D3D.GetDevContext()->ClearRenderTargetView(m_rtHeight[0]->GetRTView(), KdVec4(0, 0, 0, 0));
		D3D.GetDevContext()->ClearRenderTargetView(m_rtHeight[1]->GetRTView(), KdVec4(0, 0, 0, 0));
	}
};

//=======================================================
//
// �|�X�g�v���Z�X�V�F�[�_�[
//
//=======================================================
class KdPostProcessShader
{
public:

	// ���_�`��
	struct Vertex
	{
		Math::Vector3 Pos;
		Math::Vector2 UV;
	};

	// �F�`��
	void ColorDraw(const KdTexture* tex, const KdVec4& color = { 1,1,1,1 });

	// �ڂ����`��
	// �Etex �c �\�[�X�e�N�X�`��
	// �Edir �c �ڂ��������x�N�g��( {1,0}�ō��E�����̂ڂ��� {0,1}�ŏ㉺�����ɂڂ���)
	void BlurDraw(const KdTexture* tex, const KdVec2& dir);

	// �ڂ����摜�𐶐�����
	// �EblurTex �c �o�͐�̃e�N�X�`��
	// �EsrcTex �c �\�[�X�e�N�X�`��
	void GenerateBlur(KdBlurTexture& blurTex, const KdTexture* srcTex);

	// ���P�x���o
	// �EdestRT �c ���P�x�F������e�N�X�`��
	// �EsrcTex �c ���o���e�N�X�`��
	void BrightFiltering(const KdTexture* destRT, const KdTexture* srcTex);

	// �g�[���}�b�s���O�`��
	void ToneMappingDraw(const KdTexture* tex);

	// �g�i�s
	// �Ewave �c �X�V����g�e�N�X�`��
	void AdvanceWave(KdWaveTexture& wave);

	//================================================
	// �������E���
	//================================================
	// ������
	bool Init();
	// ���
	void Release();
	//
	~KdPostProcessShader()
	{
		Release();
	}

private:
	// ���_�V�F�[�_
	ID3D11VertexShader* m_VS = nullptr; // ���_�V�F�[�_�[
	ID3D11InputLayout* m_inputLayout = nullptr; // ���_���̓��C�A�E�g

	// �s�N�Z���V�F�[�_(�F�`��)
	ID3D11PixelShader* m_colorPS = nullptr;
	// �s�N�Z���V�F�[�_(�ڂ���)
	ID3D11PixelShader* m_blurPS = nullptr;
	// �s�N�Z���V�F�[�_(���P�x���o)
	ID3D11PixelShader* m_brightFilteringPS = nullptr;
	// �s�N�Z���V�F�[�_(�g�[���}�b�s���O)
	ID3D11PixelShader* m_toneMappingPS = nullptr;
	// �s�N�Z���V�F�[�_(�g)
	ID3D11PixelShader* m_waveHeightPS = nullptr;
	ID3D11PixelShader* m_waveNormalPS = nullptr;

	// �萔�o�b�t�@(Color�`��p)
	struct cbCoolor {
		KdVec4 Color = { 1,1,1,1 };
	};
	KdConstantBuffer<cbCoolor> m_cb0_Color;

	// �萔�o�b�t�@(Blur�`��p)
	struct cbBlur {
		KdVec4 Offset[31];
	};
	KdConstantBuffer<cbBlur> m_cb0_Blur;

	// �萔�o�b�t�@(Wave�p)
	struct cbWave {
		KdVec2 AddPowerUV; // �͂�������UV���W
		float AddPowerRadius; // ���̔��a
		float AddPower; // �З�
	};
	KdConstantBuffer<cbWave> m_cb0_Wave;
};