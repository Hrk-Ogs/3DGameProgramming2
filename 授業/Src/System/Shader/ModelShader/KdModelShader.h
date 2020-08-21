#pragma once

class KdModelShader
{
public:
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

private:
	ID3D11VertexShader* m_VS = nullptr;				// ���_�V�F�[�_�[
	ID3D11InputLayout* m_inputLayout = nullptr;		// ���_���̓��C�A�E�g

	ID3D11PixelShader* m_PS = nullptr;				// �s�N�Z���V�F�[�_�[

	// �萔�o�b�t�@�p�̍\����
	struct cbObject
	{
		KdMatrix	mW;	// ���[���h�s��
	};
	// �萔�o�b�t�@
	KdConstantBuffer<cbObject> m_cb0;
};