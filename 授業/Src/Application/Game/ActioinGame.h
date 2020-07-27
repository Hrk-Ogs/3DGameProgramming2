#pragma once


// ダメージ通知書
struct DamageArg
{
	int AtkPower = 0;		// 攻撃の威力
	int HitStop = 0;		// ヒットストップさせる値

};

// ダメージ結果返信書
struct DamageReply
{
	bool	IsGuard = false;		// 防御したよ！
};

//=====================================
// ゲームキャラクター基本クラス
//=====================================
class BaseCharacter : public BaseComponent
{
public:
	// ダメージ通知
	// ・dmg ……攻撃者から送られるダメージ通知データ
	// ・rep ……攻撃者へ返信する内容
	virtual bool OnDamage(const DamageArg& dmg, DamageReply& rep) { return false; }

	// ヒットストップ値設定
	void SetHitStopCnt(int cnt) { m_hitStopCnt = cnt; }
	// ヒットストップ値取得
	int GetHitStopCnt()const { return m_hitStopCnt; }

protected:
	// ヒットストップ値（カウンタ）
	int		m_hitStopCnt = 0;	// 0：通常操作　1～：ヒットストップ

};



//=====================================
// キャラクター制御
//=====================================
class CharacterController : public BaseCharacter
{
public:
	// 更新処理
	virtual void Start() override;

	// 更新処理
	virtual void Update() override;
	// 後更新処理
	virtual void LateUpdate() override;

	// ダメージ通知
// ・dmg ……攻撃者から送られるダメージ通知データ
// ・rep ……攻撃者へ返信する内容
	virtual bool OnDamage(const DamageArg& dmg, DamageReply& rep) override;


	// ImGui処理
	virtual void Editor_ImGuiUpdate() override;


	//===============================
	// シリアライズ/デシリアライズ
	//===============================
	virtual void Deserialize(const json11::Json& jsonObj)
	{
		BaseCharacter::Deserialize(jsonObj);
		KdJsonGet(jsonObj["Hp"], m_hp);
	}
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseCharacter::Serialize(outJsonObj);
		outJsonObj["Hp"] = m_hp;
	}

private:
	// カメラ演出時間
	int m_cameraEffectTime = 0;

	// 体力
	int m_hp = 600;

	// 設置相手へのアドレス（コレをみれば、誰の上に立っているかわかる）
	KdWptr<GameObject> m_wpGround;

	// 力ベクトル
	// ・このキャラクターに掛かっている力
	// ・完成のかかるような移動で使用する（重力など）
	KdVec3 m_vForce;

	// 重力
	void CalcGravity()
	{
		const float kGravityPower = 0.01f;
		m_vForce.y -= kGravityPower;
	}
	// 摩擦
	void CalcFriction()
	{
		const float kFrictionGround = 0.9f;	// 地上時の摩擦力
		const float kFrictionAir = 0.99f;	// 空中時の摩擦力
		// 空中時
		if (m_wpGround.expired()) {
			m_vForce *= kFrictionAir;
		}
		// 地上時
		else {
			m_vForce *= kFrictionGround;
		}
	}

	//================
	// 行動ステートマシン
	//================
	// 行動ステート基本クラス
	class State_Base
	{
	public:

		virtual ~State_Base(){}
		// 行動処理
		virtual void Update() {}
		// キャラへの参照
		CharacterController* m_chara = nullptr;
	};

	//「立ち」行動クラス
	class State_Stand :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	//「走り」行動クラス
	class State_Run :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	// ジャンプ開始
	class State_JumpStart :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
		KdVec3 m_force;	// 実際にジャンプするときの力
	};

	// ジャンプ中
	class State_Jump :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	// 着地
	class State_Landing :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	// 攻撃
	class State_Attack :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	// よろけ
	class State_Stagger :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;
	};

	// 死亡
	class State_Die :public State_Base
	{
	public:
		// 行動処理
		virtual void Update() override;

		void Reset()
		{
			m_cnt = 0;
		}

	private:
		// 削除までのカウンター
		int m_cnt = 180;
	};

	// 現在の行動ステートへの参照
	KdSptr<State_Base>	m_nowState;

	// 全行動ステートをあらかじめ作成して保管しておく場所
	std::unordered_map<std::string, KdSptr<State_Base>>	m_stateStorage;

};


