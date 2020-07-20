#pragma once

//===================================
// モデルコンポーネント
// ・3Dモデルの処理や描画を行う部品
//===================================
class ModelComponent :public BaseComponent
{
public:

	// モデルデータをセット
	// ノードアウトライナー初期化やアニメーター初期化なども行う
	void SetModel(const KdSptr<KdModel>& model);

	// モデルデータ取得
	KdSptr<KdModel> GetModel()const { return m_model; }

	// ノードアウトライナー取得
	KdNodeOutliner& NodeOutliner() { return m_nodeOL; }

	// アニメータ取得
	KdAnimator& Animator() { return m_animator; }

	// アニメーションスクリプト時に実行される関数をセット
	void SetAnimationScriptProc(std::function<void(const KdKeyScript&)> scriptProc)
	{
		m_onAnimeScriptProc = scriptProc;
	}

	// ルートモーションOn/Off設定
	void SetEnableRootMotion(bool enable)
	{
		m_enableRootMotion = enable;
	}

	//===============================
	// 動作処理
	//===============================
	// 更新処理
	virtual void Update() override;
	// 描画準備処理
	virtual void PrepareDraw(RenderingData& rdate) override;
	// 描画処理
	// phaseID　……どのタイミングで実行されているかのフラグ
	virtual void Draw(int phaseID) override;

	//===============================
	// シリアライズ/デシリアライズ
	//===============================
	// JSONデータから復元
	virtual void Deserialize(const json11::Json& jsonObj)override
	{
		// 継承元の関数も実行する
		BaseComponent::Deserialize(jsonObj);

		KdJsonGet(jsonObj["ModelFilename"], m_model);
		KdJsonGet(jsonObj["StartAnimeName"], m_startAnimeName);
		KdJsonGet(jsonObj["StartAnimatioinLoop"], m_startAnimationLoop);
		KdJsonGet(jsonObj["AnimationSpeed"], m_animationSpeed);
		KdJsonGet(jsonObj["EnableRootMotion"], m_enableRootMotion);

		KdJsonGet(jsonObj["Visible"], m_visible);
		KdJsonGet(jsonObj["LightEnable"], m_lightEnable);
		KdJsonGet(jsonObj["FogEnable"], m_fogEnable);

		// モデルデータのセット
		SetModel(m_model);
	}

	// JSONデータ化
	virtual void Serialize(json11::Json::object& outJsonObj)const override
	{
		// 継承元の関数も実行する
		BaseComponent::Serialize(outJsonObj);

		outJsonObj["ModelFilename"] = m_model->GetFilepath();

		outJsonObj["StartAnimeName"] = m_startAnimeName;
		outJsonObj["StartAnimatioinLoop"] = m_startAnimationLoop;
		outJsonObj["AnimationSpeed"] = m_animationSpeed;
		outJsonObj["EnableRootMotion"] = m_enableRootMotion;
		outJsonObj["Visible"] = m_visible;
		outJsonObj["LightEnable"] = m_lightEnable;
		outJsonObj["FogEnable"] = m_fogEnable;
	}

	//===============================
	// ImGui処理
	//===============================
	virtual void Editor_ImGuiUpdate() override;

private:

	// モデルデータ 3Dメッシュモデルの元データ
	// ※バグ軽減のため、必ずnullptrにならないようにする
	KdSptr<KdModel>			m_model = std::make_shared<KdModel>();

	// ノードアウトライナー　ノード（ボーン）の扱い専門
	KdNodeOutliner			m_nodeOL;

	// アニメーションスクリプトコールバック関数オブジェクト
	std::function<void(const KdKeyScript&)> m_onAnimeScriptProc;

	// アニメーター
	KdAnimator				m_animator;
	std::string				m_startAnimeName;
	bool					m_startAnimationLoop = true;
	float					m_animationSpeed = 1.0f;
	bool					m_enableRootMotion = false;	// ルートモーション有効

	// 描画設定
	bool					m_visible = true;
	bool					m_lightEnable = true;	// ライト有効
	bool					m_fogEnable = true;		// フォグ有効

};
