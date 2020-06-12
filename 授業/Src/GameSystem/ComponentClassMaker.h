#pragma once

class BaseComponent;
//=========================================
//
// コンポーネントクラスメーカー
// ・Register関数で指定型のクラスを登録しておけば、
// MakeShared関数で文字列名からインスタンスを作成できる
//
//=========================================
class ComponentClassMaker
{
public:
	// クラスを登録
	template<class T>
	void Register()
	{
		m_map[&typeid(T).name()[6]] = []() { return std::make_shared<T>(); };
		// ※&typeid(T).name()[6]は環境依存です。
		// ※VisualStudio以外の時は変更する必要があります。
	}
	// 指定名のクラスのメモリを確保し返す
	KdSptr<BaseComponent> MakeShared(const std::string& className)
	{
		auto it = m_map.find(className);
		if (it == m_map.end())return nullptr;
		// ポインタを指定型へ変換
		return (*it).second();
	}
	// 登録されているクラス名リストを取得する(主にエディターで使用)
	std::vector<std::string> GetClassNameList()
	{
		std::vector<std::string> list;
		for (auto&& n : m_map)
		{
			list.push_back(n.first);
		}
		return list;
	}
private:
	// クラスマップ
	std::unordered_map<std::string, std::function<KdSptr<BaseComponent>()>> m_map;
	//----------------------------
	// シングルトンパターン
	//----------------------------
private:
	ComponentClassMaker() {}
public:
	static ComponentClassMaker& GetInstance()
	{
		static ComponentClassMaker instance;
		return instance;
	}
};