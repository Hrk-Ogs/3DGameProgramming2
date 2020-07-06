#include "Application/main.h"

#include "ActioinGame.h"

void CharacterController::Start()
{
	GAMESYS.m_editorData.LogWindow.AddLog(u8"CharacterComtoller開始！");

	//============================
	// 全行動ステートクラスを作成しておく
	//============================
	m_stateStorage["Stand"] = std::make_shared<State_Stand>();
	m_stateStorage["Run"] = std::make_shared<State_Run>();
	m_stateStorage["JumpStart"] = std::make_shared<State_JumpStart>();
	m_stateStorage["Jump"] = std::make_shared<State_Jump>();
	m_stateStorage["Landing"] = std::make_shared<State_Landing>();
	m_stateStorage["Attack"] = std::make_shared<State_Attack>();

	// 全行動クラスに持ち主キャラのアドレスを入れておく
	for (auto&& state : m_stateStorage) {
		// state.first;　→キーの名前が入っている
		state.second->m_chara = this;	// データ本体が入っている
	}

	// 初期の行動ステートは立ちにしておく
	m_nowState = m_stateStorage.at("Stand");

}

void CharacterController::Update()
{

	if (m_enable == false)return;
	// 停止時はスキップ
	if (GAMESYS.IsPlay() == false)return;

	// 入力コンポーネント取得
	auto input = GetOwner()->GetComponent<InputComponent>();
	if (input == nullptr)return;

	// スクリプトキーが来たときにこの関数を実行してもらう
	auto onScript = [](const KdKeyScript& script) {
		std::string type = script.JsonData["Type"].string_value();
		if (type == "Attack") {
			GAMESYS.m_editorData.LogWindow.AddLog("Attack!!");
		}
		else if (type == "AttackCancel") {
			GAMESYS.m_editorData.LogWindow.AddLog("Cansel!!");
		}

	};

	// スクリプト関数をModelComponentに渡す
	auto model = GetOwner()->GetComponent<ModelComponent>();
	if (model) {
		model->SetAnimationScriptProc(onScript);
	}

	//-------------------------------
	// 行動ステート処理実行
	//-------------------------------

	if (m_nowState) {
		m_nowState->Update();
	}
	
	//--------------------------------
	// 力による移動
	//--------------------------------
	KdVec3 pos = GetOwner()->Transform()->GetPosition();	// 座標取得
	pos += m_vForce;		// 座標に力を加算
	GetOwner()->Transform()->SetPosition(pos);		// 座標をセット
	
	//--------------------------------
	// 地面の設置を検知する
	//--------------------------------
	{
		// (仮）設置相手をリセット
		m_wpGround.reset();

		// レイコライダーを取得
		auto ray = GetOwner()->GetComponent<RayColliderComponent>();
		if (ray) {
			// ヒット時に実行される関数
			ray->m_onHitStay = [this](ColliderComponent* collider) {	// ラムダ式
				// レイがヒットした

				// 最も近いやつを探し
				float closestDist = FLT_MAX;
				KdSptr<GameObject> closestObj;
				// ヒットしたもの全て
				for (auto&& res : collider->GetResults()) {
					// より近い距離でひっとしてる
					if (res.Dist < closestDist) {
						// 情報を記憶
						closestDist = res.Dist;
						closestObj = res.Collider->GetOwner();
					}

				}

				// 下移動している時のみ
				if (m_vForce.y <= 0) {
					// 接地キャラを把握
					m_wpGround = closestObj;
				}

				// Y方向の力をリセット
				m_vForce.y = 0;
			};
		}
	}
	
	
	//--------------------------------
	// 視点回転
	//--------------------------------
	{
		const float kMaxAngle = 2;	// カメラの回転速度
		
		// 子GameObjectの”Camera”を回転させる
		auto camObj = GetOwner()->Find("Camera");
		if (camObj) {
			// CharacterCamerControllerを取得
			auto compCtrl = camObj->GetComponent<CharacteCameraController>();
			if (compCtrl) {
				// Y軸回転
				compCtrl->CamAngles().y += input->GetAxis(GameAxes::R).x * kMaxAngle;
			}
		}
	}

}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseComponent::Editor_ImGuiUpdate();
	// Hp設定
	ImGui::InputInt("Hp", &m_hp);

}

