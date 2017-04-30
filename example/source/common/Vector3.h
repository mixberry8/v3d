#pragma once

class Vector3
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		struct
		{
			float r;
			float g;
			float b;
		};

		float data[3];
	};

public:
	Vector3(void);
	Vector3(const Vector3& v);
	Vector3(float vx, float vy, float vz);

	bool operator == (const Vector3& v) const;
	bool operator != (const Vector3& v) const;

	Vector3& operator = (const Vector3& v);
	Vector3& operator += (const Vector3& v);
	Vector3& operator -= (const Vector3& v);
	Vector3& operator *= (const Vector3& v);
	Vector3& operator *= (float s);
	Vector3& operator /= (const Vector3& v);
	Vector3& operator /= (float s);

	Vector3 operator + (const Vector3& v) const;
	Vector3 operator - (const Vector3& v) const;
	Vector3 operator * (const Vector3& v) const;
	Vector3 operator * (float s) const;
	Vector3 operator / (const Vector3& v) const;
	Vector3 operator / (float s) const;

	Vector3 operator +(void) const;
	Vector3 operator -(void) const;

	friend const Vector3 operator * (float s, const Vector3& v)
	{
		Vector3 tmp(v);
		tmp *= s;
		return tmp;
	}

	friend const Vector3 operator / (float s, const Vector3& v)
	{
		Vector3 tmp(v);
		tmp /= s;
		return tmp;
	}

	void Set(float vx, float vy, float vz);

	float Normalize(void);
	Vector3 ToNormalize(float* pLength = nullptr) const;

	void Saturate(void);
	Vector3 ToSaturate(void) const;

	float GetLengthSqr(void) const;
	float GetLength(void) const;

	static float Dot(const Vector3& l, const Vector3& r);
	static Vector3 Cross(const Vector3& l, const Vector3& r);

	static Vector3 Minimum(const Vector3& v0, const Vector3& v1);
	static Vector3 Maximum(const Vector3& v0, const Vector3& v1);

	static Vector3 Lerp(const Vector3& vs, const Vector3& ve, float t);
	static Vector3 Slerp(const Vector3& vs, const Vector3& ve, float t);

public:
	static const Vector3& Zero(void)
	{
		static const Vector3 zero(0.0f, 0.0f, 0.0f);
		return zero;
	}

	static const Vector3& XAxis(void)
	{
		static const Vector3 xAxis(1.0f, 0.0f, 0.0f);
		return xAxis;
	}

	static const Vector3& YAxis(void)
	{
		static const Vector3 yAxis(0.0f, 1.0f, 0.0f);
		return yAxis;
	}

	static const Vector3& ZAxis(void)
	{
		static const Vector3 zAxis(0.0f, 0.0f, 1.0f);
		return zAxis;
	}
};
