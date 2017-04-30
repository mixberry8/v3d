#include "Vector2.h"

Vector2::Vector2(void)
{
	x = 0.0f;
	y = 0.0f;
}

Vector2::Vector2(const Vector2& v)
{
	x = v.x;
	y = v.y;
}

Vector2::Vector2(float vx, float vy)
{
	x = vx;
	y = vy;
}

bool Vector2::operator == (const Vector2& v) const
{
	if ((FLOAT_EQUAL(x, v.x) == false) ||
		(FLOAT_EQUAL(y, v.y) == false))
	{
		return false;
	}

	return true;
}

bool Vector2::operator != (const Vector2& v) const
{
	if ((FLOAT_EQUAL(x, v.x) == false) ||
		(FLOAT_EQUAL(y, v.y) == false))
	{
		return true;
	}

	return false;
}

Vector2& Vector2::operator = (const Vector2& v)
{
	x = v.x;
	y = v.y;

	return *this;
}

Vector2& Vector2::operator += (const Vector2& v)
{
	x += v.x;
	y += v.y;

	return *this;
}

Vector2& Vector2::operator -= (const Vector2& v)
{
	x -= v.x;
	y -= v.y;

	return *this;
}

Vector2& Vector2::operator *= (const Vector2& v)
{
	x *= v.x;
	y *= v.y;

	return *this;
}

Vector2& Vector2::operator /= (const Vector2& v)
{
	x = FLOAT_DIV(x, v.x);
	y = FLOAT_DIV(y, v.y);

	return *this;
}

Vector2 Vector2::operator + (const Vector2& v) const
{
	Vector2 ret(*this);

	ret.x += v.x;
	ret.y += v.y;

	return ret;
}

Vector2 Vector2::operator - (const Vector2& v) const
{
	Vector2 ret(*this);

	ret.x -= v.x;
	ret.y -= v.y;

	return ret;
}

Vector2 Vector2::operator * (const Vector2& v) const
{
	Vector2 ret(*this);

	ret.x *= v.x;
	ret.y *= v.y;

	return ret;
}

Vector2 Vector2::operator / (const Vector2& v) const
{
	Vector2 ret(*this);

	ret.x = FLOAT_DIV(ret.x, v.x);
	ret.y = FLOAT_DIV(ret.y, v.y);

	return ret;
}

Vector2 Vector2::operator +(void) const
{
	return *this;
}

Vector2 Vector2::operator -(void) const
{
	return (*this * -1.0f);
}

Vector2 Vector2::operator * (float s) const
{
	Vector2 ret(*this);

	ret.x *= s;
	ret.y *= s;

	return ret;
}

Vector2 Vector2::operator / (float s) const
{
	Vector2 ret;

	if (FLOAT_IS_ZERO(s) == true)
	{
		ret.x = 0.0f;
		ret.y = 0.0f;
	}
	else
	{
		float invS = 1.0f / s;

		ret.x = x * invS;
		ret.y = y * invS;
	}

	return ret;
}

Vector2& Vector2::operator *= (float s)
{
	x *= s;
	y *= s;

	return *this;
}

Vector2& Vector2::operator /= (float s)
{
	if (FLOAT_IS_ZERO(s) == true)
	{
		x = 0.0f;
		y = 0.0f;
	}
	else
	{
		float invS = 1.0f / s;

		x *= invS;
		y *= invS;
	}

	return *this;
}

void Vector2::Set(float vx, float vy)
{
	x = vx;
	y = vy;
}

float Vector2::Normalize(void)
{
	float length = GetLength();

	if (FLOAT_IS_ZERO(length) == true)
	{
		x = 0.0f;
		y = 0.0f;
	}
	else
	{
		float invLength = (1.0f / length);

		x *= invLength;
		y *= invLength;
	}

	return length;
}

Vector2 Vector2::ToNormalize(float* pLength) const
{
	Vector2 ret(*this);

	if (pLength == nullptr)
	{
		ret.Normalize();
	}
	else
	{
		*pLength = ret.Normalize();
	}

	return ret;
}

void Vector2::Saturate(void)
{
	x = CLAMP(x, 0.0f, 1.0f);
	y = CLAMP(y, 0.0f, 1.0f);
}

Vector2 Vector2::ToSaturate(void) const
{
	Vector2 ret(*this);

	ret.Saturate();

	return ret;
}

float Vector2::GetLengthSqr(void) const
{
	return (x * x) + (y * y);
}

float Vector2::GetLength(void) const
{
	return sqrtf((x * x) + (y * y));
}

float Vector2::GetLengthF(void) const
{
	return sqrtf((x * x) + (y * y));
}

float Vector2::Dot(const Vector2& l, const Vector2& r)
{
	return (l.x * r.x) + (l.y * r.y);
}

float Vector2::Cross(const Vector2& l, const Vector2& r)
{
	float x2y3 = (l.x * r.y);
	float y2x3 = (l.y * r.x);

	return x2y3 - y2x3;
}

Vector2 Vector2::Minimum(const Vector2& v0, const Vector2& v1)
{
	Vector2 v;

	v.x = MINIMUM(v0.x, v1.x);
	v.y = MINIMUM(v0.y, v1.y);

	return v;
}

Vector2 Vector2::Maximum(const Vector2& v0, const Vector2& v1)
{
	Vector2 v;

	v.x = MAXIMUM(v0.x, v1.x);
	v.y = MAXIMUM(v0.y, v1.y);

	return v;
}

Vector2 Vector2::Lerp(const Vector2& vs, const Vector2& ve, float t)
{
	float tt = CLAMP(t, 0.0f, 1.0f);

	return (vs * (1.0f - tt)) + (ve * tt);
}

Vector2 Vector2::Slerp(const Vector2& vs, const Vector2& ve, float t)
{
	Vector2 s = vs.ToNormalize();
	Vector2 e = ve.ToNormalize();

	float angle = acosf(MINIMUM(1.0f, Dot(s, e)));
	float sinTh = sinf(angle);
	float invSinTh = FLOAT_RECIPROCAL(sinTh);

	float Ps = sinf(angle * (1.0f - t));
	float Pe = sinf(angle * t);

	return ((Ps * s + Pe * e) * invSinTh).ToNormalize();
}
