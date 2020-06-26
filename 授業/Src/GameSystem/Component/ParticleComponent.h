#pragma once

#include "BaseComponent.h"

//=======================================================
//
// �p�[�e�B�N�X�R���|�[�l���g
// �E��ʂ̔|���S���𔭐��E�Ǘ��E�`�悷��G�t�F�N�g�p�̃G�~�b�^�[�B
//
//=======================================================
class ParticleComponent : public BaseComponent
{
public:
	// �G�t�F�N�g���ŏ�����Đ�����
	void Restart()
	{
		m_time = -m_param.DelayTime;
		m_nodes.clear();
	}
	//===============================
	// �R���|�[�l���g����
	//===============================
	// �X�V����
	virtual void Update() override;
	// �`�揀��
	virtual void PrepareDraw(RenderingData& rdata) override;
	// �`�揈��
	virtual void Draw(int phaseID) override;
	// (Editor�p)ImGui
	virtual void Editor_ImGuiUpdate() override;
	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);
		KdJsonGet(jsonData["Param.Duration"], m_param.Duration);
		KdJsonGet(jsonData["Param.Loop"], m_param.Loop);
		KdJsonGet(jsonData["Param.PopupNum"], m_param.PopupNum);
		KdJsonGet(jsonData["Param.PopupInterval"], m_param.PopupInterval);
		KdJsonGet(jsonData["Param.LifeTime"], m_param.LifeTime);
		std::string texFilename;
		KdJsonGet(jsonData["Param.Tex"], texFilename);
		m_param.Tex = KDResFactory.GetTexture(texFilename);
		if (m_param.Tex == nullptr)
		{
			m_param.Tex = D3D.GetWhiteTex();
		}
		KdJsonGet(jsonData["Param.BillboardType"], m_param.BillboardType);
		KdJsonGet(jsonData["Param.BlendMode"], m_param.BlendMode);
		KdJsonGet(jsonData["Param.StopAction"], m_param.StopAction);
		KdJsonGet(jsonData["Param.Flags"], m_param.Flags);
		KdJsonGet(jsonData["Param.DelayTime"], m_param.DelayTime);
		KdJsonGet(jsonData["Param.Color"], m_param.Color);
		KdJsonGet(jsonData["Param.EndColor"], m_param.EndColor);
		// ��{
		KdJsonGet(jsonData["Param.PosStart"], m_param.PosStart);
		KdJsonGet(jsonData["Param.PosRandom"], m_param.PosRandom);
		KdJsonGet(jsonData["Param.SizeStart"], m_param.SizeStart);
		KdJsonGet(jsonData["Param.SizeRandom"], m_param.SizeRandom);
		KdJsonGet(jsonData["Param.AngleStart"], m_param.AngleStart);
		KdJsonGet(jsonData["Param.AngleRandom"], m_param.AngleRandom);
		// ���x
		KdJsonGet(jsonData["Param.MoveStart"], m_param.MoveStart);
		KdJsonGet(jsonData["Param.MoveRandom"], m_param.MoveRandom);
		KdJsonGet(jsonData["Param.ScaleStart"], m_param.ScaleStart);
		KdJsonGet(jsonData["Param.ScaleRandom"], m_param.ScaleRandom);
		KdJsonGet(jsonData["Param.RotateStart"], m_param.RotateStart);
		KdJsonGet(jsonData["Param.RotateRandom"], m_param.RotateRandom);
		// �����x
		KdJsonGet(jsonData["Param.MoveAccelStart"], m_param.MoveAccelStart);
		KdJsonGet(jsonData["Param.MoveAccelRandom"], m_param.MoveAccelRandom);
		KdJsonGet(jsonData["Param.MoveAccelMultiply"], m_param.MoveAccelMultiply);
		KdJsonGet(jsonData["Param.RotateAccelStart"], m_param.RotateAccelStart);
		KdJsonGet(jsonData["Param.RotateAccelRandom"], m_param.RotateAccelRandom);
		KdJsonGet(jsonData["Param.ScaleAccelStart"], m_param.ScaleAccelStart);
		KdJsonGet(jsonData["Param.ScaleAccelRandom"], m_param.ScaleAccelRandom);
		// UV�A�j��
		auto& uvAry = jsonData["Param.UVAnim_SliceNum"].array_items();
		if (uvAry.size() == 2)
		{
			m_param.UVAnim_SliceNum[0] = uvAry[0].int_value();
			m_param.UVAnim_SliceNum[1] = uvAry[1].int_value();
		}
		KdJsonGet(jsonData["Param.UVAnim_Loop"], m_param.UVAnim_Loop);
		KdJsonGet(jsonData["Param.UVAnim_Speed"], m_param.UVAnim_Speed);
		KdJsonGet(jsonData["Param.UVAnim_StartTime"], m_param.UVAnim_StartTime);
		// ���̑�
		KdJsonGet(jsonData["Param.FollowTransform"], m_param.FollowTransform);
	}
	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseComponent::Serialize(outJsonObj);
		outJsonObj["Param.Duration"] = m_param.Duration;
		outJsonObj["Param.Loop"] = m_param.Loop;
		outJsonObj["Param.PopupNum"] = m_param.PopupNum;
		outJsonObj["Param.PopupInterval"] = m_param.PopupInterval;
		outJsonObj["Param.LifeTime"] = m_param.LifeTime;
		outJsonObj["Param.Tex"] = m_param.Tex->GetFilepath();
		outJsonObj["Param.BillboardType"] = m_param.BillboardType;
		outJsonObj["Param.BlendMode"] = m_param.BlendMode;
		outJsonObj["Param.StopAction"] = m_param.StopAction;
		outJsonObj["Param.Flags"] = m_param.Flags;
		outJsonObj["Param.DelayTime"] = m_param.DelayTime;
		outJsonObj["Param.Color"] = m_param.Color.ToArray();
		outJsonObj["Param.EndColor"] = m_param.EndColor.ToArray();
		// ��{
		outJsonObj["Param.PosStart"] = m_param.PosStart.ToArray();
		outJsonObj["Param.PosRandom"] = m_param.PosRandom.ToArray();
		outJsonObj["Param.SizeStart"] = m_param.SizeStart.ToArray();
		outJsonObj["Param.SizeRandom"] = m_param.SizeRandom.ToArray();
		outJsonObj["Param.AngleStart"] = m_param.AngleStart.ToArray();
		outJsonObj["Param.AngleRandom"] = m_param.AngleRandom.ToArray();
		// ���x
		outJsonObj["Param.MoveStart"] = m_param.MoveStart.ToArray();
		outJsonObj["Param.MoveRandom"] = m_param.MoveRandom.ToArray();
		outJsonObj["Param.RotateStart"] = m_param.RotateStart.ToArray();
		outJsonObj["Param.RotateRandom"] = m_param.RotateRandom.ToArray();
		outJsonObj["Param.ScaleStart"] = m_param.ScaleStart.ToArray();
		outJsonObj["Param.ScaleRandom"] = m_param.ScaleRandom.ToArray();
		// �����x
		outJsonObj["Param.MoveAccelStart"] = m_param.MoveAccelStart.ToArray();
		outJsonObj["Param.MoveAccelRandom"] = m_param.MoveAccelRandom.ToArray();
		outJsonObj["Parm.MoveAccelMultiply"] = m_param.MoveAccelMultiply.ToArray();
		outJsonObj["Param.ScaleAccelStart"] = m_param.ScaleAccelStart.ToArray();
		outJsonObj["Param.ScaleAccelRandom"] = m_param.ScaleAccelRandom.ToArray();
		outJsonObj["Param.RotateAccellStart"] = m_param.RotateAccelStart.ToArray();
		outJsonObj["Param.RotateAccelRandom"] = m_param.RotateAccelRandom.ToArray();
		// UV�A�j��
		outJsonObj["Param.UVAnim_SliceNum"] = m_param.UVAnim_SliceNum;
		outJsonObj["Param.UVAnim_Loop"] = m_param.UVAnim_Loop;
		outJsonObj["Param.UVAnim_Speed"] = m_param.UVAnim_Speed;
		outJsonObj["Param.UVAnim_StartTime"] = m_param.UVAnim_StartTime;
		// ���̑�
		outJsonObj["Param.FollowTransform"] = m_param.FollowTransform;
	}
