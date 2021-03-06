#pragma once

//=======================================
// コンポーネント基本クラス
//=======================================
class BaseComponent :public std::enable_shared_from_this<BaseComponent>
{
public:

	// 仮想デストラクタ
	virtual ~BaseComponent(){}

	//============================
	// 動作処理
	//============================
	// 初回に一度だけ実行される関数
	virtual void Start() {}
	// 毎回実行される更新処理
	virtual void Update() {}
	// 後更新関数
	virtual void LateUpdate() {}

	// 描画準備処理
	virtual void PrepareDraw(RenderingData& rdata) {}

	// 毎回実行される描画関数
	// phaseID ……殿タイミングで実行されているかのフラグ　RenderingData構造体の「Phase定数」を指定する
	virtual void Draw(int phaseID) {}

	// 初回のみStart関数を呼び出す
	void CallStartOnce()
	{
		// 初回フラグがOnの時のみ
		if (m_isNew) {
			Start();		// Start実行
			m_isNew = false;
		}
	}

	//============================
	// ImGui処理
	//============================
	virtual void Editor_ImGuiUpdate()
	{
		ImGui::Checkbox("Enable", &m_enable);
	}

	//============================
	// シリアライズ/デシリアライズ
	//============================
	// JSONデータから自分のデータを復元する
	virtual void Deserialize(const json11::Json& jsonObj)
	{
		KdJsonGet(jsonObj["Enable"], m_enable);
	}

	// 自分をJSONデータ化
	virtual void Serialize(json11::Json::object& outJsonObj)const
	{
		// 自分のクラス名を文字列で入れる
		outJsonObj["ClassName"] = &typeid(*this).name()[6];
		outJsonObj["Enable"] = m_enable;
	}

	//============================
	// 設定・取得
	//============================
	// オーナー（持ち主）を取得
	KdSptr<GameObject> GetOwner() const { return m_pOwner.lock(); }
	// オーナーの設定
	void SetOwner(KdSptr<GameObject>& obj) { m_pOwner = obj; }
	// 持ち主が存在するか
	bool ExistOwner() const { return m_pOwner.expired() == false; }

	// 有効フラグの取得
	void SetEnable(bool enable) { m_enable = enable; }

	// 有効フラグの設定
	bool IsEnable() const { return m_enable; }

	// 持ち主存在チェック
	//bool ExistOwner() const { return m_pOwner.expired() == false; }

protected:

	// 有効フラグ
	bool			m_enable = true;

	// 作成したて？のフラグ
	bool			m_isNew = true;

	// 持ち主へのアドレス
	KdWptr<GameObject>	m_pOwner;

};