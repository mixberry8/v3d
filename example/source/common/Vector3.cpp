#include "Vector3.h"

Vector3::Vector3(void)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Vector3::Vector3(const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector3::Vector3(float vx, float vy, float vz)
{
	x = vx;
	y = vy;
	z = vz;
}

bool Vector3::operator == (const Vector3& v) const
{
	if ((FLOAT_EQUAL(x, v.x) == false) ||
		(FLOAT_EQUAL(y, v.y) == false) ||
		(FLOAT_EQUAL(z, v.z) == false))
	{
		return false;
	}

	return true;
}

bool Vector3::operator != (const Vector3& v) const
{
	if ((FLOAT_EQUAL(x, v.x) == false) ||
		(FLOAT_EQUAL(y, v.y) == false) ||
		(FLOAT_EQUAL(z, v.z) == false))
	{
		return true;
	}

	return false;
}

Vector3& Vector3::operator = (const Vector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}

Vector3& Vector3::operator += (const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vector3& Vector3::operator -= (const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

Vector3& Vector3::operator *= (const Vector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

Vector3& Vector3::operator /= (const Vector3& v)
{
	x = FLOAT_DIV(x, v.x);
	y = FLOAT_DIV(y, v.y);
	z = FLOAT_DIV(z, v.z);

	return *this;
}

Vector3 Vector3::operator + (const Vector3& v) const
{
	Vector3 ret(*this);

	ret.x += v.x;
	ret.y += v.y;
	ret.z += v.z;

	return ret;
}

Vector3 Vector3::operator - (const Vector3& v) const
{
	Vector3 ret(*this);

	ret.x -= v.x;
	ret.y -= v.y;
	ret.z -= v.z;

	return ret;
}

Vector3 Vector3::operator * (const Vector3& v) const
{
	Vector3 ret(*this);

	ret.x *= v.x;
	ret.y *= v.y;
	ret.z *= v.z;

	return ret;
}

Vector3 Vector3::operator / (const Vector3& v) const
{
	Vector3 ret(*this);

	ret.x = FLOAT_DIV(ret.x, v.x);
	ret.y = FLOAT_DIV(ret.y, v.y);
	ret.z = FLOAT_DIV(ret.z, v.z);

	return ret;
}

Vector3 Vector3::operator +(void) const
{
	return *this;
}

Vector3 Vector3::operator -(void) const
{
	return (*this * -1.0f);
}

Vector3 Vector3::operator * (float s) const
{
	Vector3 ret(*this);

	ret.x *= s;
	ret.y *= s;
	ret.z *= s;

	return ret;
}

Vector3 Vector3::operator / (float s) const
{
	Vector3 ret;

	if (FLOAT_IS_ZERO(s) == true)
	{
		ret.x = 0.0f;
		ret.y = 0.0f;
		ret.z = 0.0f;
	}
	else
	{
		float invS = 1.0f / s;

		ret.x = x * invS;
		ret.y = y * invS;
		ret.z = z * invS;
	}

	return ret;
}

Vector3& Vector3::operator *= (float s)
{
	x *= s;
	y *= s;
	z *= s;

	return *this;
}

Vector3& Vector3::operator /= (float s)
{
	if (FLOAT_IS_ZERO(s) == true)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	else
	{
		float invS = 1.0f / s;

		x *= invS;
		y *= invS;
		z *= invS;
	}

	return *this;
}

void Vector3::Set(float vx, float vy, float vz)
{
	x = vx;
	y = vy;
	z = vz;
}

float Vector3::Normalize(void)
{
	float length = GetLength();

	if (FLOAT_IS_ZERO(length) == true)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	else
	{
		float invLength = (1.0f / length);

		x *= invLength;
		y *= invLength;
		z *= invLength;
	}

	return length;
}

Vector3 Vector3::ToNormalize(float* pLength) const
{
	Vector3 ret(*this);

	if (pLength == NULL)
	{
		ret.Normalize();
	}
	else
	{
		*pLength = ret.Normalize();
	}

	return ret;
}

void Vector3::Saturate(void)
{
	x = CLAMP(x, 0.0f, 1.0f);
	y = CLAMP(y, 0.0f, 1.0f);
	z = CLAMP(z, 0.0f, 1.0f);
}

Vector3 Vector3::ToSaturate(void) const
{
	Vector3 ret(*this);

	ret.Saturate();

	return ret;
}

float Vector3::GetLengthSqr(void) const
{
	return (x * x) + (y * y) + (z * z);
}

float Vector3::GetLength(void) const
{
	return ::sqrtf((x * x) + (y * y) + (z * z));
}

float Vector3::Dot(const Vector3& l, const Vector3& r)
{
	return (l.x * r.x) + (l.y * r.y) + (l.z * r.z);
}

Vector3 Vector3::Cross(const Vector3& l, const Vector3& r)
{
	float y2z3 = (l.y * r.z);
	float z2y3 = (l.z * r.y);
	float z2x3 = (l.z * r.x);
	float x2z3 = (l.x * r.z);
	float x2y3 = (l.x * r.y);
	float y2x3 = (l.y * r.x);

	return Vector3((y2z3 - z2y3), (z2x3 - x2z3), (x2y3 - y2x3));
}

Vector3 Vector3::Minimum(const Vector3& v0, const Vector3& v1)
{
	Vector3 v;

	v.x = MINIMUM(v0.x, v1.x);
	v.y = MINIMUM(v0.y, v1.y);
	v.z = MINIMUM(v0.z, v1.z);

	return v;
}

Vector3 Vector3::Maximum(const Vector3& v0, const Vector3& v1)
{
	Vector3 v;

	v.x = MAXIMUM(v0.x, v1.x);
	v.y = MAXIMUM(v0.y, v1.y);
	v.z = MAXIMUM(v0.z, v1.z);

	return v;
}

Vector3 Vector3::Lerp(const Vector3& vs, const Vector3& ve, float t)
{
	float tt = CLAMP(t, 0.0f, 1.0f);

	return (vs * (1.0f - tt)) + (ve * tt);
}

Vector3 Vector3::Slerp(const Vector3& vs, const Vector3& ve, float t)
{
	Vector3 s = vs.ToNormalize();
	Vector3 e = ve.ToNormalize();

	float angle = acosf(MINIMUM(1.0f, Vector3::Dot(s, e)));
	float sinTh = sinf(angle);
	float invSinTh = FLOAT_RECIPROCAL(sinTh);

	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	return ((Ps * s + Pe * e) * invSinTh).ToNormalize();
}
