#pragma once

class KdVec2;
class KdVec3;
class KdVec4;
class KdQuaternion;
class KdMatrix;

//=========================================
//
// 2Dベクトル
//
//=========================================
class KdVec2 : public DirectX::XMFLOAT2
{
public:
	// コンストラクタで0埋め
	KdVec2()
	{
		x = 0;
		y = 0;
	}
	// 座標指定付きコンストラクタ
	KdVec2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	// XMVECTORから代入してきた時
	KdVec2(const DirectX::XMVECTOR& V)
	{
		// 変換して代入
		DirectX::XMStoreFloat2(this, V);
		// 下記と同意だが、上記のSIMD命令を使用したほうが高速
//		x = V.m128_f32[0];
//		y = V.m128_f32[1];
	}
	// XMFLOAT2から代入してきた時
	KdVec2(const DirectX::XMFLOAT2& V)
	{
		x = V.x;
		y = V.y;
	}

	// XMVECTORへ変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat2(this); }
	// Math::Vector2と互換性を持つための関数
	operator Math::Vector2& () { return *(Math::Vector2*)this; }

	// 演算子
	KdVec2& operator+= (const KdVec2& v)
	{
		*this = DirectX::XMVectorAdd(*this, v);
		return *this;
	}

	KdVec2& operator-= (const KdVec2& v)
	{
		*this = DirectX::XMVectorSubtract(*this, v);
		return *this;
	}

	KdVec2& operator*= (const KdVec2& v)
	{
		*this = DirectX::XMVectorMultiply(*this, v);
		return *this;
	}

	KdVec2& operator*= (float s)
	{
		*this = DirectX::XMVectorScale(*this, s);
		return *this;
	}

	KdVec2& operator/= (float s)
	{
		assert(s != 0.0f);
		*this = DirectX::XMVectorScale(*this, 1.f / s);
		return *this;
	}

	// セット
	void Set(float fx, float fy)
	{
		x = fx;
		y = fy;
	}

	// 長さ
	float Length() const
	{
		return DirectX::XMVector2Length(*this).m128_f32[0];
	}
	// 長さの２乗(高速なので判定用に使用することが多い)
	float LengthSquared() const
	{
		return DirectX::XMVector2LengthSq(*this).m128_f32[0];
	}

	// 内積
	float Dot(const KdVec2& v) const
	{
		return DirectX::XMVector2Dot(*this, v).m128_f32[0];
	}
	// 内積
	static float sDot(const KdVec2& v1, const KdVec2& v2)
	{
		return DirectX::XMVector2Dot(v1, v2).m128_f32[0];
	}

	// 外積
	KdVec2 Cross(const KdVec2& v)
	{
		return DirectX::XMVector2Cross(*this, v);
	}

	// 外積
	static KdVec2 sCross(const KdVec2& v1, KdVec2& v2)
	{
		return DirectX::XMVector2Cross(v1, v2);
	}

	// 自分を正規化
	KdVec2& Normalize()
	{
		*this = DirectX::XMVector2Normalize(*this);
		return *this;
	}

	// クランプ
	KdVec2& Clamp(const KdVec2& vmin, const KdVec2& vmax)
	{
		*this = DirectX::XMVectorClamp(*this, vmin, vmax);
		return *this;
	}

	// 線形補間
	KdVec2& Lerp(const KdVec2& v1, const KdVec2& v2, float t)
	{
		*this = DirectX::XMVectorLerp(v1, v2, t);
		return *this;
	}

	// スムーズステップ補間
	KdVec2& SmoothStep(const KdVec2& v1, const KdVec2& v2, float t)
	{
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // 値を0～1にクランプ
		t = t * t * (3.f - 2.f * t);
		*this = DirectX::XMVectorLerp(v1, v2, t);
		return *this;
	}

