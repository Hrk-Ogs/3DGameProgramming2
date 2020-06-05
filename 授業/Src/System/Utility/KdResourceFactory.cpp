#include "System/KdSystem.h"

#include "KdResourceFactory.h"

KdSptr<KdModel> KdResourceFactory::GetModel(const std::string& filename)
{
	// 存在チェック
	{
		// マルチスレッド用排他制御
		std::lock_guard<std::mutex> lockguard(m_lock);

		// filenameのモデルがあるか？
		auto itFound = m_modelMap.find(filename);
		// ある場合
		if (itFound != m_modelMap.end())
		{
			// それをそのまま返す
			return (*itFound).second;
		}
	}

	// 生成 & 読み込み
	auto newModel = std::make_shared<KdModel>();
	if (newModel->Load(filename) == false) {
		// 読み込み失敗時は、nullを返す
		return nullptr;
	}

	
	m_lock.lock();			// マルチスレッド用排他制御
	// リスト(map)に登録
	m_modelMap[filename] = newModel;

	m_lock.unlock();

	// リソースを返す
	return newModel;
}

KdSptr<KdTexture> KdResourceFactory::GetTexture(const std::string& filename)
{
	// 存在チェック
	{
		// マルチスレッド用排他制御
		std::lock_guard<std::mutex> lockguard(m_lock);

		// filenameのモデルがあるか？
		auto itFound = m_textureMap.find(filename);
		// ある場合
		if (itFound != m_textureMap.end())
		{
			// それをそのまま返す
			return (*itFound).second;
		}
	}

	// 生成 & 読み込み
	auto tex = std::make_shared<KdTexture>();
	if (tex->Load(filename) == false) {
		return nullptr;
	}

	m_lock.lock();			// マルチスレッド用排他制御
	// 登録
	m_textureMap[filename] = tex;
	m_lock.unlock();

	// 返す
	return tex;
}

KdSptr<KdSoundEffect> KdResourceFactory::GetSound(const std::string& filename)
{
	// 存在チェック
	{
		// マルチスレッド用排他制御
		std::lock_guard<std::mutex> lockguard(m_lock);

		// filenameのモデルがあるか？
		auto itFound = m_soundMap.find(filename);
		// ある場合
		if (itFound != m_soundMap.end())
		{
			// それをそのまま返す
			return (*itFound).second;
		}
	}

	// 生成 & 読み込み
	auto sound = std::make_shared<KdSoundEffect>();
	if (sound->Load(filename) == false) {
		return nullptr;
	}

	m_lock.lock();			// マルチスレッド用排他制御
	// 登録
	m_soundMap[filename] = sound;

	m_lock.unlock();

	// 返す
	return sound;
}

json11::Json KdResourceFactory::GetJSON(const std::string & filename)
{
	// 存在チェック
	{
		// マルチスレッド用排他制御
		std::lock_guard<std::mutex> lockguard(m_lock);

		// filenameのモデルがあるか？
		auto itFound = m_jsonMap.find(filename);
		// ある場合
		if (itFound != m_jsonMap.end())
		{
			// それをそのまま返す
			return (*itFound).second;
		}
	}


	// JSONファイルを文字列として読み込む
	json11::Json json = KdLoadJSONFile(filename);
	if (json.is_null())return nullptr;

	m_lock.lock();			// マルチスレッド用排他制御
	// 登録
	m_jsonMap[filename] = json;

	m_lock.unlock();

	// 返す
	return json;
}

// 管理を破棄する

void KdResourceFactory::Clear()
{
	m_lock.lock();

	m_modelMap.clear();
	m_textureMap.clear();
	m_jsonMap.clear();

	m_lock.unlock();
}

int KdResourceFactory::Optimize()
{
	// マルチスレッド用排他制御
	std::lock_guard<std::mutex> lockguard(m_lock);

	int cnt = 0;

	// １つしか参照していないリソースをリストから解除する(解放される)。
	for (auto it = m_modelMap.begin(); it != m_modelMap.end();) {
		if ((*it).second.use_count() == 1) {
			it = m_modelMap.erase(it);
			cnt++;
		}
		else {
			++it;
		}
	}

	for (auto it = m_textureMap.begin(); it != m_textureMap.end();) {
		if ((*it).second.use_count() == 1) {
			it = m_textureMap.erase(it);
			cnt++;
		}
		else {
			++it;
		}
	}

	for (auto it = m_soundMap.begin(); it != m_soundMap.end();) {
		if ((*it).second.use_count() == 1) {
			it = m_soundMap.erase(it);
			cnt++;
		}
		else {
			++it;
		}
	}

	// jsonMapは全解放する
	m_jsonMap.clear();

	return cnt;
}

