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

	// 輪郭On/Off
	void SetOutlineEnable(bool enable)
	{
		m_enableOutline = enable;
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
		KdJsonGet(jsonObj["EnableOutline"], m_enableOutline);
		KdJsonGet(jsonObj["UseWaveNormalMap"], m_useWaveNormalMap);
		KdJsonGet(jsonObj["MulMaterialColor"], m_mulMaterialColor);
		KdJsonGet(jsonObj["UvOffset"], m_uvOffset);
		KdJsonGet(jsonObj["UvTiling"], m_uvTiling);
		KdJsonGet(jsonObj["RefractiveIndex"], m_refractiveIndex);
		KdJsonGet(jsonObj["CastShadow"], m_castShadow);

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
		outJsonObj["EnableOutline"] = m_enableOutline;
		outJsonObj["UseWaveNormalMap"] = m_useWaveNormalMap;
		outJsonObj["MulMaterialColor"] = m_mulMaterialColor.ToArray();
		outJsonObj["UvOffset"] = m_uvOffset.ToArray();
		outJsonObj["UvTiling"] = m_uvTiling.ToArray();
		outJsonObj["RefractiveIndex"] = m_refractiveIndex;
		outJsonObj["CastShadow"] = m_castShadow;
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
	bool					m_lightEnable = true;		// ライト有効
	bool					m_fogEnable = true;			// フォグ有効
	bool					m_enableOutline = false;	// 輪郭有効
	bool m_useWaveNormalMap = false; // 波法線マップを使用する
	KdVec4 m_mulMaterialColor = { 1,1,1,1 }; // 乗算色
	KdVec2 m_uvOffset = { 0, 0 }; // UVオフセット
	KdVec2 m_uvTiling = { 1, 1 }; // UVタイリング
	float m_refractiveIndex = 1.0f; // 屈折率
	bool m_castShadow = true; // 影を生成する
};