	// Catmull-Romスプライン補間
	KdVec2& CatmullRom(const KdVec2& v1, const KdVec2& v2, const KdVec2& v3, const KdVec2& v4, float t)
	{
		*this = DirectX::XMVectorCatmullRom(v1, v2, v3, v4, t);
		return *this;
	}

	// エルミートスプライン補間
	KdVec2& Hermite(const KdVec2& v1, const KdVec2& t1, const KdVec2& v2, const KdVec2& t2, float t)
	{
		*this = DirectX::XMVectorHermite(v1, t1, v2, t2, t);
		return *this;
	}

	// 行列で変換し、w=1に投影する。
	KdVec2& TransformCoord(const DirectX::XMMATRIX& m)
	{
		*this = XMVector2TransformCoord(*this, m);
		return *this;
	}

	// (x,y,0,0)で変換
	KdVec2& TransformNormal(const DirectX::XMMATRIX& m)
	{
		*this = XMVector2TransformNormal(*this, m);
		return *this;
	}

	// 配列化
	std::vector<float> ToArray() const
	{
		return { x, y};
	}

	// float配列の先頭アドレスとして返す
	operator const float* () const
	{
		return &x;
	}
	operator float* ()
	{
		return &x;
	}
};

//=========================================
//
// 3Dベクトル
//
//=========================================
class KdVec3 : public DirectX::XMFLOAT3
{
public:
	// コンストラクタで0埋め
	KdVec3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	// 座標指定付きコンストラクタ
	KdVec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	// XMVECTORから代入してきた時
	KdVec3(const DirectX::XMVECTOR& V)
	{
		// 変換して代入
		DirectX::XMStoreFloat3(this, V);
		// 下記と同意だが、上記のSIMD命令を使用したほうが高速
//		x = V.m128_f32[0];
//		y = V.m128_f32[1];
//		z = V.m128_f32[2];
	}
	// XMFLOAT3から代入してきた時
	KdVec3(const DirectX::XMFLOAT3& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
	}

	// XMVECTORへ変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(this); }
	// Math::Vector3と互換性を持つための関数
	operator Math::Vector3&() { return *(Math::Vector3*)this; }

	// 演算子
	KdVec3& operator+= (const KdVec3& v)
	{
		*this = DirectX::XMVectorAdd(*this, v);
		return *this;
	}

	KdVec3& operator-= (const KdVec3& v)
	{
		*this = DirectX::XMVectorSubtract(*this, v);
		return *this;
	}

	KdVec3& operator*= (const KdVec3& v)
	{
		*this = DirectX::XMVectorMultiply(*this, v);
		return *this;
	}

	KdVec3& operator*= (float s)
	{
		*this = DirectX::XMVectorScale(*this, s);
		return *this;
	}

	KdVec3& operator/= (float s)
	{
		assert(s != 0.0f);
		*this = DirectX::XMVectorScale(*this, 1.f / s);
		return *this;
	}

	// セット
	KdVec3& Set(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
		return *this;
	}

	// 指定方向に、angleだけ向ける
	KdVec3& LookTo(const KdVec3& dir, float angle);

	// 長さ
	float Length() const
	{
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}
	// 長さの２乗(高速なので判定用に使用することが多い)
	float LengthSquared() const
	{
		return DirectX::XMVector3LengthSq(*this).m128_f32[0];
	}

	// 内積
	float Dot(const KdVec3& v) const
	{
		return DirectX::XMVector3Dot(*this, v).m128_f32[0];
	}
	// 内積
	static float sDot(const KdVec3& v1, const KdVec3& v2)
	{
		return DirectX::XMVector3Dot(v1, v2).m128_f32[0];
	}

	// 外積
	KdVec3 Cross(const KdVec3& v)
	{
		return DirectX::XMVector3Cross(*this, v);
	}

	// 外積
	static KdVec3 sCross(const KdVec3& v1, const KdVec3& v2)
	{
		return DirectX::XMVector3Cross(v1, v2);
	}

