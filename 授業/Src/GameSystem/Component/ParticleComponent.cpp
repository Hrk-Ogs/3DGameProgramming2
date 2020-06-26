#include "../GameSystem.h"
#include "ParticleComponent.h"

void ParticleComponent::Update()
{
	// 無効時はスキップ
	if (m_enable == false)return;

	// ワールド行列
	auto& mW = GetOwner()->Transform()->GetMatrix();
	//------------------------------
	// 再生終了時
	//------------------------------
	if (m_time >= m_param.Duration)
	{
		// ループ再生？
		if (m_param.Loop)
		{
			m_time = -m_param.DelayTime;
		}
		else
		{
			// 終了時処理は再生時のみ実行
			if (GAMESYS.IsPlay())
			{
				// 終了時の処理
				if (m_param.StopAction == kStopAction_Destroy) // GameObject削除
				{
					GetOwner()->Destroy(); // GameObjectを削除
					return;
				}
			}
		}
	}

	//------------------------------
	// パーティクルノードの生成
	//------------------------------
	if (m_time >= 0 && m_time < m_param.Duration)
	{
		// 「発生間隔」の判定
		if (m_time % m_param.PopupInterval == 0)
		{
			// 「発生数」ぶんノードを作成
			for (int i = 0; i < m_param.PopupNum; i++)
			{
				//-----------------
				// 追加
				//-----------------
				m_nodes.emplace_back();
				auto& node = m_nodes.back();
				// 板ポリゴン設定
				node.Polygon.Init(1, 1, { 1,1,1,1 });
				node.Polygon.SetTexture(m_param.Tex);
				node.Polygon.SetAnimationInfo(m_param.UVAnim_SliceNum[0], m_param.UVAnim_SliceNum[1]);// UVアニメ分割数
				node.Polygon.SetAnimationPos(m_param.UVAnim_StartTime); // UVアニメ位置
				// 座標 設定
				node.Pos = m_param.PosStart;
				node.Pos.x += KdMTRand::s_rnd.GetFloat(-m_param.PosRandom.x, m_param.PosRandom.x);
				node.Pos.y += KdMTRand::s_rnd.GetFloat(-m_param.PosRandom.y, m_param.PosRandom.y);
				node.Pos.z += KdMTRand::s_rnd.GetFloat(-m_param.PosRandom.z, m_param.PosRandom.z);
				// 拡大 設定
				node.Scale = m_param.SizeStart;
				node.Scale.x += KdMTRand::s_rnd.GetFloat(-m_param.SizeRandom.x, m_param.SizeRandom.x);
				node.Scale.y += KdMTRand::s_rnd.GetFloat(-m_param.SizeRandom.y, m_param.SizeRandom.y);
				node.Scale.z += KdMTRand::s_rnd.GetFloat(-m_param.SizeRandom.z, m_param.SizeRandom.z);
				// 回転 設定
				node.Rotate = m_param.AngleStart;
				node.Rotate.x += KdMTRand::s_rnd.GetFloat(-m_param.AngleRandom.x, m_param.AngleRandom.x);
				node.Rotate.y += KdMTRand::s_rnd.GetFloat(-m_param.AngleRandom.y, m_param.AngleRandom.y);
				node.Rotate.z += KdMTRand::s_rnd.GetFloat(-m_param.AngleRandom.z, m_param.AngleRandom.z);
				// (速度)移動 設定
				node.MoveSpeed = m_param.MoveStart;
				node.MoveSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.MoveRandom.x, m_param.MoveRandom.x);
				node.MoveSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.MoveRandom.y, m_param.MoveRandom.y);
				node.MoveSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.MoveRandom.z, m_param.MoveRandom.z);
				// (速度)移動 設定
				node.RotateSpeed = m_param.RotateStart;
				node.RotateSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.RotateRandom.x, m_param.RotateRandom.x);
				node.RotateSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.RotateRandom.y, m_param.RotateRandom.y);
				node.RotateSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.RotateRandom.z, m_param.RotateRandom.z);
				// (速度)拡大 設定
				node.ScaleSpeed = m_param.ScaleStart;
				node.ScaleSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.ScaleRandom.x, m_param.ScaleRandom.x);
				node.ScaleSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.ScaleRandom.y, m_param.ScaleRandom.y);
				node.ScaleSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.ScaleRandom.z, m_param.ScaleRandom.z);
				// (加速度)移動 設定
				node.MoveAccelSpeed = m_param.MoveAccelStart;
				node.MoveAccelSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.MoveAccelRandom.x, m_param.MoveAccelRandom.x);
				node.MoveAccelSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.MoveAccelRandom.y, m_param.MoveAccelRandom.y);
				node.MoveAccelSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.MoveAccelRandom.z, m_param.MoveAccelRandom.z);
				// (加速度)移動 設定
				node.RotateAccelSpeed = m_param.RotateAccelStart;
				node.RotateAccelSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.RotateAccelRandom.x, m_param.RotateAccelRandom.x);
				node.RotateAccelSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.RotateAccelRandom.y, m_param.RotateAccelRandom.y);
				node.RotateAccelSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.RotateAccelRandom.z, m_param.RotateAccelRandom.z);
				// (加速度)拡大 設定
				node.ScaleAccelSpeed = m_param.ScaleAccelStart;
				node.ScaleAccelSpeed.x += KdMTRand::s_rnd.GetFloat(-m_param.ScaleAccelRandom.x, m_param.ScaleAccelRandom.x);
				node.ScaleAccelSpeed.y += KdMTRand::s_rnd.GetFloat(-m_param.ScaleAccelRandom.y, m_param.ScaleAccelRandom.y);
				node.ScaleAccelSpeed.z += KdMTRand::s_rnd.GetFloat(-m_param.ScaleAccelRandom.z, m_param.ScaleAccelRandom.z);
				// Transformに影響しない場合は、初回だけ変換する
				if (m_param.FollowTransform == false)
				{
					// 座標を変換
					node.Pos.TransformCoord(mW);
					// 移動ベクトル系を変換
					node.MoveSpeed.TransformNormal(mW);
					node.MoveAccelSpeed.TransformNormal(mW);
					// 回転
					node.Rotate += mW.ComputeAngles() * KdToDegrees;
					// 拡大
					KdVec3 s = mW.GetScale();
					node.Scale *= s;
					node.ScaleAccelSpeed *= s;
				}
			}
		}
		// 時間経過
		m_time++;
	}

	//------------------------------
	// 全パーティクスノードの処理
	//------------------------------
	for (auto it = m_nodes.begin(); it != m_nodes.end();)
	{
		auto& node = (*it);
		// 座標
		node.MoveSpeed += node.MoveAccelSpeed; // 移動速度 加速
		node.Pos += node.MoveSpeed; // 座標移動
		node.MoveSpeed *= m_param.MoveAccelMultiply;	// 移動速度乗算

		// 回転
		node.RotateSpeed += node.RotateAccelSpeed; // 角速度 加速
		node.Rotate += node.RotateSpeed; // 回転

		// 拡大
		node.ScaleSpeed += node.ScaleAccelSpeed; // 拡大速度 加速
		node.Scale += node.ScaleSpeed; // 拡大

		// UVアニメーション
		node.Polygon.AdvanceTime(m_param.UVAnim_Speed, m_param.UVAnim_Loop);

		// 寿命
		node.Time++;
		if (node.Time >= m_param.LifeTime)
		{
			// 時間が寿命を超えたら削除する
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

	// 描画登録
	rdata.m_drawEffectList.push_back(this);
}

void ParticleComponent::Draw(int phaseID)
{
	if (m_enable == false)return;

	// GameObjectの行列
	auto& mW = GetOwner()->Transform()->GetMatrix();

	// シェーダーセット
	SHADER.m_effectShader.SetToDevice();

	// カメラ行列を算出
	KdMatrix mCam = SHADER.m_cb7_Camera.GetWork().mV;
	mCam.Inverse();

	// ブレンドモード変更
	if (m_param.BlendMode == kBlendType_Alpha) // 半透明合成
	{
		D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	}
	else if (m_param.BlendMode == kBlendType_Add) // 加算合成
	{
		D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Add, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	}

	//------------------------------
	// 全パーティクスノードを描画
	//------------------------------
	KdMatrix m;
	for (auto&& node : m_nodes)
	{
		// 寿命から時間比率を求める(0～1)
		float t = (float)node.Time / (float)m_param.LifeTime;

		//------------------
		// 行列計算
		// ・主にビルボーディング計算
		//------------------
		// カメラに合わせる
		if (m_param.BillboardType == kBBType_Billboard)
		{
			// 回転以外の情報で行列を作成
			m.CreateScaling(node.Scale);
			m.Translation(node.Pos);
			if (m_param.FollowTransform) m *= mW;
			// 行列の拡大率を取得
			KdVec3 scale = m.GetScale();
			// カメラの回転に合わせる(ここで拡大成分は１になってしまう)
			m.Right(mCam.Right());
			m.Up(mCam.Up());
			m.Forward(mCam.Forward());
			// Z回転みの使用
			m = KdMatrix().CreateRotationZ(node.Rotate.z * KdToRadians) * m;
			// 取得しておいた拡大を適用
			m = KdMatrix().CreateScaling(scale) * m;

			// 行列セット
			SHADER.m_effectShader.SetWorldMatrix(m);
		}
		// Y軸のみ固定で、カメラの方向に向くようにXZ軸を計算
		else if (m_param.BillboardType == kBBType_FixedYAxis) // Y軸固定
		{
			//--------------
			// 行列作成
			//--------------
			m.CreateScaling(node.Scale);
			// 移動ベクトルをY軸として使用する
			if (m_param.Flags & kEtcFlag_MoveDirToYAxis)
			{
				// 移動ベクトルを正規化
				auto vY = node.MoveSpeed;
				vY.Normalize();
				// 現在のY軸の拡大率にする
				vY *= m.Up().Length();
				// Y軸セット
				m.Up(vY);
			}
			// 通常
			else
			{
				m.RotateX(node.Rotate.x * KdToRadians);
				m.RotateY(node.Rotate.y * KdToRadians);
				m.RotateZ(node.Rotate.z * KdToRadians);
			}
			// 移動
			m.Translation(node.Pos);
			// GameObjectの行列と合成
			if (m_param.FollowTransform) m *= mW;

			//--------------
			// X軸とZ軸をカメラに対して
			// 垂直になるように計算で求める。
			//--------------
			// カメラから、パーティクスノードへの方向
			KdVec3 v = m.Translation() - mCam.Translation();
			// X軸を求める
			KdVec3 vAxis = KdVec3::sCross(m.Up(), v);
			vAxis.Normalize();
			m.Right(vAxis * m.Right().Length()); // 拡大率を適用
			// Z軸を求める
			vAxis = KdVec3::sCross(m.Right(), m.Up());
			vAxis.Normalize();
			m.Forward(vAxis * m.Forward().Length()); // 拡大率を適用

			// 行列セット
			SHADER.m_effectShader.SetWorldMatrix(m);
		}
		else// 通常
		{
			// 拡大*回転*座標で行列作成
			m.CreateScaling(node.Scale);
			m.RotateX(node.Rotate.x * KdToRadians);
			m.RotateY(node.Rotate.y * KdToRadians);
			m.RotateZ(node.Rotate.z * KdToRadians);
			m.Translation(node.Pos);
			// GameObjectの行列と合成
			if (m_param.FollowTransform) m *= mW;

			// 行列セット
			SHADER.m_effectShader.SetWorldMatrix(m);
		}

		//------------------
		// 色
		//------------------
		// 色～色(終了) を補間してセット
		KdVec4 color;
		color.Lerp(m_param.Color, m_param.EndColor, t);
		SHADER.m_effectShader.SetColor(color);

		//------------------
		// 書き込み
		//------------------
		SHADER.m_effectShader.WriteToCB();

		//------------------
		// ポリゴン描画
		//------------------
		node.Polygon.Draw(0);
	}
	// 半透明合成に戻す
	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
	SHADER.m_effectShader.SetColor({ 1,1,1,1 });
}
void ParticleComponent::Editor_ImGuiUpdate()
{
	// 継承元のImGui処理を実行
	BaseComponent::Editor_ImGuiUpdate();
	ImGui::Indent(10);
	ImGui::PushItemWidth(ImGui::GetWindowSize().x * 0.5f);
	if (ImGui::TreeNodeEx(u8"動作設定", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
	{
		ImGui::DragInt(u8"継続時間", &m_param.Duration, 1, 1, 100000);
		ImGuiShowHelp(u8"この時間が経過すると再生終了となる");
		ImGui::DragInt(u8"発生間隔", &m_param.PopupInterval, 1, 1, 100000);
		ImGuiShowHelp(u8"この間隔ごとにパーティクスが発生する");
		if (m_param.PopupInterval <= 0)m_param.PopupInterval = 1;
		ImGui::Checkbox(u8"ループ", &m_param.Loop);
		ImGuiShowHelp(u8"再生終了した場合、時間が0に戻す");
		ImGui::DragInt(u8"発生数", &m_param.PopupNum, 1, 0, 10000);
		ImGuiShowHelp(u8"一度に発生させる数");
		// 再生終了時
		std::vector<char*> stopActions = { u8"なにもしない", u8"GameObject削除" };
		ImGui::Combo(u8"再生終了時", &m_param.StopAction, &stopActions[0], stopActions.size());
		ImGuiShowHelp(u8"「継続時間」まで再生し終えた時の処理\n※ループをOFFにした時のみ");
		//
		ImGui::DragInt(u8"遅延時間", &m_param.DelayTime, 1, -10000, 10000);
		ImGuiShowHelp(u8"指定時間後から再生を始める");
		//
		ImGui::Checkbox(u8"Transformに追従", &m_param.FollowTransform);
		ImGuiShowHelp(u8"GameObjectのTransformに完全に追従する");
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx(u8"パーティクルノード設定", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
	{
		ImGui::DragInt(u8"寿命", &m_param.LifeTime, 1, 0, 10000);
		// ビルボードタイプ
		std::vector<char*> bbtypes = { u8"無し", u8"カメラに合わせる", u8"Y軸固定" };
		ImGui::Combo(u8"Billboard", &m_param.BillboardType, &bbtypes[0], bbtypes.size());

		// ブレンドタイプ
		std::vector<char*> blendtypes = { u8"半透明合成", u8"加算合成" };
		ImGui::Combo(u8"BlendMode", &m_param.BlendMode, &blendtypes[0], blendtypes.size());

		// 色
		ImGui::ColorEdit4(u8"色", &m_param.Color.x);
		ImGui::ColorEdit4(u8"色(終了)", &m_param.EndColor.x);
		ImGuiShowHelp(u8"「色」～「色(終了)」を間を滑らかに変化する");

		// その他フラグ
		if (ImGui::TreeNodeEx(u8"フラグ", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected))
		{
			ImGui::CheckboxFlags(u8"移動方向をY軸にする", (UINT*)&m_param.Flags, kEtcFlag_MoveDirToYAxis);
			ImGuiShowHelp(u8"移動方向をY軸で描画する\n"
				u8"※ビルボードタイプを「Y軸固定」にした時のみ有効"
			);
			ImGui::TreePop();
		}

		// 基本設定
		if (ImGui::CollapsingHeader(u8"基本設定", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGuiShowHelp(u8"「色」～「色(終了)」を間を滑らかに変化する");

			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"座標");
			ImGui::DragFloat3(u8"座標##PosStart", &m_param.PosStart.x, 0.01f);
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##PosRandom", &m_param.PosRandom.x, 0.01f);

			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"角度");
			ImGui::DragFloat3(u8"回転##AngleStart", &m_param.AngleStart.x, 0.1f);
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##AngleRandom", &m_param.AngleRandom.x, 0.1f);

			ImGui::TextColored(ImVec4(0, 0, 1, 1), u8"大きさ");
			ImGui::DragFloat3(u8"拡大##SizeStart", &m_param.SizeStart.x, 0.001f);
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##SizeRandom", &m_param.SizeRandom.x, 0.001f);
		}

		// 速度設定
		if (ImGui::CollapsingHeader(u8"速度設定", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"移動");
			ImGui::DragFloat3(u8"移動##MoveStart", &m_param.MoveStart.x, 0.01f);
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##MoveRandom", &m_param.MoveRandom.x, 0.01f);

			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"回転");
			ImGui::DragFloat3(u8"回転##RotateStart", &m_param.RotateStart.x, 0.01f, 0, 0, "%.4f");
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##RotateRandom", &m_param.RotateRandom.x, 0.01f, 0, 0, "%.4f");

			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"拡大");
			ImGui::DragFloat3(u8"拡大##ScaleStart", &m_param.ScaleStart.x, 0.0001f, 0, 0, "%.4f");
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##ScaleRandom", &m_param.ScaleRandom.x, 0.001f, 0, 0, "%.4f");
		}

		// 加速度設定
		if (ImGui::CollapsingHeader(u8"加速度設定", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"移動");
			ImGui::DragFloat3(u8"移動##MoveAccelStart", &m_param.MoveAccelStart.x, 0.001f, 0, 0, "%.4f");
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##MoveAccelRandom", &m_param.MoveAccelRandom.x, 0.01f, 0, 0, "%.4f");
			ImGui::DragFloat3(u8"乗算##MoveAccelMultiply", &m_param.MoveAccelMultiply.x, 0.001f, 0, 0, "%.4f");

			ImGui::TextColored(ImVec4(0, 1, 0, 1), u8"回転");
			ImGui::DragFloat3(u8"回転##RotateAccelStart", &m_param.RotateAccelStart.x, 0.01f, 0, 0, "%.4f");
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##RotateAccelRandom", &m_param.RotateAccelRandom.x, 0.01f, 0, 0, "%.4f");

			ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"拡大");
			ImGui::DragFloat3(u8"拡大##ScaleAccelAccel", &m_param.ScaleAccelStart.x, 0.0001f, 0, 0, "%.4f");
			ImGui::Text(u8"乱"); ImGui::SameLine();
			ImGui::DragFloat3(u8"乱数##ScaleAccelRandom", &m_param.ScaleAccelRandom.x, 0.001f, 0, 0, "%.4f");
		}

		// 画像
		if (ImGui::CollapsingHeader(u8"画像設定", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text(u8"画像");
			if (ImGuiSelectTexture(m_param.Tex))
			{
				if (m_param.Tex == nullptr)
				{
					m_param.Tex = D3D.GetWhiteTex();
				}
			}
			// UVアニメーション
			ImGui::InputInt2(u8"UVアニメ分割数", &m_param.UVAnim_SliceNum[0]);
			ImGuiShowHelp(u8"縦・横の画像分割数");
			ImGui::DragFloat(u8"UVアニメSpeed", &m_param.UVAnim_Speed, 0.01f);
			ImGuiShowHelp(u8"アニメーション速度\n0で再生停止");
			ImGui::DragFloat(u8"UVアニメ開始時間", &m_param.UVAnim_StartTime, 1);
			ImGuiShowHelp(u8"アニメーションの開始時間");
			ImGui::Checkbox(u8"UVアニメLoop", &m_param.UVAnim_Loop);
			ImGuiShowHelp(u8"ループ再生するか？");
		}
		ImGui::TreePop();
	}
	ImGui::PopItemWidth();
	ImGui::Unindent();

	//-----------------------------------
	//
	// Particle確認用ウィンドウ
	//
	//-----------------------------------
	ImGui::SetNextWindowPos(ImVec2(740, 600));
	if (ImGui::Begin("ParticleComponentDialog", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button(u8"リスタート"))
		{
			std::vector<KdSptr<ParticleComponent>> comps;
			GAMESYS.GetLevel()->GetRootObject()->GetComponents<ParticleComponent>(comps, true);;
			for (auto&& comp : comps)
			{
				comp->Restart();
			}
		}
		ImGuiShowHelp(u8"全てのGameObjectのParticleComponentをRestartさせる");
	}
	ImGui::End(); }