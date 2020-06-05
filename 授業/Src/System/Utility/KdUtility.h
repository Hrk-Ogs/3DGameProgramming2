#pragma once

//===========================================
//
// 便利機能
//
//===========================================

// ComPtr短縮名
using Microsoft::WRL::ComPtr;
// shared_ptr短縮名
template<class T>
using KdSptr = std::shared_ptr<T>;
template<class T>
using KdWptr = std::weak_ptr<T>;

// 算術系短縮名
namespace Math = DirectX::SimpleMath;

// 角度変換
constexpr float KdToDegrees = 180.0f / 3.141592654f;
constexpr float KdToRadians = 3.141592654f / 180.0f;

// 安全にReleaseするための関数
template<class T>
void KdSafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

// 安全にDeleteするための関数
template<class T>
void KdSafeDelete(T*& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

// ファイルパスから、親ディレクトリまでのパスを取得
inline std::string KdGetDirFromPath(const std::string &path)
{
	const std::string::size_type pos = std::max<signed>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

// std::string版 sprintf
template <typename ... Args>
std::string KdFormat(const std::string& fmt, Args ... args)
{
	size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
	std::vector<char> buf(len + 1);
	std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
	return std::string(&buf[0], &buf[0] + len);
}

//=====================================================
//
// テクスチャ
//
//=====================================================

// viewから画像情報を取得する
void KdGetTextureInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc);

//=================================================================
//
// メルセンヌ･ツイスター乱数クラス
//
//=================================================================
class KdMTRand {
public:
	// 乱数シードを設定する(つまりシャッフル)
	// ・seed	… 乱数シード
	void InitSeed(ULONG seed)
	{
		m_rnd.seed(seed);
	}

	// 整数の乱数取得
	// ・戻り値	… 32bit整数値(0～4294967295)
	UINT GetInt() { return m_rnd(); }

	// 整数の乱数取得
	// ・戻り値	… 0 ～ num-1 までの乱数
	int GetInt(int num) {
		return m_rnd() % num;
	}

	// st ～ ed間の整数乱数を発生
	// ・戻り値	… st ～　ed の範囲の乱数
	int GetInt(int st, int ed) {
		if (ed < st)std::swap(ed, st);
		return (int)GetInt(ed - st + 1) + st;
	}

	// st ～ ed間の浮動小数点乱数を発生
	// ・戻り値	… st ～　ed の範囲の乱数
	float GetFloat(float st, float ed) {
		if (ed < st)std::swap(ed, st);
		return (GetInt() / 4294967295.0f) * (ed - st) + st;
	}

	//
	KdMTRand() {}
	KdMTRand(ULONG seed) : m_rnd(seed) {}

	// とりあえずstaticで１つ作成しておく
	static KdMTRand s_rnd;

private:
	std::mt19937	m_rnd;		// メルセンヌ・ツイスター乱数
};