	// 自分を正規化
	KdVec3& Normalize()
	{
		*this = DirectX::XMVector3Normalize(*this);
		return *this;
	}

	// クランプ
	KdVec3& Clamp(const KdVec3& vmin, const KdVec3& vmax)
	{
		*this = DirectX::XMVectorClamp(*this, vmin, vmax);
		return *this;
	}

	// 線形補間
	KdVec3& Lerp(const KdVec3& v1, const KdVec3& v2, float t)
	{
		*this = DirectX::XMVectorLerp(v1, v2, t);
		return *this;
	}

	// スムーズステップ補間
	KdVec3& SmoothStep(const KdVec3& v1, const KdVec3& v2, float t)
	{
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // 値を0～1にクランプ
		t = t * t * (3.f - 2.f * t);
		*this = DirectX::XMVectorLerp(v1, v2, t);
		return *this;
	}

	// Catmull-Romスプライン補間
	KdVec3& CatmullRom(const KdVec3& v1, const KdVec3& v2, const KdVec3& v3, const KdVec3& v4, float t)
	{
		*this = DirectX::XMVectorCatmullRom(v1, v2, v3, v4, t);
		return *this;
	}

	// エルミネートスプライン補間
	KdVec3& Hermite(const KdVec3& v1, const KdVec3& t1, const KdVec3& v2, const KdVec3& t2, float t)
	{
		*this = DirectX::XMVectorHermite(v1, t1, v2, t2, t);
		return *this;
	}

	// クォータニオンで変換
	KdVec3& Transform(const KdQuaternion& quat);

	// 行列で変換し、w=1に投影する。
	KdVec3& TransformCoord(const DirectX::XMMATRIX& m)
	{
		*this = DirectX::XMVector3TransformCoord(*this, m);
		return *this;
	}

	// (x,y,z,0)で変換
	KdVec3& TransformNormal(const DirectX::XMMATRIX& m)
	{
		*this = DirectX::XMVector3TransformNormal(*this, m);
		return *this;
	}

	// 配列化
	std::vector<float> ToArray() const
	{
		return { x, y, z };
	}
	// Vector4化
	KdVec4 ToVec4(float w);

	// float配列の先頭アドレスとして返す
	operator const float* () const
	{
		return &x;
	}
	operator float* ()
	{
		return &x;
	}
};

//=========================================
//
// 4Dベクトル
//
//=========================================
class KdVec4 : public DirectX::XMFLOAT4
{
public:
	// コンストラクタで0埋め
	KdVec4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	// 座標指定付きコンストラクタ
	KdVec4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	// XMVECTORから代入してきた時
	KdVec4(const DirectX::XMVECTOR& V)
	{
		// 変換して代入
		DirectX::XMStoreFloat4(this, V);
		// 下記と同意だが、上記のSIMD命令を使用したほうが高速
//		x = V.m128_f32[0];
//		y = V.m128_f32[1];
//		z = V.m128_f32[2];
//		w = V.m128_f32[3];
	}
	// XMFLOAT4から代入してきた時
	KdVec4(const DirectX::XMFLOAT4& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		w = V.w;
	}

	// XMVECTORへ変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }
	// Math::Vector3と互換性を持つための関数
	operator Math::Vector4& () { return *(Math::Vector4*)this; }

	// 演算子
	KdVec4& operator+= (const KdVec4& v)
	{
		*this = DirectX::XMVectorAdd(*this, v);
		return *this;
	}

	KdVec4& operator-= (const KdVec4& v)
	{
		*this = DirectX::XMVectorSubtract(*this, v);
		return *this;
	}

	KdVec4& operator*= (const KdVec4& v)
	{
		*this = DirectX::XMVectorMultiply(*this, v);
		return *this;
	}

	KdVec4& operator*= (float s)
	{
		*this = DirectX::XMVectorScale(*this, s);
		return *this;
	}