void CharacterController::State_Stand::Update()
{
		// 入力コンポーネント取得
	auto input = m_chara->GetOwner()->GetComponent<InputComponent>();

	// 方向キーを押すと、「走り」に変更
	if (input->GetAxis(GameAxes::L).Length() > 0.1f) {
		// 「走り」行動へ切り替える
		m_chara->m_nowState = m_chara->m_stateStorage.at("Run");
		
		// 「走り」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// 走りアニメーションデータを取得
		auto anim = model->GetModel()->GetAnimation("Run");
		// アニメーションへ切り替え
		model->Animator().CrossFade(anim, 0.2f, true);
	}

	// ジャンプ
	if (input->IsButtonPressed(GameButtons::B)) {
		// 「ジャンプ」行動へ切り替える
		auto state = std::dynamic_pointer_cast<State_JumpStart>(m_chara->m_stateStorage.at("JumpStart"));
		state->m_force = { 0,0.2f,0 };	// 上方向
		m_chara->m_nowState = state;

		// 「ジャンプ」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// ジャンプアニメーションデータを取得
		auto anim = model->GetModel()->GetAnimation("JumpStart");
		// アニメーションへ切り替え
		model->Animator().CrossFade(anim, 1.0f, false);
	}

	// いま空中か
	if (m_chara->m_wpGround.expired() == true) {
		// 行動ステート変更
		m_chara->m_nowState = m_chara->m_stateStorage.at("Jump");

		// アニメーション変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Jump");
		model->Animator().CrossFade(anim, 0.1f, false);
	}

	// 攻撃
	if (input->IsButtonPressed(GameButtons::A)) {
		// 「攻撃」行動へ切り替える
		m_chara->m_nowState = m_chara->m_stateStorage.at("Attack");

		// 「攻撃」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// 攻撃アニメーションデータを取得
		auto anim = model->GetModel()->GetAnimation("Attack_Slash1_Root");
		// アニメーションへ切り替え
		model->Animator().CrossFade(anim, 0.5f, false);
	}

	// 重力
	m_chara->CalcGravity();
	// 摩擦
	m_chara->CalcFriction();


}

void CharacterController::State_Run::Update()
{
	// 入力コンポーネント取得
	auto input = m_chara->GetOwner()->GetComponent<InputComponent>();

	// 方向キーを話すと、「立ち」に変更
	if (input->GetAxis(GameAxes::L).Length() <= 0.1f) {
		// 「立ち」行動へ切り替える
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// 「立ち」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// 立ちアニメーションの取得
		auto anim = model->GetModel()->GetAnimation("Stand");
		// そのアニメーションへの切り替え
		model->Animator().CrossFade(anim, 0.2f, true);
	}



	//--------------------------
	// カメラベース移動
	//--------------------------
	auto camObj = m_chara->GetOwner()->Find("Camera");

	KdVec3 vDir;
	if (camObj) {
		// カメラGameObjectの行列(ワールド行列）
		KdMatrix mCamWorld = camObj->Transform()->GetMatrix();
		// 方向キーの情報から、移動方向を決定する
		vDir += mCamWorld.Forward() * input->GetAxis(GameAxes::L).y;
		vDir += mCamWorld.Right() * input->GetAxis(GameAxes::L).x;
		vDir.y = 0;
		vDir.Normalize();

		if (vDir.Length() > 0) {
			// キャラのTransformを取得
			auto charaTrans = m_chara->GetOwner()->Transform();
			
			//-------------
			// 回転
			//-------------
			KdVec3 vCharaZ = charaTrans->GetRotation().Forward();
			// Yを潰し、XZ平面に平行にする。
			vCharaZ.y = 0;
			vCharaZ.Normalize();

			// キャラの向きベクトルを、指定方向に指定角度だけ向ける
			constexpr float kMaxAngle = 15;	// 最大回転角度
			vCharaZ.LookTo(vDir, kMaxAngle * KdToRadians);

			// 自分の回転（クオータニオン）取得
			KdQuaternion rota = charaTrans->GetRotation();
			// 回転を指定方向へ向け、セットする
			rota.LookTo(vCharaZ, { 0,1,0 });
			charaTrans->SetRotation(rota);
			
			//-------------
			// 移動
			//-------------
			const float kSpeed = 0.05f;	// 移動速度
			
			KdVec3 pos = charaTrans->GetPosition();
			pos += vDir * kSpeed;
			charaTrans->SetPosition(pos);
		}
	}

	// ジャンプ
	if (input->IsButtonPressed(GameButtons::B)) {
		// 「ジャンプ」行動へ切り替える
		auto state = std::dynamic_pointer_cast<State_JumpStart>(m_chara->m_stateStorage.at("JumpStart"));
		state->m_force = { 0,0.2f,0 };	// 上方向
		state->m_force += vDir * 0.1f;
		m_chara->m_nowState = state;

		// 「ジャンプ」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// ジャンプアニメーションデータを取得
		auto anim = model->GetModel()->GetAnimation("JumpStart");
		// アニメーションへ切り替え
		model->Animator().CrossFade(anim, 1.0f, false);
	}

	// いま空中か
	if (m_chara->m_wpGround.expired() == true) {
		// 行動ステート変更
		m_chara->m_nowState = m_chara->m_stateStorage.at("Jump");

		// アニメーション変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Jump");
		model->Animator().CrossFade(anim, 0.1f, false);
	}

	// 攻撃
	if (input->IsButtonPressed(GameButtons::A)) {
		// 「攻撃」行動へ切り替える
		m_chara->m_nowState = m_chara->m_stateStorage.at("Attack");

		// 「攻撃」アニメーションへ変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		// 攻撃アニメーションデータを取得
		auto anim = model->GetModel()->GetAnimation("Attack_Slash1_Root");
		// アニメーションへ切り替え
		model->Animator().CrossFade(anim, 0.5f, false);
	}

	// 重力
	m_chara->CalcGravity();
	// 摩擦
	m_chara->CalcFriction();
}