//=========================================
// タイトル制御クラス
//=========================================
class TitleController :public UIObserverComponent
{
public:
	virtual void Update() override
	{
		// 無効時はスキップ
		if (m_enable == false)return;
		// 停止時はスキップ
		if (GAMESYS.IsPlay() == false)return;

		// 入力コンポーネント
		auto input = GetOwner()->GetComponent<InputComponent>();
		if (input == nullptr)return;

		// 決定ボタン
		if (input->IsButtonPressed(GameButtons::A)) {
			// Level切り替え予約
			GAMESYS.ChangeLevel("Data\\Level\\Stage1.level");
		}
	}

	// ImGui処理
	virtual void Editor_ImGuiUpdate()override
	{
		UIObserverComponent::Editor_ImGuiUpdate();

		auto input = GetOwner()->GetComponent<InputComponent>();
		if (input == nullptr) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(u8"InputComponentを追加してください！！");
			ImGui::PopStyleColor();
		}
	}

	// UIがクリックされたと通知される
// ・gameObj … クリックされたやつ
// ・message … 送られてくる文字列
	virtual void NotifyClicked(KdSptr<GameObject> gameObj, const std::string& message) override
	{
		// messageを判定し、処理を行う
		if (message == "GameStart")
		{
			// Level切り替え予約
			GAMESYS.ChangeLevel("Data\\Level\\Stage1.level");
		}
	}


};

//=================================================
//
// TPS系カメラの動きをするコンポーネント（CamerComponentではない）
//
//=================================================
class CharacteCameraController : public BaseComponent
{
public:
	// パラメータ取得・設定
	KdVec2& CamAngles() { return m_camAngles; }
	KdVec3& CamOffset() { return m_camOffset; }

	// 指定方向dirになるように、カメラのY角度をセットする
	void SetAngleYByDirection(const KdVec3& dir)
	{
		// Y角度を算出
		KdVec3 normDir = dir;
		normDir.y = 0;
		normDir.Normalize();
		if (normDir.z < 0)
		{
			m_camAngles.y = 180 - asin(normDir.x) * KdToDegrees;
		}
		else
		{
			m_camAngles.y = asin(normDir.x) * KdToDegrees;
		}
	}


	// 更新処理
	virtual void Update()override;

	// ImGui処理
	virtual void Editor_ImGuiUpdate()override
	{
		BaseComponent::Editor_ImGuiUpdate();

		ImGui::DragFloat2(u8"カメラ角度", &m_camAngles.x, 0.1f);
		ImGui::DragFloat3(u8"カメラオフセット", &m_camOffset.x, 0.01f);
	}

	//==============================
	// Serialize/Deserialize
	//==============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData) 
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["CamAngles"], m_camAngles);
		KdJsonGet(jsonData["CamOffset"], m_camOffset);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["CamAngles"] = m_camAngles.ToArray();
		outJsonObj["CamOffset"] = m_camOffset.ToArray();
	}

private:

	// カメラ角度
	KdVec2 m_camAngles;
	// 
	KdVec3 m_camOffset;

};

//=========================================
//
// 武器制御スクリプト
//
//=========================================
class WeaponScript :public BaseComponent
{
	struct Parameter;
public:
	// 武器の性能取得
	const Parameter& GetParameter()const { return m_param; }

	// 装備者を設定
	void SetChara(KdSptr<BaseCharacter> chara)
	{
		m_wpChara = chara;
	}

	// ヒットストップ値を設定
	void SetHitStopValue(int value)
	{
		m_hitStopValue = value;
	}

	// ヒット時の威力設定
	void SetHitPower(int power) 
	{
		m_hitPower = power;
	}

	// 有効時間を設定
	void SetEnableTime(int count)
	{
		m_enableTime = count;
	}

	// ヒット間隔設定
	void SetHitInterval(int value)
	{
		m_hitInterbal = value;
	}

	// 無視リストをクリアする
	void ClearIgnoreList()
	{
		m_ignoreList.clear();
	}

	// 更新処理
	virtual void Update() override;

	//=============================
	// Serialize/Deserialize
	//=============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData) 
	{
		BaseComponent::Deserialize(jsonData);

		KdJsonGet(jsonData["Param_Power"], m_param.Power);
		KdJsonGet(jsonData["Param_CrititalRate"], m_param.CrititalRate);
		KdJsonGet(jsonData["HitInterval"], m_hitInterbal);
		KdJsonGet(jsonData["EnablieTime"], m_enableTime);
		KdJsonGet(jsonData["HitStopValue"], m_hitStopValue);

		// 武器の威力をヒット威力としていれておく
		m_hitPower = m_param.Power;
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["Param_Power"] = m_param.Power;
		outJsonObj["Param_CrititalRate"] = m_param.CrititalRate;
		outJsonObj["HitInterval"] = m_hitInterbal;
		outJsonObj["EnablieTime"] = m_enableTime;
		outJsonObj["HitStopValue"] = m_hitStopValue;
	}

