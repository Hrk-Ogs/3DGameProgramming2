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
	m_stateStorage["Stagger"] = std::make_shared<State_Stagger>();
	m_stateStorage["Die"] = std::make_shared<State_Die>();

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


	//------------------------
	// アニメーションスクリプト関数
	//------------------------
	// スクリプトキーが来たときにこの関数を実行してもらう
	auto onScript = [this](const KdKeyScript& script) {
		std::string type = script.JsonData["Type"].string_value();

		if (type == "Attack") {
			// 右手の武器装備ジョイントGameObjectを検索
			auto joint = GetOwner()->Find("WeaponJointR");
			if (joint == nullptr)return;
			// そこから武器データGameObjectを検索
			auto weaponObj = joint->Find("WeaponData");
			if (weaponObj == nullptr)return;

			// そこからWeaponScriptを取得
			auto wpnScript = weaponObj->GetComponent<WeaponScript>();
			if (wpnScript == nullptr)return;

			//-------------------------
			// 武器の攻撃パラメータ設定
			//-------------------------
			// 一度無視リストを全てクリアする
			wpnScript->ClearIgnoreList();
			// 有効時間設定
			wpnScript->SetEnableTime(script.JsonData["Duration"].int_value());
			// ヒット間隔
			wpnScript->SetHitInterval(script.JsonData["HitInterval"].int_value());
			// 装備者をセット
			auto chara = std::static_pointer_cast<CharacterController>(shared_from_this());
			wpnScript->SetChara(chara);
			// ヒットストップ値セット
			wpnScript->SetHitStopValue(script.JsonData["HitStop"].int_value());

			// ヒット威力
			float powerRate = (float)script.JsonData["PowerRate"].number_value();
			// （キャラの攻撃力　*　武器の威力）* 倍率
			int hitPower = (10 + wpnScript->GetParameter().Power) * powerRate;
			wpnScript->SetHitPower(hitPower);

		}
		else if (type == "AttackCancel") {
			auto input = GetOwner()->GetComponent<InputComponent>();

			// 攻撃ボタンおしているか
			if (input->IsButtonPressed(GameButtons::A)) {
				// 変更先アニメ名
				std::string nextAnimeName = script.JsonData["NextAnimeName"].string_value();
				// アニメーション変更
				auto model = GetOwner()->GetComponent<ModelComponent>();
				auto anim = model->GetModel()->GetAnimation(nextAnimeName);
				model->Animator().CrossFade(anim, 1.0f, false);
			}

		}
		else if (type == "PopPrefab") {
			std::string filename;
			KdJsonGet(script.JsonData["PrefabFilename"], filename);

			// Prefab生成
			auto newObj = GAMESYS.Instantiate(filename);
			// 座標をセットする
			auto myPos = GetOwner()->Transform()->GetPosition();
			newObj->Transform()->SetPosition(myPos);

		}
		// カメラ演出モードへ
		else if (type == "CameraEffect")
		{
			// カメラとなるボーン名を取得
			std::string boneName = script.JsonData["BoneName"].string_value();
			// 持続時間
			m_cameraEffectTime = script.JsonData["Duration"].int_value();

			// 子カメラを取得
			auto camObj = GetOwner()->Find("Camera");
			if (camObj)
			{
				// カメラコントロールは一時停止
				auto camCtrl = camObj->GetComponent<CharacteCameraController>();
				camCtrl->SetEnable(false);

				// TransformComponentに、ボーン追従指定をする
				camObj->Transform()->SetFollowBoneName(boneName);
			}
		}

	};

	// スクリプト関数をModelComponentに渡す
	auto model = GetOwner()->GetComponent<ModelComponent>();
	if (model) {
		model->SetAnimationScriptProc(onScript);
	}

	// ヒットストップ中でない場合
	if (m_hitStopCnt <= 0) {

		// アニメーターを有効
		model->Animator().NowPlayTrack().Enable = true;

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
		//--------------------------
		// カメラ演出中
		//--------------------------
		if (m_cameraEffectTime > 0)
		{
			// 時間を減らす
			m_cameraEffectTime--;
			// 時間が来たら、カメラの設定を元に戻す
			if (m_cameraEffectTime == 0)
			{
				auto camObj = GetOwner()->Find("Camera");
				if (camObj)
				{
					// 追従解除
					camObj->Transform()->SetFollowBoneName("");

					// カメラコントローラーを復活
					auto camCtrl = camObj->GetComponent<CharacteCameraController>();
					camCtrl->SetEnable(true);
				}
			}
		}

	}
	// ヒットストップ中の時
	else {
		// アニメーターを止める
		model->Animator().NowPlayTrack().Enable = false;

		// ヒットストップカウンターを減らす
		m_hitStopCnt--;


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

void CharacterController::LateUpdate()
{
	// 無効時はスキップ
	if (m_enable == false)return;
	// 停止時はスキップ
	if (GAMESYS.IsPlay() == false)return;

	//--------------------
	// 地面との連動
	//--------------------
	auto ground = m_wpGround.lock();
	if (ground) {
		// 下にいる奴の変化量をもらう
		KdMatrix mDelta;
		ground->Transform()->GetDeltaMatrix(mDelta);
		mDelta.NormalizeScale();

		// 自分にこの変化量を適用する
		auto m = GetOwner()->Transform()->GetMatrix() * mDelta;

		// キャラの行列のZ軸を平行にする
		KdVec3 vZ = m.Forward();	// 行列のZ軸ベクトル取得
		vZ.y = 0;					// Z軸ベクトルのY成分をつぶす
		m.LookTo(vZ, { 0,1,0 });	// 行列をこのZ方向に向ける

		// セット
		GetOwner()->Transform()->SetMatrix(m);
	}

	// 設置相手をリセット
	m_wpGround.reset();
}


bool CharacterController::OnDamage(const DamageArg& dmg, DamageReply& rep)
{

	// HPを減らす
	m_hp -= dmg.AtkPower;

	// ヒットストップ
	m_hitStopCnt = dmg.HitStop;

	// 生きてる
	if (m_hp > 0) {
		// よろけへ
		m_nowState = m_stateStorage.at("Stagger");

		// アニメーション変更
		auto model = GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("KnockBack_Front_Root");
		model->Animator().CrossFade(anim, 1.0f, false);
	}
	// HPが0を下回った
	else {
		m_hp = 0;

		// 死亡行動へ切り替え
		auto state = std::dynamic_pointer_cast<State_Die>(m_stateStorage.at("Die"));
		state->Reset();
		m_nowState = state;

		// アニメーション変更
		auto model = GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Die_Root");
		model->Animator().CrossFade(anim, 1.0f, false);

		// 力をリセット
		m_vForce = { 0,0,0 };

		// 全コライダーOFF（地面判定とかぶつかり判定とかを無効にする）
		std::vector<KdSptr<ColliderComponent>> colliders;
		GetOwner()->GetComponents(colliders);
		for (auto&& collier : colliders) {
			collier->SetEnable(false);
		}
	}

	// 返信
	rep.IsGuard = false;

	return true;

	// ログ
	GAMESYS.m_editorData.LogWindow.AddLog("hitd");

}

void CharacterController::Editor_ImGuiUpdate()
{
	BaseCharacter::Editor_ImGuiUpdate();
	// Hp設定
	ImGui::InputInt("Hp", &m_hp);

	//DEBUG
	if (ImGui::Button("Die"))
	{
		auto model = GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Die_Root");
		model->Animator().CrossFade(anim, 1.0f, false);
	}

}

//-----------------------------
// 立ち
//-----------------------------
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

//-----------------------------
// 走る
//-----------------------------
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

//-----------------------------
// ジャンプ開始
//-----------------------------
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

//-----------------------------
// ジャンプ
//-----------------------------
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

//-----------------------------
// 着地
//-----------------------------
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

//-----------------------------
// 攻撃
//-----------------------------
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

//-----------------------------
// よろけ
//-----------------------------
void CharacterController::State_Stagger::Update()
{
	// ModelComponent取得
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// アニメ終了
	if (model->Animator().IsAnimationEnd()) {
		// 行動ステートを変更
		m_chara->m_nowState = m_chara->m_stateStorage.at("Stand");

		// アニメーション変更
		auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();
		auto anim = model->GetModel()->GetAnimation("Stand");
		model->Animator().CrossFade(anim, 0.5f, true);
	}

}

//-----------------------------
// 死亡
//-----------------------------
void CharacterController::State_Die::Update()
{
	// ModelComponent取得
	auto model = m_chara->GetOwner()->GetComponent<ModelComponent>();

	// アニメ終了
	if (model->Animator().IsAnimationEnd()) {
		m_cnt--;
		if (m_cnt <= 0) {
			m_chara->GetOwner()->Destroy();
		}
	}

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


void WeaponScript::Update()
{
	// 無効時はスキップ
	if (IsEnable() == false)return;
	// 停止時はスキップ
	if (GAMESYS.IsPlay() == false)return;

	// 装備者がヒットストップ中の場合、処理を中断
	auto chara = m_wpChara.lock();
	if (chara) {
		if (chara->GetHitStopCnt() >= 1)return;
	}


	//--------------
	// 無視リスト処理
	//--------------
	auto it = m_ignoreList.begin();
	while (it != m_ignoreList.end()) {
		// カウンタを減らす
		(*it).second--;
		// カウンタが0になったら、無視リストから削除
		if ((*it).second <= 0) {
			it = m_ignoreList.erase(it);
		}
		else {
			++it;
		}
	}

	// 有効カウンター
	m_enableTime--;

	if (m_enableTime < 0) {
		m_enableTime = 0;
	}
	// 有工事のみヒット処理
	else {
		//-------------------
		// ヒット処理
		//-------------------
		// 全コライダー達に、ヒット時処理をセット
		std::vector<KdSptr<ColliderComponent>> colliders;
		GetOwner()->GetComponents(colliders, true);
		for (auto&& collider : colliders) {
			// ヒット時処理を実行
			collider->m_onHitStay = [this](ColliderComponent* collider) {
				// 全ヒット情報
				for (auto&& res : collider->GetResults()) {
					// ヒット相手のBaseCharacterコンポーネントを取得
					auto you = res.Collider->GetOwner()->GetComponent<BaseCharacter>();
					// 相手はキャラではないので、ダメージ通知はしない
					if (you == nullptr) {
						continue;
					}

					// 無視リストにいるキャラなら、ダメージ処理などはしない
					if (m_ignoreList.count(res.Collider->GetOwner().get()) >= 1) {
						continue;
					}

					//-----------------------
					// ダメージ通知処理
					//-----------------------
					DamageArg dmg;	// 通知書
					dmg.AtkPower = m_hitPower;
					dmg.HitStop = 10;

					DamageReply rep;

					if (you->OnDamage(dmg, rep)) {
						// エフェクト
						auto effectObj = GAMESYS.Instantiate("Data\\Effect\\HitEffect.pref");
						if (effectObj) {
							// 座標をヒット位置に変更
							effectObj->Transform()->SetPosition(res.HitPosList.front());
						}

						// ヒットしたキャラ（装備者）を止める
						auto chara = m_wpChara.lock();
						if (chara) {
							chara->SetHitStopCnt(dmg.HitStop);
						}

						// ヒットしたキャラを一定期間無視化するため、GameObjectのアドレスを登録
						m_ignoreList[res.Collider->GetOwner().get()] = m_hitInterbal;

						// ログ
						GAMESYS.m_editorData.LogWindow.AddLog("hit");

					}

				}
			};
		}
	}
}

void PortalGimmick::Update()
{
	// 無効時はスキップ
	if (IsEnable() == false)return;
	// 停止時はスキップ
	if (GAMESYS.IsPlay() == false)return;

	// コライダーへのヒットを検知する
	auto collider = GetOwner()->GetComponent<ColliderComponent>();
	if (collider) {
		// ヒット時に実行される関数
		collider->m_onHitStay = [this](ColliderComponent* collider)
		{
			// ヒットしたプレイヤーを取得
			KdSptr<GameObject> pPlayer;
			for (auto&& res : collider->GetResults()) {
				if (res.Collider->GetOwner()->ExistTag("Player")) {
					// あたった奴を記憶
					pPlayer = res.Collider->GetOwner();
					break;
				}
			}
			// プレイヤーがいなかった場合は終了
			if (pPlayer == nullptr)return;

			// 指定名のGameObjectを探し、消す
			auto delObj = GAMESYS.GetLevel()->GetRootObject()->Find(m_deleteGameObjectName);
			if (delObj) {
				delObj->Destroy();
			}

			// 指定ファイルのPrefabを読み込み・追加
			auto newGameObj = GAMESYS.Instantiate(m_loadPrefabFile);
			if (newGameObj) {
				// 移動先のGameObjectを検索
				auto pointObj = newGameObj->Find(m_warpPointName);
				if (pointObj) {
					// ポイントの回転と座標をヒットした奴に入れる
					pPlayer->Transform()->SetRotation(pointObj->Transform()->GetRotation());
					pPlayer->Transform()->SetPosition(pointObj->Transform()->GetPosition());

				}
			}

		};
	}
}

void CharacterAIInput::Start()
{
	m_stateStorage["Wait"] = std::make_shared<State_Wait>();
	m_stateStorage["Chase"] = std::make_shared<State_Chase>();
	m_stateStorage["Attack"] = std::make_shared<State_Attack>();

	for (auto&& state : m_stateStorage) {
		state.second->m_input = this;
	}

	// 現在のステートをセット
	m_nowState = m_stateStorage.at("Wait");
}

void CharacterAIInput::Update()
{
	// 無効時はスキップ
	if (IsEnable() == false)return;
	// 停止時はスキップ
	if (GAMESYS.IsPlay() == false)return;

	// 自分のカメラ
	auto camObj = GetOwner()->Find("Camera");
	if (camObj == nullptr)return;

	/*
	// ターゲットのキャラへ移動する
	auto lockOnTarget = m_lockOn.lock();
	if (lockOnTarget) {
		// 相手のTransform
		auto targetTrans = lockOnTarget->Transform();

		// デバッグ線
		GAMESYS.DebugLine().AddLine(targetTrans->GetPosition() + KdVec3(0, 1, 0), GetOwner()->Transform()->GetPosition() + KdVec3(0, 1, 0), { 1,1,0,1 });

		//----------------------
		// 相手の方へカメラを向ける
		//----------------------
		auto charaCamera = camObj->GetComponent<CharacteCameraController>();
		if (charaCamera) {
			KdVec3 vTarget = targetTrans->GetPosition() - camObj->Transform()->GetPosition();
			vTarget.y = 0;
			vTarget.Normalize();

			charaCamera->SetAngleYByDirection(vTarget);
		}

		//-------------------------
		// キャラを前進
		//-------------------------
		// 相手との距離
		float distance = (targetTrans->GetPosition() - GetOwner()->Transform()->GetPosition()).Length();

		if (distance < 2.0f) {
			// 2ｍ以内なら、上を離す
			m_axes[(int)GameAxes::L].y = 0;			
			GAMESYS.m_editorData.LogWindow.AddLog(u8"2ｍ以内");
		}
		else {
			// 2ｍ以上なら、上を押す
			m_axes[(int)GameAxes:: L].y = 1;
			GAMESYS.m_editorData.LogWindow.AddLog(u8"2ｍ以上");

		}
	}
	else
	{
		// ターゲットしていない場合は、上を離す
		m_axes[(int)GameAxes::L].y = 0;
	}
	*/

	//----------------------
	// AIステート実行
	//----------------------
	if (m_nowState) {
		m_nowState->Update();
	}
	
	//----------------------
	// サーチ判定
	//----------------------
	if (m_cnt % 10 == 0) {
		// 標的をリセット
		m_lockOn.reset();

		// 球で周囲を判定
		KdSptr<SphereColliderComponent> serchCollider = std::make_shared<SphereColliderComponent>();
		serchCollider->SetOwner(GetOwner());

		// 情報の設定
		serchCollider->Set(GetOwner()->Transform()->GetPosition(), 5, true, 1, 131072);
		serchCollider->SetTrigger(true);
		// 球の情報が、Transformによって変化しないようにする
		serchCollider->SetUpdateCollisionData(false);

		// CollisionManagerに登録
		serchCollider->Register();

		// ヒット時に実行される関数
		serchCollider->m_onHitEnter = [this](ColliderComponent* collider)
		{
			for (auto&& res : collider->GetResults()) {
				m_lockOn = res.Collider->GetOwner();
			}
		};
	}
	// カウンタ増加
	m_cnt++;

}

void CharacterAIInput::State_Wait::Update()
{
	// ターゲット発見
	auto target = m_input->m_lockOn.lock();
	if (target)
	{
		// 「追跡」へ
		m_input->m_nowState = m_input->m_stateStorage.at("Chase");
	}
	// 移動キーを離す
	m_input->m_axes[(int)GameAxes::L] = { 0, 0 };
}

void CharacterAIInput::State_Chase::Update()
{
	// キーをクリア
	m_input->m_buttonFlags.fill(0);

	// 自分のカメラ
	auto camObj = m_input->GetOwner()->Find("Camera");
	if (camObj == nullptr)return;

	// ターゲットのキャラへ移動する
	auto lockOnTarget = m_input->m_lockOn.lock();
	if (lockOnTarget)
	{
		// 相手のTransform
		auto targetTrans = lockOnTarget->Transform();

		// デバッグ線
		GAMESYS.DebugLine().AddLine(targetTrans->GetPosition() + KdVec3(0, 1, 0), m_input->GetOwner()->Transform()->GetPosition() + KdVec3(0, 1, 0), { 1,1,0,1 });

		//================================
		// 相手の方へカメラを向ける
		//================================
		auto charaCamera = camObj->GetComponent<CharacteCameraController>();
		if (charaCamera)
		{
			// 相手への方向
			auto vTarget = targetTrans->GetPosition() - camObj->Transform()->GetPosition();
			vTarget.y = 0;
			vTarget.Normalize();
			charaCamera->SetAngleYByDirection(vTarget);
		}
		
		// 相手との距離
		float distance = (targetTrans->GetPosition() - m_input->GetOwner()->Transform()->GetPosition()).Length();
		if (distance < 2.0f)
		{
			// 2m以内なら、上を離す
			m_input->m_axes[(int)GameAxes::L].y = 0;
		}
		else
		{
			// 2m以上なら、上を押す
			m_input->m_axes[(int)GameAxes::L].y = 1;
		}
		
		// 一定確率で攻撃
		if (KdMTRand::s_rnd.GetInt(100) == 0)
		{
			// 「攻撃」へ
			m_input->m_nowState = m_input->m_stateStorage.at("Attack");
		}
	}
	else
	{
		// ターゲットがいないなら、待機へ
		m_input->m_nowState = m_input->m_stateStorage.at("Wait");
	}
}

void CharacterAIInput::State_Attack::Update()
{
	// 方向キーをクリア
	m_input->m_axes[(int)GameAxes::L] = { 0, 0 };
	// 攻撃ボタンを押す
	m_input->PressButton(GameButtons::A);
	// 追跡へ
	m_input->m_nowState = m_input->m_stateStorage.at("Chase");
}