void CharacterController::State_JumpStart::Update()
{
	// 
	GAMESYS.m_editorData.LogWindow.AddLog("JumpStart!!");

	// ModelComponent取得
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// アニメ終了した
	if (model->Animator().IsAnimationEnd()) {
		// 行動ステート変更
		m_chara->m_nowState = m_chara->m_stateStorage.at("Jump");

		// アニメーション変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Jump");
		model->Animator().CrossFade(anim, 0.05f, true);

		// 力を入れる
		m_chara->m_vForce = m_force;
	}
	// 重力
	m_chara->CalcGravity();
	// 摩擦処理
	m_chara->CalcFriction();
}

void CharacterController::State_Jump::Update()
{
	// 立ち状態の時のみの処理を書く
	GAMESYS.m_editorData.LogWindow.AddLog("Jump!!");

	// ModelComponent取得
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// 今地面か
	if (m_chara->m_wpGround.expired() == false) {
		// 行動ステートを変更
		m_chara->m_nowState = m_chara->m_stateStorage.at("Landing");

		// アニメーション変更
		auto anim = model->GetModel()->GetAnimation("Landing");
		model->Animator().CrossFade(anim, 1.0f, false);
	}

	// 重力
	m_chara->CalcGravity();
	// 摩擦
	m_chara->CalcFriction();
}

void CharacterController::State_Landing::Update()
{
	// 立ち状態の時のみの処理を書く
	GAMESYS.m_editorData.LogWindow.AddLog("Landing!!");

	// ModelComponent取得
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// アニメ終了
	if (model->Animator().IsAnimationEnd()) {
		// 行動ステートを変更
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// アニメーション変更
		auto anim = model->GetModel()->GetAnimation("Stand");
		model->Animator().CrossFade(anim, 0.5f, true);
	}

	// 重力
	m_chara->CalcGravity();
	// 摩擦
	m_chara->CalcFriction();
}

void CharacterController::State_Attack::Update()
{
	// ModelComponent取得
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// アニメ終了
	if (model->Animator().IsAnimationEnd()) {
		// 行動ステートを変更
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// アニメーション変更
		auto anim = model->GetModel()->GetAnimation("Stand");
		model->Animator().CrossFade(anim, 0.5f, true);
	}

	// 重力
	m_chara->CalcGravity();
	// 摩擦
	m_chara->CalcFriction();

}



void CharacteCameraController::Update()
{

	if (m_enable == false)return;
	//if (GAMESYS.IsPlay() == false)return;	// エディター時でも動作させる

	// 親のGameObject
	auto parent = GetOwner()->GetParent();
	if (parent) {
		// 親の変換行列
		auto& parentTransform = parent->Transform();

		// 親の行列から、カメラの行列を決定する
		KdMatrix camMat;
		// オフセット座標分移動
		camMat.Move(m_camOffset);

		// 回転
		camMat.RotateX(m_camAngles.x * KdToRadians);
		camMat.RotateY(m_camAngles.y * KdToRadians);

		// 親の「座標のみ」を加算
		camMat.Move(parentTransform->GetMatrix().Translation());

		// セット
		GetOwner()->Transform()->SetMatrix(camMat);
	}

}

