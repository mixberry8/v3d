#include "Quaternion.h"

Quaternion::Quaternion(void)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 1.0f;
}

Quaternion::Quaternion(const Quaternion& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

Quaternion::Quaternion(float qx, float qy, float qz, float qw)
{
	x = qx;
	y = qy;
	z = qz;
	w = qw;
}

Quaternion::Quaternion(const Vector3& axis, float angle)
{
	SetRotationAxis(axis, angle);
}

Quaternion::~Quaternion(void)
{
}

void Quaternion::SetRotationAxis(const Vector3& axis, float angle)
{
	float ha;
	float s;

	ha = (angle * 0.5f);
	s = ::sinf(ha);

	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	w = ::cosf(ha);
}

void Quaternion::RotationAxis(const Vector3& axis, float angle)
{
	float ha = (angle * 0.5f);
	float s = ::sinf(ha);

	float rx = axis.x * s;
	float ry = axis.y * s;
	float rz = axis.z * s;
	float rw = ::cosf(ha);

	float tx = x;
	float ty = y;
	float tz = z;
	float tw = w;

	x = rw * tx + rx * tw + ry * tz - rz * ty;
	y = rw * ty - rx * tz + ry * tw + rz * tx;
	z = rw * tz + rx * ty - ry * tx + rz * tw;
	w = rw * tw - rx * tx - ry * ty - rz * tz;
}

Vector3 Quaternion::GetAxis(void) const
{
	float sSqr = 1.0f - w * w;

	if (FLOAT_IS_ZERO(sSqr) == true)
	{
		return Vector3(1.0f, 0.0f, 0.0f);
	}

	float s = 1.0f / sSqr;

	return Vector3(x * s, y * s, z * s);
}

float Quaternion::GetAngle(void) const
{
	return ::acosf(w) * 2.0f;
}

void Quaternion::Normalize(void)
{
	float sq;

	sq = ::sqrtf(x * x + y * y + z * z + w * w);

	if (FLOAT_IS_ZERO(sq) == false)
	{
		float mag = (1.0f / sq);

		x *= mag;
		y *= mag;
		z *= mag;
		w *= mag;
	}
	else
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}
}

Quaternion Quaternion::ToNormalize(void) const
{
	Quaternion ret = *this;

	ret.Normalize();

	return ret;
}

void Quaternion::Inverse(void)
{
	float s = FLOAT_RECIPROCAL(x * x + y * y + z * z + w * w);

	x = -x * s;
	y = -y * s;
	z = -z * s;
	w = w * s;
}

Quaternion Quaternion::ToInverse(void) const
{
	Quaternion ret = *this;

	ret.Inverse();

	return ret;
}

bool Quaternion::operator == (const Quaternion& q) const
{
	if ((FLOAT_EQUAL(x, q.x) == false) ||
		(FLOAT_EQUAL(y, q.y) == false) ||
		(FLOAT_EQUAL(z, q.z) == false) ||
		(FLOAT_EQUAL(w, q.w) == false))
	{
		return false;
	}

	return true;
}

bool Quaternion::operator != (const Quaternion& q) const
{
	if ((FLOAT_EQUAL(x, q.x) == false) ||
		(FLOAT_EQUAL(y, q.y) == false) ||
		(FLOAT_EQUAL(z, q.z) == false) ||
		(FLOAT_EQUAL(w, q.w) == false))
	{
		return true;
	}

	return false;
}

Quaternion Quaternion::operator + (const Quaternion& q) const
{
	Quaternion quat(*this);

	quat += q;

	return quat;
}

Quaternion Quaternion::operator - (const Quaternion& q) const
{
	Quaternion quat(*this);

	quat -= q;

	return quat;
}

Quaternion Quaternion::operator * (const Quaternion& q) const
{
	Quaternion quat(*this);

	quat *= q;

	return quat;
}

Quaternion Quaternion::operator * (float v) const
{
	Quaternion quat(*this);

	quat *= v;

	return quat;
}

Quaternion& Quaternion::operator = (const Quaternion& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;

	return *this;
}

Quaternion& Quaternion::operator += (const Quaternion& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;

	return *this;
}

Quaternion& Quaternion::operator -= (const Quaternion& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;

	return *this;
}

Quaternion& Quaternion::operator *= (const Quaternion& q)
{
	float tx = x;
	float ty = y;
	float tz = z;
	float tw = w;

	x = q.w * tx + q.x * tw + q.y * tz - q.z * ty;
	y = q.w * ty - q.x * tz + q.y * tw + q.z * tx;
	z = q.w * tz + q.x * ty - q.y * tx + q.z * tw;
	w = q.w * tw - q.x * tx - q.y * ty - q.z * tz;

	return *this;
}

Quaternion& Quaternion::operator *= (float v)
{
	x *= v;
	y *= v;
	z *= v;
	w *= v;

	return *this;
}

Quaternion Quaternion::operator -(void) const
{
	Quaternion ret;

	ret.x = -x;
	ret.y = -y;
	ret.z = -z;
	ret.w = -w;

	return ret;
}

float Quaternion::Dot(const Quaternion& l, const Quaternion& r)
{
	return (l.x * r.x) + (l.y * r.y) + (l.z * r.z) + (l.w * r.w);
}

Quaternion Quaternion::Cross(const Quaternion& l, const Quaternion& r)
{
	return Quaternion();
}

Quaternion Quaternion::Difference(const Quaternion& from, const Quaternion& to)
{
	return to * from.ToInverse();
}

Quaternion Quaternion::Slerp(const Quaternion& qs, const Quaternion& qd, float t)
{
	float d = Quaternion::Dot(qs, qd);
	float e = (d < 0.0f) ? -1.0f : 1.0f;

	return qs * (1.0f - t) + qd * (e * t);
}