private:

	// �o�ߎ���
	int m_time = 0;

	// �p�[�e�B�N���m�[�h
	// �E�P�̔|���S���f�[�^
	struct Node
	{
		// �l�p�`�|���S��
		KdSquarePolygon Polygon;
		// ��{�f�[�^
		KdVec3 Pos; // ���݂̍��W
		KdVec3 Scale = { 1,1,1 }; // ���݂̊g��
		KdVec3 Rotate; // ���݂̉�]�p�x(�x)
		// ���x�f�[�^
		KdVec3 MoveSpeed; // �ړ����x
		KdVec3 RotateSpeed; // �p���x
		KdVec3 ScaleSpeed; // �g�呬�x
		// �����x�f�[�^
		KdVec3 MoveAccelSpeed; // �ړ������x
		KdVec3 RotateAccelSpeed; // �p�����x
		KdVec3 ScaleAccelSpeed; // �g������x
		int Time = 0; // �o�ߎ���
	};
	// �p�[�e�B�N���m�[�h�z��
	std::list<Node> m_nodes;
	//-------------------
	// ����ݒ�
	//-------------------
	struct Parameter
	{
		int Duration = 300; // �p������
		bool Loop = true; // ���[�v�Đ�
		int PopupNum = 3; // ������
		int PopupInterval = 5; // �����Ԋu
		int LifeTime = 60; // ����
		
		// �e�N�X�`��
		KdSptr<KdTexture> Tex = D3D.GetWhiteTex();
		
		// UV�A�j���[�V����
		std::array<int, 2> UVAnim_SliceNum = { 1, 1 };
		float UVAnim_Speed = 0.0f;
		float UVAnim_StartTime = 0;
		bool UVAnim_Loop = false;
		
		// �r���{�[�h�^�C�v
		int BillboardType = 0; // 0:�Ȃɂ����Ȃ� 1:�J�����Ɍ����� 2:Y���Œ�

		// �u�����h�^�C�v
		int BlendMode = 0; // 0:���������� 1:���Z����

		// �F
		KdVec4 Color = { 1,1,1,1 };

		// �F(�I��)
		KdVec4 EndColor = { 1,1,1,1 };

		// ��~���̏���
		int StopAction = 0;

		// ���̑��t���O
		int Flags = 0;

		// �x������
		int DelayTime = 0;

		// TransformComponent�ɒǏ]
		bool FollowTransform = true;

		// ��{�ݒ�
		KdVec3 PosStart; // ���� ���W
		KdVec3 PosRandom; // PosStart����̃����_���l
		KdVec3 SizeStart = { 1.0f, 1.0f, 1.0f }; // ���� �g��
		KdVec3 SizeRandom; // SizeStart����̃����_���l
		KdVec3 AngleStart; // ���� ��]�p�x
		KdVec3 AngleRandom; // AngleRandom����̃����_���l
		// ���x
		KdVec3 MoveStart; // ���� �ړ����x
		KdVec3 MoveRandom = { 0.1f, 0.1f, 0.1f }; // MoveStart����̃����_���l
		KdVec3 RotateStart; // ��]����
		KdVec3 RotateRandom; // RotateStart����̃����_���l
		KdVec3 ScaleStart; // �g�����
		KdVec3 ScaleRandom; // ScaleStart����̃����_���l
		// �����ݒ�
		KdVec3 MoveAccelStart; // ���� �ړ�����
		KdVec3 MoveAccelRandom; // MoveAccelStart����̃����_���l
		KdVec3 MoveAccelMultiply = { 1,1,1 };	// �ړ���Z
		KdVec3 RotateAccelStart; // ���� ��]����
		KdVec3 RotateAccelRandom; // RotateAccelStart����̃����_���l
		KdVec3 ScaleAccelStart; // ���� �g�����
		KdVec3 ScaleAccelRandom; // ScaleAccelStart����̃����_���l
	};
	Parameter m_param;
	
	
//-------------------------
	// �萔
	//-------------------------
	// BillboardType�萔
	enum {
		kBBType_None = 0,
		kBBType_Billboard = 1, // ���S�ɃJ�����ɍ��킹��
		kBBType_FixedYAxis = 2, // Y���͌Œ肵�āA���̑����J�����ɍ��킹��
	};
	// BlendType�萔
	enum {
		kBlendType_Alpha = 0, // ����������
		kBlendType_Add = 1, // ���Z����
	};
	// StopAction�萔
	enum {
		kStopAction_None = 0, // �������Ȃ�
		kStopAction_Destroy = 1, // ���M��GameObject���폜
	};
	// ���̑��t���O�萔
	enum {
		kEtcFlag_None = 0,
		kEtcFlag_MoveDirToYAxis = 1,
	};
};