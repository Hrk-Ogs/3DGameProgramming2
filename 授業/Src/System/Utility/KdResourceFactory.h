#pragma once

//==============================
// リソース管理クラス
// ・Flyweightパターン
//==============================
class KdResourceFactory {
public:

	// モデルデータ取得
	KdSptr<KdModel> GetModel(const std::string& filename);

	// テクスチャ取得
	KdSptr<KdTexture> GetTexture(const std::string& filename);

	// サウンド取得
	KdSptr<KdSoundEffect> GetSound(const std::string& filename);

	// JSON取得
	json11::Json GetJSON(const std::string& filename);

	// 管理を破棄する
	void Clear();

	// 参照が１つしかない(外部から参照されていない)リソースは、管理リストから解除する
	// ・戻り値：解放されたリソース数
	int Optimize();

private:
	// モデルデータ管理マップ
	std::unordered_map< std::string, KdSptr<KdModel> > m_modelMap;
	
	// テクスチャ管理マップ
	std::unordered_map< std::string, KdSptr<KdTexture> > m_textureMap;

	// サウンド管理マップ
	std::unordered_map< std::string, KdSptr<KdSoundEffect> > m_soundMap;

	// JSON管理マップ
	std::unordered_map< std::string, json11::Json > m_jsonMap;

	// マルチスレッド排他制御用
	std::mutex	m_lock;

//============================
// シングルトン
//============================
private:
	KdResourceFactory() {}

public:
	static KdResourceFactory& GetInstance() {
		static KdResourceFactory instance;
		return instance;
	}
};

#define KDResFactory KdResourceFactory::GetInstance()