	KdVec4& operator/= (float s)
	{
		assert(s != 0.0f);
		*this = DirectX::XMVectorScale(*this, 1.f / s);
	}

	// セット
	void Set(float fx, float fy, float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	void Set(const KdVec3& v3, float fw)
	{
		x = v3.x;
		y = v3.y;
		z = v3.z;
		w = fw;
	}

	// 長さ
	float Length() const
	{
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}
	// 長さの２乗(高速なので判定用に使用することが多い)
	float LengthSquared() const
	{
		return DirectX::XMVector3LengthSq(*this).m128_f32[0];
	}

	// 内積
	float Dot(const KdVec4& V) const
	{
		return DirectX::XMVector4Dot(*this, V).m128_f32[0];
	}
	// 内積
	static float Dot(const KdVec4& v1, const KdVec4& v2)
	{
		return DirectX::XMVector4Dot(v1, v2).m128_f32[0];
	}

	// 外積
	KdVec4 Cross(const KdVec4& v1, KdVec4& v2)
	{
		return DirectX::XMVector4Cross(*this, v1, v2);
	}

	// 外積
	static KdVec4 Cross(const KdVec4& v1, KdVec4& v2, const KdVec4& v3)
	{
		return DirectX::XMVector4Cross(v1, v2, v3);
	}

	// 自分を正規化
	KdVec4& Normalize()
	{
		*this = DirectX::XMVector3Normalize(*this);
		return *this;
	}

	// クランプ
	KdVec4& Clamp(const KdVec4& vmin, const KdVec4& vmax)
	{
		*this = DirectX::XMVectorClamp(*this, vmin, vmax);
		return *this;
	}

	// 線形補間
	KdVec4& Lerp(const KdVec4& v1, const KdVec4& v2, float t)
	{
		*this = DirectX::XMVectorLerp(v1, v2, t);
		return *this;
	}

	// スムーズステップ補間
	KdVec4& SmoothStep(const KdVec4& v1, const KdVec4& v2, float t)
	{
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // 値を0～1にクランプ
		t = t * t * (3.f - 2.f * t);
		*this = DirectX::XMVectorLerp(v1, v2, t);
		return *this;
	}

	// Catmull-Romスプライン補間
	KdVec4& CatmullRom(const KdVec4& v1, const KdVec4& v2, const KdVec4& v3, const KdVec4& v4, float t)
	{
		*this = DirectX::XMVectorCatmullRom(v1, v2, v3, v4, t);
		return *this;
	}

	// エルミネートスプライン補間
	KdVec4& Hermite(const KdVec4& v1, const KdVec4& t1, const KdVec4& v2, const KdVec4& t2, float t)
	{
		*this = DirectX::XMVectorHermite(v1, t1, v2, t2, t);
		return *this;
	}

	// 変換
	KdVec4& Transform(const DirectX::XMMATRIX& m)
	{
		*this = DirectX::XMVector4Transform(*this, m);
		return *this;
	}

	// 配列化
	std::vector<float> ToArray() const
	{
		return { x, y, z, w };
	}

	// float配列の先頭アドレスとして返す
	operator const float* () const
	{
		return &x;
	}
	operator float* ()
	{
		return &x;
	}
};

//=========================================
//
// クォータニオン
//
//=========================================
class KdQuaternion : public DirectX::XMFLOAT4
{
public:

	// コンストラクタで0埋め
	KdQuaternion()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}
	// 座標指定付きコンストラクタ
	KdQuaternion(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	// XMVECTORから代入してきた時
	KdQuaternion(const DirectX::XMVECTOR& V)
	{
		// 変換して代入
		DirectX::XMStoreFloat4(this, V);
	}
	// XMFLOAT3から代入してきた時
	KdQuaternion(const DirectX::XMFLOAT4& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		w = V.w;
	}

	// XMVECTORへ変換
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }
	// Math::Quaternionと互換性を持つための関数
	operator Math::Quaternion& () { return *(Math::Quaternion*)this; }

