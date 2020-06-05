#include "KdMath.h"


inline KdVec3& KdVec3::LookTo(const KdVec3& dir, float angle)
{
	// Šp“x
	float dot = Dot(dir);
	dot = std::clamp(dot, -1.0f, 1.0f);
	float ang = acos(dot);
	// Šp“x§ŒÀ
	if (ang > angle)ang = angle;
	// ‰ñ“]²
	KdVec3 vCrs = KdVec3::sCross(*this, dir);
	vCrs.Normalize();
	if (vCrs.Length() == 0)vCrs.y = 1;
	// ‰ñ“]
	KdQuaternion qRota;
	qRota.CreateRotationAxis(vCrs, ang);
	Transform(qRota);

	return *this;
}

inline KdVec3& KdVec3::Transform(const KdQuaternion& quat)
{
	*this = DirectX::XMVector3Rotate(*this, quat);
	return *this;
}

inline KdVec4 KdVec3::ToVec4(float w)
{
	return KdVec4(x, y, z, w);
}


inline KdVec3 operator+ (const KdVec3& v1, const KdVec3& v2)
{
	return DirectX::XMVectorAdd(v1, v2);
}

inline KdVec3 operator- (const KdVec3& v1, const KdVec3& v2)
{
	return DirectX::XMVectorSubtract(v1, v2);
}

inline KdVec3 operator* (const KdVec3& v1, const KdVec3& v2)
{
	return DirectX::XMVectorMultiply(v1, v2);
}

inline KdVec3 operator* (const KdVec3& v, float s)
{
	return DirectX::XMVectorScale(v, s);
}

inline KdVec3 operator/ (const KdVec3& v1, const KdVec3& v2)
{
	return DirectX::XMVectorDivide(v1, v2);
}

inline KdVec3 operator/ (const KdVec3& v, float s)
{
	return DirectX::XMVectorScale(v, 1.f / s);
}

inline KdVec3 operator* (float s, const KdVec3& v)
{
	return DirectX::XMVectorScale(v, s);
}

// KdQuaternion“¯m‚Ì‡¬
inline KdQuaternion operator* (const KdQuaternion& Q1, const KdQuaternion& Q2)
{
	return DirectX::XMQuaternionMultiply(Q1, Q2);
}

// Q1 * Q2‚Ì‹tƒNƒH[ƒ^ƒjƒIƒ“ ‚Ì‡¬
inline KdQuaternion operator/ (const KdQuaternion& Q1, const KdQuaternion& Q2)
{
	return DirectX::XMQuaternionMultiply(Q1, DirectX::XMQuaternionInverse(Q2));
}

inline KdQuaternion& KdQuaternion::CreateFromMatrix(const Math::Matrix& m)
{
	// s—ñ‚ÌŠg‘å¬•ª‚ğÁ‚·
	KdMatrix mTmp = m;
	mTmp.NormalizeScale();
	mTmp.Translation({ 0, 0, 0 });
	*this = DirectX::XMQuaternionRotationMatrix(mTmp);
	return *this;
}

inline KdQuaternion& KdQuaternion::LookTo(const KdVec3& dir, const KdVec3& up)
{
	KdMatrix m;
	m.LookTo(dir, up);
	CreateFromMatrix(m);
	return *this;
}

// KdMatrix“¯m‚Ì‡¬
inline KdMatrix operator* (const KdMatrix& M1, const KdMatrix& M2)
{
	return DirectX::XMMatrixMultiply(M1, M2);
}
