#include "Vector4.h"

Vector4::Vector4(void)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

Vector4::Vector4(const Vector2& v, float z, float w)
{
	x = v.x;
	y = v.y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4(const Vector3& v, float w)
{
	x = v.x;
	y = v.y;
	z = v.z;
	this->w = w;
}

Vector4::Vector4(const Vector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

Vector4::Vector4(float vx, float vy, float vz)
{
	x = vx;
	y = vy;
	z = vz;
	w = 1.0f;
}

Vector4::Vector4(float vx, float vy, float vz, float vw)
{
	x = vx;
	y = vy;
	z = vz;
	w = vw;
}

bool Vector4::operator == (const Vector4& v) const
{
	if ((FLOAT_EQUAL(x, v.x) == false) ||
		(FLOAT_EQUAL(y, v.y) == false) ||
		(FLOAT_EQUAL(z, v.z) == false) ||
		(FLOAT_EQUAL(w, v.w) == false))
	{
		return false;
	}

	return true;
}

bool Vector4::operator != (const Vector4& v) const
{
	if ((FLOAT_EQUAL(x, v.x) == false) ||
		(FLOAT_EQUAL(y, v.y) == false) ||
		(FLOAT_EQUAL(z, v.z) == false) ||
		(FLOAT_EQUAL(w, v.w) == false))
	{
		return true;
	}

	return false;
}

Vector4& Vector4::operator = (const Vector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;

	return *this;
}

Vector4& Vector4::operator += (const Vector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;

	return *this;
}

Vector4& Vector4::operator -= (const Vector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;

	return *this;
}

Vector4& Vector4::operator *= (const Vector4& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;

	return *this;
}

Vector4& Vector4::operator /= (const Vector4& v)
{
	x = FLOAT_DIV(x, v.x);
	y = FLOAT_DIV(y, v.y);
	z = FLOAT_DIV(z, v.z);
	w = FLOAT_DIV(w, v.w);

	return *this;
}

Vector4 Vector4::operator + (const Vector4& v) const
{
	Vector4 ret(*this);

	ret.x += v.x;
	ret.y += v.y;
	ret.z += v.z;
	ret.w += v.w;

	return ret;
}

Vector4 Vector4::operator - (const Vector4& v) const
{
	Vector4 ret(*this);

	ret.x -= v.x;
	ret.y -= v.y;
	ret.z -= v.z;
	ret.w -= v.w;

	return ret;
}

Vector4 Vector4::operator * (const Vector4& v) const
{
	Vector4 ret(*this);

	ret.x *= v.x;
	ret.y *= v.y;
	ret.z *= v.z;
	ret.w *= v.w;

	return ret;
}

Vector4 Vector4::operator / (const Vector4& v) const
{
	Vector4 ret(*this);

	ret.x = FLOAT_DIV(ret.x, v.x);
	ret.y = FLOAT_DIV(ret.y, v.y);
	ret.z = FLOAT_DIV(ret.z, v.z);
	ret.w = FLOAT_DIV(ret.w, v.w);

	return ret;
}

Vector4 Vector4::operator +(void) const
{
	return *this;
}

Vector4 Vector4::operator -(void) const
{
	return (*this * -1.0f);
}

Vector4 Vector4::operator * (float s) const
{
	Vector4 ret(*this);

	ret.x *= s;
	ret.y *= s;
	ret.z *= s;
	ret.w *= s;

	return ret;
}

Vector4 Vector4::operator / (float s) const
{
	Vector4 ret;

	if (FLOAT_IS_ZERO(s) == true)
	{
		ret.x = 0.0f;
		ret.y = 0.0f;
		ret.z = 0.0f;
		ret.w = 0.0f;
	}
	else
	{
		float invS = 1.0f / s;

		ret.x = x * invS;
		ret.y = y * invS;
		ret.z = z * invS;
		ret.w = w * invS;
	}

	return ret;
}

Vector4& Vector4::operator *= (float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;

	return *this;
}

Vector4& Vector4::operator /= (float s)
{
	if (FLOAT_IS_ZERO(s) == true)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}
	else
	{
		float invS = 1.0f / s;

		x *= invS;
		y *= invS;
		z *= invS;
		w *= invS;
	}

	return *this;
}

void Vector4::Set(float vx, float vy, float vz, float vw)
{
	x = vx;
	y = vy;
	z = vz;
	w = vw;
}

float Vector4::Normalize(void)
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

Vector4 Vector4::ToNormalize(float* pLength) const
{
	Vector4 tmp(*this);

	if (pLength == NULL)
	{
		tmp.Normalize();
	}
	else
	{
		*pLength = tmp.Normalize();
	}

	return tmp;
}

void Vector4::Saturate(void)
{
	x = CLAMP(x, 0.0f, 1.0f);
	y = CLAMP(y, 0.0f, 1.0f);
	z = CLAMP(z, 0.0f, 1.0f);
	w = CLAMP(w, 0.0f, 1.0f);
}

Vector4 Vector4::ToSaturate(void) const
{
	Vector4 tmp(*this);

	tmp.Saturate();

	return tmp;
}

float Vector4::GetLengthSqr(void) const
{
	return (x * x) + (y * y) + (z * z);
}

float Vector4::GetLength(void) const
{
	return ::sqrtf((x * x) + (y * y) + (z * z));
}

float Vector4::Dot(const Vector4& lv, const Vector4& rv)
{
	return (lv.x * rv.x) + (lv.y * rv.y) + (lv.z * rv.z) + (lv.w * rv.w);
}

Vector4 Vector4::Cross(const Vector4& lv, const Vector4& rv)
{
	float y2z3 = (lv.y * rv.z);
	float z2y3 = (lv.z * rv.y);
	float z2x3 = (lv.z * rv.x);
	float x2z3 = (lv.x * rv.z);
	float x2y3 = (lv.x * rv.y);
	float y2x3 = (lv.y * rv.x);

	return Vector4((y2z3 - z2y3), (z2x3 - x2z3), (x2y3 - y2x3), 1.0f);
}

Vector4 Vector4::Minimum(const Vector4& v0, const Vector4& v1)
{
	Vector4 v;

	v.x = MINIMUM(v0.x, v1.x);
	v.y = MINIMUM(v0.y, v1.y);
	v.z = MINIMUM(v0.z, v1.z);
	v.w = MINIMUM(v0.w, v1.w);

	return v;
}

Vector4 Vector4::Maximum(const Vector4& v0, const Vector4& v1)
{
	Vector4 v;

	v.x = MAXIMUM(v0.x, v1.x);
	v.y = MAXIMUM(v0.y, v1.y);
	v.z = MAXIMUM(v0.z, v1.z);
	v.w = MAXIMUM(v0.w, v1.w);

	return v;
}

Vector4 Vector4::Lerp(const Vector4& vs, const Vector4& ve, float t)
{
	float tt = CLAMP(t, 0.0f, 1.0f);

	return (vs * (1.0f - tt)) + (ve * tt);
}

Vector4 Vector4::Slerp(const Vector4& vs, const Vector4& ve, float t)
{
	Vector4 s = vs.ToNormalize();
	Vector4 e = ve.ToNormalize();

	float angle = acosf(MINIMUM(1.0f, Vector4::Dot(s, e)));
	float sinTh = sinf(angle);
	float invSinTh = FLOAT_RECIPROCAL(sinTh);

	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	return ((Ps * s + Pe * e) * invSinTh).ToNormalize();
}
