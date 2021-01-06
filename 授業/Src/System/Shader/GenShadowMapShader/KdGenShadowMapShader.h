#pragma once

//================================================
// �V���h�E�}�b�v�����V�F�[�_�[
//================================================
class KdGenShadowMapShader
{
public:

	// �W�����p�[�x�}�b�v�擾
	KdSptr<KdTexture> GetSpotShadowMap() const { return m_spotLightShadowMap; }
	// ���s���p�[�x�}�b�v�擾
	KdSptr<KdTexture> GetDirShadowMap() const { return m_dirLightShadowMap; }

	// �X�|�b�g���C�g�p�̐[�x�}�b�v��RT�Ƃ��ăZ�b�g(Z�o�b�t�@��)
	void SetRenderTarget_SpotShadowMap(int idx)
	{
		// RT�ύX
		D3D.GetDevContext()->OMSetRenderTargets(1, m_spotLightShadowMap->GetRTViewAddress(idx), m_spotLightZBuffer->GetDSView(idx));
		D3D.SetViewportFromRTV(m_spotLightShadowMap->GetRTView(idx));
		// �N���A
		D3D.GetDevContext()->ClearRenderTargetView(m_spotLightShadowMap->GetRTView(idx), KdVec4(1, 1, 1, 1));
		D3D.GetDevContext()->ClearDepthStencilView(m_spotLightZBuffer->GetDSView(idx), D3D11_CLEAR_DEPTH, 1, 0);
	}

	// ���s���C�g�p�̐[�x�}�b�v��RT�Ƃ��ăZ�b�g(Z�o�b�t�@��)
	void SetRenderTarget_DirShadowMap(int lightIdx, int cascadeIdx)
	{
		int idx = lightIdx * 4 + cascadeIdx;

		// RT�ύX
		D3D.GetDevContext()->OMSetRenderTargets(1, m_dirLightShadowMap->GetRTViewAddress(idx), m_dirLightZBuffer->GetDSView(idx));
		D3D.SetViewportFromRTV(m_dirLightShadowMap->GetRTView(idx));
		// �N���A
		D3D.GetDevContext()->ClearRenderTargetView(m_dirLightShadowMap->GetRTView(idx), KdVec4(1, 1, 1, 1));
		D3D.GetDevContext()->ClearDepthStencilView(m_dirLightZBuffer->GetDSView(idx), D3D11_CLEAR_DEPTH, 1, 0);
	}

	// ���C�g�J�����s��(�r���[�s�� * �ˉe�s��)�Z�b�g
	void SetLightVP(const KdMatrix& m)
	{
		m_cb0.Work().mLightVP = m;
	}

	//================================================
	// �`��
	//================================================
	// ���̃V�F�[�_���f�o�C�X�փZ�b�g
	void SetToDevice();

	// ���b�V���`��
	// �Emesh �c �`�悷�郁�b�V��
	// �Ematerials �c �g�p����ގ��z��
	void DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials, bool isSkinMesh);

	// ���f���`��
	void Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform);

	//================================================
	// �������E���
	//================================================
	// ������
	bool Init();
	// ���
	void Release();

	~KdGenShadowMapShader()
	{
		Release();
	}

private:
	ID3D11VertexShader* m_VS = nullptr; // ���_�V�F�[�_�[
	ID3D11InputLayout* m_inputLayout = nullptr; // ���_���̓��C�A�E�g
	ID3D11VertexShader* m_VS_Skin = nullptr; //���_�V�F�[�_�[(�X�L�����b�V��)
		ID3D11InputLayout* m_inputLayout_Skin = nullptr; // ���_���̓��C�A�E�g(�X�L�����b�V��)

	ID3D11PixelShader* m_PS = nullptr; // �s�N�Z���V�F�[�_�[

	// �[�x�}�b�v
	KdSptr<KdTexture> m_spotLightShadowMap; // �W�����p �[�x�}�b�v
	KdSptr<KdTexture> m_spotLightZBuffer; // �W�����p Z�o�b�t�@
	KdSptr<KdTexture> m_dirLightShadowMap; // ���s���p �[�x�}�b�v
	KdSptr<KdTexture> m_dirLightZBuffer; // ���s���p Z�o�b�t�@

	// �萔�o�b�t�@
	struct cbObject
	{
		KdMatrix mW; // ���[���h�s��
		KdMatrix mLightVP; // ���C�g�J�����̃r���[�s�� * �ˉe�s��
	};
	KdConstantBuffer<cbObject> m_cb0;
};