	// 演算子
	KdQuaternion& operator*= (const KdQuaternion& q)
	{
		*this = DirectX::XMQuaternionMultiply(*this, q);
		return *this;
	}
	KdQuaternion& operator/= (const KdQuaternion& q)
	{
		*this = DirectX::XMQuaternionMultiply(*this, DirectX::XMQuaternionInverse(q));
		return *this;
	}

	//=================================
	//
	// 作成
	//
	//=================================

	// 指定軸回転クォータニオン作成
	KdQuaternion& CreateRotationAxis(const KdVec3& axis, float angle)
	{
		*this = DirectX::XMQuaternionRotationAxis(axis, angle);
		return *this;
	}

	// x,y,z回転角度からクォータニオン作成
	KdQuaternion& CreateRotationFromAngles(float angleX, float angleY, float angleZ)
	{
		*this = DirectX::XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ);
		return *this;
	}

	// 行列から作成
	KdQuaternion& CreateFromMatrix(const Math::Matrix& m);

	//=================================
	//
	// 操作
	//
	//=================================

	// 共役化
	// (小ネタ)クォータニオンの長さが１の場合は、逆クォータニオンと等しくなる
	KdQuaternion& Conjugate()
	{
		*this = DirectX::XMQuaternionConjugate(*this);
		return *this;
	}

	// 逆クォータニオン化
	KdQuaternion& Inverse()
	{
		*this = DirectX::XMQuaternionInverse(*this);
		return *this;
	}

	// 正規化
	KdQuaternion& Normalize()
	{
		*this = DirectX::XMQuaternionNormalize(*this);
		return *this;
	}

	// 球面線形補間
	KdQuaternion& Slerp(const KdQuaternion& q1, const KdQuaternion& q2, float t)
	{
		*this = XMQuaternionSlerp(q1, q2, t);
		return *this;
	}

	// Z軸を指定方向に向ける
	KdQuaternion& LookTo(const KdVec3& dir, const KdVec3& up);

	//=================================
	//
	// プロパティ
	//
	//=================================

	// クォータニオンの大きさ
	float Length() const
	{
		return DirectX::XMQuaternionLength(*this).m128_f32[0];
	}


	// 配列化
	std::vector<float> ToArray() const
	{
		return { x, y, z, w };
	}

	// X軸取得
	KdVec3 Right()
	{
		return {
			1 - 2 * y * y - 2 * z * z,
			2 * x * y + 2 * w * z,
			2 * x * z - 2 * w * y
		};
	}

	// Y軸取得
	KdVec3 Up()
	{
		return {
			2 * x * y - 2 * w * z,
			1 - 2 * x * x - 2 * z * z,
			2 * y * z + 2 * w * x
		};
	}

	// Z軸取得
	KdVec3 Forward()
	{
		return {
			2 * x * z + 2 * w * y,
			2 * y * z - 2 * w * x,
			1 - 2 * x * x - 2 * y * y
		};
	}

	// float配列の先頭アドレスとして返す
	operator const float* () const
	{
		return &x;
	}
	operator float* ()
	{
		return &x;
	}
};

//=========================================
//
// 4x4 行列
//
//=========================================
class KdMatrix : public DirectX::XMFLOAT4X4
{
public:
	// デフォルトコンストラクタは単位行列
	KdMatrix()
	{
		*this = DirectX::XMMatrixIdentity();
	}

	// XMMATRIXから代入してきた
	KdMatrix(const DirectX::XMMATRIX& M)
	{
		DirectX::XMStoreFloat4x4(this, M);
	}

	// XMFLOAT4X4から代入してきた
	KdMatrix(const DirectX::XMFLOAT4X4& M)
	{
		memcpy_s(this, sizeof(float) * 16, &M, sizeof(XMFLOAT4X4));
	}