	// 
	virtual void Editor_ImGuiUpdate()override
	{
		BaseComponent::Editor_ImGuiUpdate();

		ImGui::InputInt(u8"威力", &m_param.Power);
		ImGui::InputInt(u8"クリティカル率", &m_param.CrititalRate);

		ImGui::InputInt(u8"ヒットストップ間隔", &m_hitInterbal);
		ImGui::InputInt(u8"有効時間", &m_enableTime);
		ImGui::InputInt(u8"ヒットストップ値", &m_hitStopValue);
	}


private:
	// 固定の武器の性能
	struct Parameter 
	{
		int Power = 1;			// 武器の威力
		int CrititalRate = 0;	// クリティカル率
	};
	Parameter m_param;

	// 動的な武器の性能
	int m_hitInterbal = 0;	
	int m_hitPower = 1;
	int m_hitStopValue = 0;

	// 実行時変数
	int m_enableTime = 0;
	KdWptr<BaseCharacter> m_wpChara;		// 装備者へのアドレス

	// 無視リスト（個々に登録されているGameObjectはヒットしていても無視する）
	std::unordered_map<const GameObject*, int> m_ignoreList;
};

//==================================================
// ポータルギミック
//==================================================
class PortalGimmick :public BaseComponent
{
public:
	// 更新処理
	virtual void Update() override;

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const json11::Json& jsonData)
	{
		BaseComponent::Deserialize(jsonData);
		KdJsonGet(jsonData["LoadPrefabFile"], m_loadPrefabFile);
		KdJsonGet(jsonData["DeleteGameObjectName"], m_deleteGameObjectName);
		KdJsonGet(jsonData["WarpPointName"], m_warpPointName);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(json11::Json::object& outJsonObj) const
	{
		BaseComponent::Serialize(outJsonObj);
		outJsonObj["LoadPrefabFile"] = m_loadPrefabFile;
		outJsonObj["DeleteGameObjectName"] = m_deleteGameObjectName;
		outJsonObj["WarpPointName"] = m_warpPointName;
	}

	//
	virtual void Editor_ImGuiUpdate() override
	{
		BaseComponent::Editor_ImGuiUpdate();
		ImGui::InputText(u8"追加するPrefabファイル名", &m_loadPrefabFile);
		ImGui::InputText(u8"削除するGameObject名", &m_deleteGameObjectName);
		ImGui::InputText(u8"ワープ先GameObject名", &m_warpPointName);
	}

private:
	// 読み込むPrefabファイル名
	std::string		m_loadPrefabFile;
	// 削除するGameObject名
	std::string		m_deleteGameObjectName = "StageMap";
	// ワープ先のGameObject名
	std::string		m_warpPointName;
};

//=================================================
//
// 敵AI入力コンポーネント
//
//=================================================
class CharacterAIInput : public InputComponent
{
public:
	// 初期処理
	virtual void Start() override;

	// AI入力処理
	virtual void Update() override;

private:
	// 標的となるキャラ
	KdWptr<GameObject> m_lockOn;
	// 経過カウント
	int m_cnt = 0;

	//---------------------------
	// AIステート基本クラス
	//---------------------------
	class State_Base
	{
	public:
		virtual ~State_Base(){}
		// AI処理
		virtual void Update(){}
		// 持ち主へのアドレス
		CharacterAIInput* m_input = nullptr;
	};

	// 待機
	class State_Wait :public State_Base
	{
	public:
		// AI処理
		virtual void Update() override;
	};
	
	// 追跡
	class State_Chase :public State_Base
	{
		// AI処理
		virtual void Update() override;
	};

	// 攻撃
	class State_Attack :public State_Base
	{
		// AI処理
		virtual void Update() override;
	};

	// 現在のAIステート
	KdSptr<State_Base> m_nowState;
	// 全AIクラスをあらかじめ作成して保管しておく場所
	std::unordered_map<std::string, KdSptr<State_Base>> m_stateStorage;

};
