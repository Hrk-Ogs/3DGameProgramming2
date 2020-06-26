#include "../GameSystem.h"
#include "ParticleComponent.h"

void ParticleComponent::Update()
{
	// �������̓X�L�b�v
	if (m_enable == false)return;

	// ���[���h�s��
	auto& mW = GetOwner()->Transform()->GetMatrix();
	//------------------------------
	// �Đ��I����
	//------------------------------
	if (m_time >= m_param.Duration)
	{
		// ���[�v�Đ��H
		if (m_param.Loop)
		{
			m_time = -m_param.DelayTime;
		}
		else
		{
			// �I���������͍Đ����̂ݎ��s
			if (GAMESYS.IsPlay())
			{
				// �I�����̏���
				if (m_param.StopAction == kStopAction_Destroy) // GameObject�폜
				{
					GetOwner()->Destroy(); // GameObject���폜
					return;
				}
			}
		}
	}

	//------------------------------
	// �p�[�e�B�N���m�[�h�̐���
	//------------------------------
	if (m_time >= 0 && m_time < m_param.Duration)
	{
		// �u�����Ԋu�v�̔���
		if (m_time % m_param.PopupInterval == 0)
		{
			// �u�������v�Ԃ�m�[�h���쐬
			for (int i = 0; i < m_param.PopupNum; i++)
			{
				//-----------------
				// �ǉ�
				//-----------------
				m_nodes.emplace_back();
				auto& node = m_nodes.back();
				// �|���S���ݒ�
				node.Polygon.Init(1, 1, { 1,1,1,1 });
				node.Polygon.SetTexture(m_param.Tex);
				node.Polygon.SetAnimationInfo(m_param.UVAnim_SliceNum[0], m_param.UVAnim_SliceNum[1]);// UV�A�j��������
				node.Polygon.SetAnimationPos(m_param.UVAnim_StartTime); // UV�A�j���ʒu
				// ���W �ݒ�
				node.Pos = m_param.PosStart;
				node.Pos.x += KdMTRand::s_rnd.GetFloat(-m_param.PosRandom.x, m_param.PosRandom.x);
				node.Pos.y += KdMTRand::s_rnd.GetFloat(-m_param.PosRandom.y, m_param.PosRandom.y);
				node.Pos.z += KdMTRand::s_rnd.GetFloat(-m_param.PosRandom.z, m_param.PosRandom.z);
				// �g�� �ݒ�
				node.Scale = m_param.SizeStart;
				node.Scale.x += KdMTRand::s_rnd.GetFloat(-m_param.SizeRandom.x, m_param.SizeRandom.x);
				node.Scale.y += KdMTRand::s_rnd.GetFloat(-m_param.SizeRandom.y, m_param.SizeRandom.y);
				node.Scale.z += KdMTRand::s_rnd.GetFloat(-m_param.SizeRandom.z, m_param.SizeRandom.z);
				// ��] �ݒ�
				node.Rotate = m_param.AngleStart;
				node.Rotate.x += KdMTRand::s_rnd.GetFloat(-m_param.AngleRandom.x, m_param.AngleRandom.x);
				node.Rotate.y += KdMTRand::s_rnd.GetFloat(-m_param.AngleRandom.y, m_param.AngleRandom.y);
				node.Rotate.z += KdMTRand::s_rnd.GetFloat(-m_param.AngleRandom.z, m_param.AngleRandom.z);
				// (���x)�ړ� �ݒ�
				node.MoveSpeed = m_param.MoveStart;
				node.MoveSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.MoveRandom.x, m_param.MoveRandom.x);
				node.MoveSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.MoveRandom.y, m_param.MoveRandom.y);
				node.MoveSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.MoveRandom.z, m_param.MoveRandom.z);
				// (���x)�ړ� �ݒ�
				node.RotateSpeed = m_param.RotateStart;
				node.RotateSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.RotateRandom.x, m_param.RotateRandom.x);
				node.RotateSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.RotateRandom.y, m_param.RotateRandom.y);
				node.RotateSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.RotateRandom.z, m_param.RotateRandom.z);
				// (���x)�g�� �ݒ�
				node.ScaleSpeed = m_param.ScaleStart;
				node.ScaleSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.ScaleRandom.x, m_param.ScaleRandom.x);
				node.ScaleSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.ScaleRandom.y, m_param.ScaleRandom.y);
				node.ScaleSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.ScaleRandom.z, m_param.ScaleRandom.z);
				// (�����x)�ړ� �ݒ�
				node.MoveAccelSpeed = m_param.MoveAccelStart;
				node.MoveAccelSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.MoveAccelRandom.x, m_param.MoveAccelRandom.x);
				node.MoveAccelSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.MoveAccelRandom.y, m_param.MoveAccelRandom.y);
				node.MoveAccelSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.MoveAccelRandom.z, m_param.MoveAccelRandom.z);
				// (�����x)�ړ� �ݒ�
				node.RotateAccelSpeed = m_param.RotateAccelStart;
				node.RotateAccelSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.RotateAccelRandom.x, m_param.RotateAccelRandom.x);
				node.RotateAccelSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.RotateAccelRandom.y, m_param.RotateAccelRandom.y);
				node.RotateAccelSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.RotateAccelRandom.z, m_param.RotateAccelRandom.z);
				// (�����x)�g�� �ݒ�
				node.ScaleAccelSpeed = m_param.ScaleAccelStart;
				node.ScaleAccelSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.ScaleAccelRandom.x, m_param.ScaleAccelRandom.x);
				node.ScaleAccelSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.ScaleAccelRandom.y, m_param.ScaleAccelRandom.y);
				node.ScaleAccelSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.ScaleAccelRandom.z, m_param.ScaleAccelRandom.z);
				// Transform�ɉe�����Ȃ��ꍇ�́A���񂾂��ϊ�����
				if (m_param.FollowTransform == false)
				{
					// ���W��ϊ�
					node.Pos.TransformCoord(mW);
					// �ړ��x�N�g���n��ϊ�
					node.MoveSpeed.TransformNormal(mW);
					node.MoveAccelSpeed.TransformNormal(mW);
					// ��]
					node.Rotate += mW.ComputeAngles() * KdToDegrees;
					// �g��
					KdVec3 s = mW.GetScale();
					node.Scale *= s;
					node.ScaleAccelSpeed *= s;
				}
			}
		}
		// ���Ԍo��
		m_time++;
	}

	//------------------------------
	// �S�p�[�e�B�N�X�m�[�h�̏���
	//------------------------------
	for (auto it = m_nodes.begin(); it != m_nodes.end();)
	{
		auto& node = (*it);
		// ���W
		node.MoveSpeed += node.MoveAccelSpeed; // �ړ����x ����
		node.Pos += node.MoveSpeed; // ���W�ړ�
		node.MoveSpeed *= m_param.MoveAccelMultiply;	// �ړ����x��Z

		// ��]
		node.RotateSpeed += node.RotateAccelSpeed; // �p���x ����
		node.Rotate += node.RotateSpeed; // ��]

		// �g��
		node.ScaleSpeed += node.ScaleAccelSpeed; // �g�呬�x ����
		node.Scale += node.ScaleSpeed; // �g��

		// UV�A�j���[�V����
		node.Polygon.AdvanceTime(m_param.UVAnim_Speed, m_param.UVAnim_Loop);

		// ����
		node.Time++;
		if (node.Time >= m_param.LifeTime)
		{
			// ���Ԃ������𒴂�����폜����
			it = m_nodes.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ParticleComponent::PrepareDraw(RenderingData& rdata)
{
	if (m_enable == false)return;

	// �`��o�^
	rdata.m_drawEffectList.push_back(this);
}

void ParticleComponent::Draw(int phaseID)
{
	if (m_enable == false)return;

	// GameObject�̍s��
	auto& mW = GetOwner()->Transform()->GetMatrix();

	// �V�F�[�_�[�Z�b�g
	SHADER.m_effectShader.SetToDevice();

	// �J�����s����Z�o
	KdMatrix mCam = SHADER.m_cb7_Camera.GetWork().mV;
	mCam.Inverse();

	// �u�����h���[�h�ύX
	if (m_param.BlendMode == kBlendType_Alpha) // ����������
	{
		D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	}
	else if (m_param.BlendMode == kBlendType_Add) // ���Z����
	{
		D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Add, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	}

	//------------------------------
	// �S�p�[�e�B�N�X�m�[�h��`��
	//------------------------------
	KdMatrix m;
	for (auto&& node : m_nodes)
	{
		// �������玞�Ԕ䗦�����߂�(0�`1)
		float t = (float)node.Time / (float)m_param.LifeTime;

		//------------------
		// �s��v�Z
		// �E��Ƀr���{�[�f�B���O�v�Z
		//------------------
		// �J�����ɍ��킹��
		if (m_param.BillboardType == kBBType_Billboard)
		{
			// ��]�ȊO�̏��ōs����쐬
			m.CreateScaling(node.Scale);
			m.Translation(node.Pos);
			if (m_param.FollowTransform) m *= mW;
			// �s��̊g�嗦���擾
			KdVec3 scale = m.GetScale();
			// �J�����̉�]�ɍ��킹��(�����Ŋg�听���͂P�ɂȂ��Ă��܂�)
			m.Right(mCam.Right());
			m.Up(mCam.Up());
			m.Forward(mCam.Forward());
			// Z��]�݂̎g�p
			m = KdMatrix().CreateRotationZ(node.Rotate.z * KdToRadians) * m;
			// �擾���Ă������g���K�p
			m = KdMatrix().CreateScaling(scale) * m;

			// �s��Z�b�g
			SHADER.m_effectShader.SetWorldMatrix(m);
		}
		// Y���̂݌Œ�ŁA�J�����̕����Ɍ����悤��XZ�����v�Z
		else if (m_param.BillboardType == kBBType_FixedYAxis) // Y���Œ�
		{
			//--------------
			// �s��쐬
			//--------------
			m.CreateScaling(node.Scale);
			// �ړ��x�N�g����Y���Ƃ��Ďg�p����
			if (m_param.Flags & kEtcFlag_MoveDirToYAxis)
			{
				// �ړ��x�N�g���𐳋K��
				auto vY = node.MoveSpeed;
				vY.Normalize();
				// ���݂�Y���̊g�嗦�ɂ���
				vY *= m.Up().Length();
				// Y���Z�b�g
				m.Up(vY);
			}
			// �ʏ�
			else
			{
				m.RotateX(node.Rotate.x * KdToRadians);
				m.RotateY(node.Rotate.y * KdToRadians);
				m.RotateZ(node.Rotate.z * KdToRadians);
			}
			// �ړ�
			m.Translation(node.Pos);
			// GameObject�̍s��ƍ���
			if (m_param.FollowTransform) m *= mW;

			//--------------
			// X����Z�����J�����ɑ΂���
			// �����ɂȂ�悤�Ɍv�Z�ŋ��߂�B
			//--------------
			// �J��������A�p�[�e�B�N�X�m�[�h�ւ̕���
			KdVec3 v = m.Translation() - mCam.Translation();
			// X�������߂�
			KdVec3 vAxis = KdVec3::sCross(m.Up(), v);
			vAxis.Normalize();
			m.Right(vAxis * m.Right().Length()); // �g�嗦��K�p
			// Z�������߂�
			vAxis = KdVec3::sCross(m.Right(), m.Up());
			vAxis.Normalize();
			m.Forward(vAxis * m.Forward().Length()); // �g�嗦��K�p

			// �s��Z�b�g
			SHADER.m_effectShader.SetWorldMatrix(m);
		}
		else// �ʏ�
		{
			// �g��*��]*���W�ōs��쐬
			m.CreateScaling(node.Scale);
			m.RotateX(node.Rotate.x * KdToRadians);
			m.RotateY(node.Rotate.y * KdToRadians);
			m.RotateZ(node.Rotate.z * KdToRadians);
			m.Translation(node.Pos);
			// GameObject�̍s��ƍ���
			if (m_param.FollowTransform) m *= mW;

			// �s��Z�b�g
			SHADER.m_effectShader.SetWorldMatrix(m);
		}

		//------------------
		// �F
		//------------------
		// �F�`�F(�I��) ���Ԃ��ăZ�b�g
		KdVec4 color;
		color.Lerp(m_param.Color, m_param.EndColor, t);
		SHADER.m_effectShader.SetColor(color);

		//------------------
		// ��������
		//------------------
		SHADER.m_effectShader.WriteToCB();

		//------------------
		// �|���S���`��
		//------------------
		node.Polygon.Draw(0);
	}
	// �����������ɖ߂�
	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	SHADER.m_effectShader.SetColor({ 1,1,1,1 });
}
void ParticleComponent::Editor_ImGuiUpdate()
{
	// �p������ImGui���������s
	BaseComponent::Editor_ImGuiUpdate();
	ImGui::Indent(10);
	ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
	if (ImGui::TreeNodeEx(u8"����ݒ�", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
	{
		ImGui::DragInt(u8"�p������", &m_param.Duration, 1, 1, 100000);
		ImGuiShowHelp(u8"���̎��Ԃ��o�߂���ƍĐ��I���ƂȂ�");
		ImGui::DragInt(u8"�����Ԋu", &m_param.PopupInterval, 1, 1, 100000);
		ImGuiShowHelp(u8"���̊Ԋu���ƂɃp�[�e�B�N�X����������");
		if (m_param.PopupInterval <= 0)m_param.PopupInterval = 1;
		ImGui::Checkbox(u8"���[�v", &m_param.Loop);
		ImGuiShowHelp(u8"�Đ��I�������ꍇ�A���Ԃ�0�ɖ߂�");
		ImGui::DragInt(u8"������", &m_param.PopupNum, 1, 0, 10000);
		ImGuiShowHelp(u8"��x�ɔ��������鐔");
		// �Đ��I����
		std::vector<char*> stopActions = { u8"�Ȃɂ����Ȃ�", u8"GameObject�폜" };
		ImGui::Combo(u8"�Đ��I����", &m_param.StopAction, &stopActions[0], stopActions.size());
		ImGuiShowHelp(u8"�u�p�����ԁv�܂ōĐ����I�������̏���\n�����[�v��OFF�ɂ������̂�");
		//
		ImGui::DragInt(u8"�x������", &m_param.DelayTime, 1, -10000, 10000);
		ImGuiShowHelp(u8"�w�莞�Ԍォ��Đ����n�߂�");
		//
		ImGui::Checkbox(u8"Transform�ɒǏ]", &m_param.FollowTransform);
		ImGuiShowHelp(u8"GameObject��Transform�Ɋ��S�ɒǏ]����");
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx(u8"�p�[�e�B�N���m�[�h�ݒ�", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
	{
		ImGui::DragInt(u8"����", &m_param.LifeTime, 1, 0, 10000);
		// �r���{�[�h�^�C�v
		std::vector<char*> bbtypes = { u8"����", u8"�J�����ɍ��킹��", u8"Y���Œ�" };
		ImGui::Combo(u8"Billboard", &m_param.BillboardType, &bbtypes[0], bbtypes.size());

		// �u�����h�^�C�v
		std::vector<char*> blendtypes = { u8"����������", u8"���Z����" };
		ImGui::Combo(u8"BlendMode", &m_param.BlendMode, &blendtypes[0], blendtypes.size());

		// �F
		ImGui::ColorEdit4(u8"�F", &m_param.Color.x);
		ImGui::ColorEdit4(u8"�F(�I��)", &m_param.EndColor.x);
		ImGuiShowHelp(u8"�u�F�v�`�u�F(�I��)�v���Ԃ����炩�ɕω�����");

		// ���̑��t���O
		if (ImGui::TreeNodeEx(u8"�t���O", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
		{
			ImGui::CheckboxFlags(u8"�ړ�������Y���ɂ���", (UINT*)&m_param.Flags, kEtcFlag_MoveDirToYAxis);
			ImGuiShowHelp(u8"�ړ�������Y���ŕ`�悷��\n"
				u8"���r���{�[�h�^�C�v���uY���Œ�v�ɂ������̂ݗL��"
			);
			ImGui::TreePop();
		}

		// ��{�ݒ�
		if (ImGui::CollapsingHeader(u8"��{�ݒ�", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGuiShowHelp(u8"�u�F�v�`�u�F(�I��)�v���Ԃ����炩�ɕω�����");

			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"���W");
			ImGui::DragFloat3(u8"���W##PosStart", &m_param.PosStart.x, 0.01f);
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##PosRandom", &m_param.PosRandom.x, 0.01f);

			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"�p�x");
			ImGui::DragFloat3(u8"��]##AngleStart", &m_param.AngleStart.x, 0.1f);
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##AngleRandom", &m_param.AngleRandom.x, 0.1f);

			ImGui::TextColored(ImVec4(0, 0, 1, 1), u8"�傫��");
			ImGui::DragFloat3(u8"�g��##SizeStart", &m_param.SizeStart.x, 0.001f);
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##SizeRandom", &m_param.SizeRandom.x, 0.001f);
		}

		// ���x�ݒ�
		if (ImGui::CollapsingHeader(u8"���x�ݒ�", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"�ړ�");
			ImGui::DragFloat3(u8"�ړ�##MoveStart", &m_param.MoveStart.x, 0.01f);
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##MoveRandom", &m_param.MoveRandom.x, 0.01f);

			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"��]");
			ImGui::DragFloat3(u8"��]##RotateStart", &m_param.RotateStart.x, 0.01f, 0, 0, "%.4f");
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##RotateRandom", &m_param.RotateRandom.x, 0.01f, 0, 0, "%.4f");

			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"�g��");
			ImGui::DragFloat3(u8"�g��##ScaleStart", &m_param.ScaleStart.x, 0.0001f, 0, 0, "%.4f");
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##ScaleRandom", &m_param.ScaleRandom.x, 0.001f, 0, 0, "%.4f");
		}

		// �����x�ݒ�
		if (ImGui::CollapsingHeader(u8"�����x�ݒ�", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"�ړ�");
			ImGui::DragFloat3(u8"�ړ�##MoveAccelStart", &m_param.MoveAccelStart.x, 0.001f, 0, 0, "%.4f");
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##MoveAccelRandom", &m_param.MoveAccelRandom.x, 0.01f, 0, 0, "%.4f");
			ImGui::DragFloat3(u8"��Z##MoveAccelMultiply", &m_param.MoveAccelMultiply.x, 0.001f, 0, 0, "%.4f");

			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"��]");
			ImGui::DragFloat3(u8"��]##RotateAccelStart", &m_param.RotateAccelStart.x, 0.01f, 0, 0, "%.4f");
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##RotateAccelRandom", &m_param.RotateAccelRandom.x, 0.01f, 0, 0, "%.4f");

			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"�g��");
			ImGui::DragFloat3(u8"�g��##ScaleAccelAccel", &m_param.ScaleAccelStart.x, 0.0001f, 0, 0, "%.4f");
			ImGui::Text(u8"��"); ImGui::SameLine();
			ImGui::DragFloat3(u8"����##ScaleAccelRandom", &m_param.ScaleAccelRandom.x, 0.001f, 0, 0, "%.4f");
		}

		// �摜
		if (ImGui::CollapsingHeader(u8"�摜�ݒ�", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text(u8"�摜");
			if (ImGuiSelectTexture(m_param.Tex))
			{
				if (m_param.Tex == nullptr)
				{
					m_param.Tex = D3D.GetWhiteTex();
				}
			}
			// UV�A�j���[�V����
			ImGui::InputInt2(u8"UV�A�j��������", &m_param.UVAnim_SliceNum[0]);
			ImGuiShowHelp(u8"�c�E���̉摜������");
			ImGui::DragFloat(u8"UV�A�j��Speed", &m_param.UVAnim_Speed, 0.01f);
			ImGuiShowHelp(u8"�A�j���[�V�������x\n0�ōĐ���~");
			ImGui::DragFloat(u8"UV�A�j���J�n����", &m_param.UVAnim_StartTime, 1);
			ImGuiShowHelp(u8"�A�j���[�V�����̊J�n����");
			ImGui::Checkbox(u8"UV�A�j��Loop", &m_param.UVAnim_Loop);
			ImGuiShowHelp(u8"���[�v�Đ����邩�H");
		}
		ImGui::TreePop();
	}
	ImGui::PopItemWidth();
	ImGui::Unindent();

	//-----------------------------------
	//
	// Particle�m�F�p�E�B���h�E
	//
	//-----------------------------------
	ImGui::SetNextWindowPos(ImVec2(740, 600));
	if (ImGui::Begin("ParticleComponentDialog", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button(u8"���X�^�[�g"))
		{
			std::vector<KdSptr<ParticleComponent>> comps;
			GAMESYS.GetLevel()->GetRootObject()->GetComponents<ParticleComponent>(comps, true);;
			for (auto&& comp : comps)
			{
				comp->Restart();
			}
		}
		ImGuiShowHelp(u8"�S�Ă�GameObject��ParticleComponent��Restart������");
	}
	ImGui::End(); }