	// XMMATRIXへ変換
	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(this);
	}
	// Math::Matrixと互換性を持つための関数
	operator Math::Matrix& () { return *(Math::Matrix*)this; }

	// 代入演算子
	KdMatrix& operator*= (const KdMatrix& M)
	{
		*this = DirectX::XMMatrixMultiply(*this, M);
		return *this;
	}

	//=================================
	//
	// 作成
	//
	//=================================

	// 単位行列にする
	KdMatrix& CreateIdentity()
	{
		*this = DirectX::XMMatrixIdentity();
		return *this;
	}

	// 移動行列作成
	KdMatrix& CreateTranslation(float x, float y, float z)
	{
		*this = DirectX::XMMatrixTranslation(x, y, z);
		return *this;
	}
	// 移動行列作成
	KdMatrix& CreateTranslation(const KdVec3& pos)
	{
		*this = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		return *this;
	}
	// X回転行列作成
	KdMatrix& CreateRotationX(float angle)
	{
		*this = DirectX::XMMatrixRotationX(angle);
		return *this;
	}
	// Y回転行列作成
	KdMatrix& CreateRotationY(float angle)
	{
		*this = DirectX::XMMatrixRotationY(angle);
		return *this;
	}
	// Z回転行列作成
	KdMatrix& CreateRotationZ(float angle)
	{
		*this = DirectX::XMMatrixRotationZ(angle);
		return *this;
	}
	// 指定軸回転行列作成
	KdMatrix& CreateRotationAxis(const KdVec3& axis, float angle)
	{
		*this = DirectX::XMMatrixRotationAxis(axis, angle);
		return *this;
	}
	// クォータニオンから回転行列作成
	KdMatrix& CreateFromQuaternion(const Math::Quaternion& quat)
	{
		*this = DirectX::XMMatrixRotationQuaternion(quat);
		return *this;
	}

	// 拡大、回転、座標から、行列を作成する
	KdMatrix& CreateFromSRT(const KdVec3& scale, const KdQuaternion& rotation, const KdVec3& translation)
	{
		*this = DirectX::XMMatrixScalingFromVector(scale) * DirectX::XMMatrixRotationQuaternion(rotation) * DirectX::XMMatrixTranslationFromVector(translation);
		return *this;
	}

	// 拡縮行列作成
	KdMatrix& CreateScaling(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x, y, z);
		return *this;
	}
	KdMatrix& CreateScaling(const KdVec3& scale)
	{
		*this = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		return *this;
	}

	// 透視影行列作成
	KdMatrix& CreateProjection_PerspectiveFov(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		return *this;
	}

	// 正射影行列作成
	KdMatrix& CreateProjection_Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
		return *this;
	}


	//=================================
	//
	// 操作
	//
	//=================================

	// 逆行列にする
	void Inverse()
	{
		*this = DirectX::XMMatrixInverse(nullptr, *this);
	}

	// 移動
	KdMatrix& Move(float x, float y, float z)
	{
		_41 += x;
		_42 += y;
		_43 += z;
		return *this;
	}
	KdMatrix& Move(const KdVec3& v)
	{
		_41 += v.x;
		_42 += v.y;
		_43 += v.z;
		return *this;
	}

	// X回転
	KdMatrix& RotateX(float angle)
	{
		*this *= DirectX::XMMatrixRotationX(angle);
		return *this;
	}
	// Y回転
	KdMatrix& RotateY(float angle)
	{
		*this *= DirectX::XMMatrixRotationY(angle);
		return *this;
	}
	// Z回転
	KdMatrix& RotateZ(float angle)
	{
		*this *= DirectX::XMMatrixRotationZ(angle);
		return *this;
	}
	// 指定軸回転
	KdMatrix& RotateAxis(const KdVec3& axis, float angle)
	{
		*this *= DirectX::XMMatrixRotationAxis(axis, angle);
		return *this;
	}

	// 拡縮
	KdMatrix& Scale(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x, y, z);
		return *this;
	}
	KdMatrix& Scale(const KdVec3& v)
	{
		*this = DirectX::XMMatrixScaling(v.x, v.y, v.z);
		return *this;
	}

	// 合成
	KdMatrix& Multiply(const KdMatrix& m)
	{
		*this *= m;
		return *this;
	}

	// Z軸を指定方向に向ける
	KdMatrix& LookTo(const KdVec3& dir, const KdVec3& up = { 0, 1, 0 })
	{
		KdVec3 vZ = dir;
		vZ.Normalize();
		KdVec3 vX = KdVec3::sCross(up, vZ).Normalize();
		KdVec3 vY = KdVec3::sCross(vZ, vX).Normalize();

		float scaleX = Right().Length();
		float scaleY = Up().Length();
		float scaleZ = Forward().Length();

		Right(vX * scaleX);
		Up(vY * scaleY);
		Forward(vZ * scaleZ);

		return *this;
	}

	// 拡大を正規化
	KdMatrix& NormalizeScale()
	{
		KdVec3 v;
		v = Right();
		Right(v.Normalize());
		v = Up();
		Up(v.Normalize());
		v = Forward();
		Forward(v.Normalize());

		return *this;
	}


	//=================================
	//
	// プロパティ
	//
	//=================================

	// X軸取得
	KdVec3 Right() const { return { _11, _12, _13 }; }
	// X軸セット
	void Right(const KdVec3& v)
	{
		_11 = v.x;
		_12 = v.y;
		_13 = v.z;
	}
	// Y軸取得
	KdVec3 Up() const { return { _21, _22, _23 }; }
	// Y軸セット
	void Up(const KdVec3& v)
	{
		_21 = v.x;
		_22 = v.y;
		_23 = v.z;
	}
	// Z軸取得
	KdVec3 Forward() const { return { _31, _32, _33 }; }
	// Z軸セット
	void Forward(const KdVec3& v)
	{
		_31 = v.x;
		_32 = v.y;
		_33 = v.z;
	}

	// 座標取得
	KdVec3 Translation() const { return { _41, _42, _43 }; }
	// 座標セット
	void Translation(const KdVec3& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}

	// スケール取得
	KdVec3 GetScale() const
	{
		return {
			Right().Length(),
			Up().Length(),
			Forward().Length()
		};
	}

	// スケースセット
	void SetScale(const KdVec3& v)
	{
		NormalizeScale();
		Right(Right() * v.x);
		Up(Up() * v.y);
		Forward(Forward() * v.z);
	}

	// XYZの順番で合成したときの、回転角度を算出する
	KdVec3 ComputeAngles() const
	{
		KdMatrix mat = *this;
		mat.NormalizeScale();

		KdVec3 angles;
		angles.x = atan2f(mat.m[1][2], mat.m[2][2]);
		angles.y = atan2f(-mat.m[0][2], sqrtf(mat.m[1][2] * mat.m[1][2] + mat.m[2][2] * mat.m[2][2]));
		angles.z = atan2f(mat.m[0][1], mat.m[0][0]);
		return angles;
	}

	// 行列を、拡大、回転、座標へ分解する
	bool Decompose(KdVec3& scale, KdQuaternion& rotation, KdVec3& translation) const
	{
		DirectX::XMVECTOR s, r, t;

		if (!DirectX::XMMatrixDecompose(&s, &r, &t, *this))
		{
			return false;
		}

		scale = s;
		rotation = r;
		translation = t;
		return true;
	}

	// 配列化
	std::vector<float> ToArray() const
	{
		return {	_11, _12, _13, _14,
					_21, _22, _23, _24,
					_31, _32, _33, _34,
					_41, _42, _43, _44
				};
	}

	// float配列の先頭アドレスとして返す
	operator const float* () const
	{
		return &_11;
	}
	operator float* ()
	{
		return &_11;
	}
};

#include "KdMath.inl"
