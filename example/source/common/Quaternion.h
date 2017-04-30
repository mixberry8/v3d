#pragma once

#include "Vector3.h"

class Quaternion
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		float data[4];
	};

public:
	Quaternion(void);
	Quaternion(const Quaternion& q);
	Quaternion(float qx, float qy, float qz, float qw);
	Quaternion(const Vector3& axis, float angle);
	~Quaternion(void);

public:
	void SetRotationAxis(const Vector3& axis, float angle);
	void RotationAxis(const Vector3& axis, float angle);

	Vector3 GetAxis(void) const;
	float GetAngle(void) const;

	void Normalize(void);
	Quaternion ToNormalize(void) const;

	void Inverse(void);
	Quaternion ToInverse(void) const;

public:
	bool operator == (const Quaternion& q) const;
	bool operator != (const Quaternion& q) const;

	Quaternion& operator = (const Quaternion& q);
	Quaternion& operator += (const Quaternion& q);
	Quaternion& operator -= (const Quaternion& q);
	Quaternion& operator *= (const Quaternion& q);
	Quaternion& operator *= (float v);

	Quaternion operator + (const Quaternion& q) const;
	Quaternion operator - (const Quaternion& q) const;
	Quaternion operator * (const Quaternion& q) const;
	Quaternion operator * (float v) const;

	Quaternion operator -(void) const;

public:
	static float Dot(const Quaternion& l, const Quaternion& r);
	static Quaternion Cross(const Quaternion& l, const Quaternion& r);

	static Quaternion Difference(const Quaternion& from, const Quaternion& to);

	static Quaternion Slerp(const Quaternion& qs, const Quaternion& qd, float t);

public:
	static const Quaternion& identity(void)
	{
		static const Quaternion identity(0.0f, 0.0f, 0.0f, 1.0f);

		return identity;
	}
};
