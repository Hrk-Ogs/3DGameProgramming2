#pragma once

class KdModelShader
{
public:

	// ���C�gOn/Off
	void SetLightEnable(bool enable)
	{
		m_cb0.Work().LightEnable = enable;
	}
	// �t�H�O�L��/����
	void SetFogEnable(bool enable)
	{
		m_cb0.Work().FogEnable = enable;
	}
	// ��Z�F
	void SetMulMaterialColor(const KdVec4& color)
	{
		m_cb0.Work().MulMaterialColor = color;
	}

	// UV�I�t�Z�b�g�ݒ�
	void SetUVOffset(const KdVec2& uvOffset)
	{
		m_cb0.Work().UVOffset = uvOffset;
	}
	// UV�^�C�����O�ݒ�
	void SetUVTiling(const KdVec2& uvOffset)
	{
		m_cb0.Work().UVTiling = uvOffset;
	}
	// ���ܗ��ݒ�
	void SetRefractiveIndex(float refractiveIndex)
	{
		m_cb0.Work().RefractiveIndex = refractiveIndex;
	}


	// ������
	bool Init();
	// ���
	void Release();
	~KdModelShader() { Release(); }

	//================================================
	// �`��
	//================================================
	
	// ���̃V�F�[�_���g�p�ł���悤�Ƀf�o�C�X�փZ�b�g
	void SetToDevice();

	// �P�̃��b�V����`��
	// �Emesh �c �`�悷�郁�b�V��
	// �Ematerials �c �g�p����ގ��z��
	void DrawMesh(const KdMesh* mesh, const std::vector<KdSptr<KdMaterial>>& materials);
	
	// �m�[�h�A�E�g���C�i�[���̑S�Ẵ��b�V����`�悷��
	// �܂��ADrawMesh�Ƃ͈Ⴂ�֐�����SetWorldMatrix�����s�����
	// �EnodeOL �c �`�悷��A�E�g���C�i�[
	// �Ematerials �c �`��ɂ��悤����}�e���A���z��
	// �EworldTransform �c ���[���h�ϊ��s�� �e�m�[�h�͂��̍s���ŕϊ�����`�悳���
	void Draw(const KdNodeOutliner* nodeOL, const std::vector<KdSptr<KdMaterial>>& materials, const KdMatrix& worldTransform);

	//================================================
	// �֊s�`��
	//================================================
	// �֊s�`��Ƃ��ăf�o�C�X�փZ�b�g
	void SetToDevice_Outline();
	// �P�̃��b�V����`��
	void DrawMesh_Outline(const KdMesh* mesh);
	// �m�[�h�A�E�g���C�i�[���̑S�Ẵ��b�V����`�悷��
	void Draw_Outline(const KdNodeOutliner* nodeOL, const KdMatrix& worldTransform);

private:
	ID3D11VertexShader* m_VS = nullptr;				// ���_�V�F�[�_�[
	ID3D11InputLayout* m_inputLayout = nullptr;		// ���_���̓��C�A�E�g

	ID3D11VertexShader* m_VS_Skin = nullptr; // (�X�L�����b�V��)���_�V�F�[�_�[
	ID3D11InputLayout* m_inputLayout_Skin = nullptr; // (�X�L�����b�V��)���_���̓��C�A�E�g

	ID3D11PixelShader* m_PS = nullptr;				// �s�N�Z���V�F�[�_�[

	// �֊s�`��p
	ID3D11VertexShader* m_outlineVS = nullptr; // ���_�V�F�[�_�[
	ID3D11PixelShader* m_outlinePS = nullptr; // �s�N�Z���V�F�[�_�[

	KdSptr<KdTexture> m_texToon;					// �g�D�[���p�̃e�N�X�`��

	// �萔�o�b�t�@�p�̍\����
	struct cbObject
	{
		KdMatrix	mW;	// ���[���h�s��
		int LightEnable = 1;	// ���C�g�L��
		int FogEnable = 1;		// �t�H�O�L��
		float _space_[2]; // �p�b�L���O�K�������K�v���邽�߁A�S�~�����Ă���

		KdVec4 MulMaterialColor = { 1,1,1,1 }; // ��Z�F

		KdVec2 UVOffset; // UV�I�t�Z�b�g
		KdVec2 UVTiling = { 1,1 }; // UV�^�C�����O
		float RefractiveIndex = 1.0f;// ���ܗ�
		float _space2_[3]; // �p�b�L���O�K�������K�v���邽�߁A�S�~�����Ă���
	};
	// �萔�o�b�t�@
	KdConstantBuffer<cbObject> m_cb0;

	// �萔�o�b�t�@�i�}�e���A���P�ʍX�V�j
	struct cbMaterial {
		KdVec4			BaseColor;
		KdVec3			Emissive;
		float			Matallic;
		float			Roughness;

		float			tmp[3];
	};
	KdConstantBuffer<cbMaterial>	m_cb1_Material;
};