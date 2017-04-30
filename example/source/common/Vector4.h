#pragma once

class Vector4
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

		struct
		{
			float r;
			float g;
			float b;
			float a;
		};

		float data[4];
	};

public:
	Vector4(void);
	Vector4(const Vector2& v, float z, float w);
	Vector4(const Vector3& v, float w);
	Vector4(const Vector4& v);
	Vector4(float vx, float vy, float vz);
	Vector4(float vx, float vy, float vz, float vw);

	bool operator == (const Vector4& v) const;
	bool operator != (const Vector4& v) const;

	Vector4& operator = (const Vector4& v);
	Vector4& operator += (const Vector4& v);
	Vector4& operator -= (const Vector4& v);
	Vector4& operator *= (const Vector4& v);
	Vector4& operator *= (float s);
	Vector4& operator /= (const Vector4& v);
	Vector4& operator /= (float s);

	Vector4 operator + (const Vector4& v) const;
	Vector4 operator - (const Vector4& v) const;
	Vector4 operator * (const Vector4& v) const;
	Vector4 operator * (float s) const;
	Vector4 operator / (const Vector4& v) const;
	Vector4 operator / (float s) const;

	Vector4 operator +(void) const;
	Vector4 operator -(void) const;

	friend const Vector4 operator * (float s, const Vector4& v)
	{
		Vector4 tmp(v);
		tmp *= s;
		return tmp;
	}

	friend const Vector4 operator / (float s, const Vector4& v)
	{
		Vector4 tmp(v);
		tmp /= s;
		return tmp;
	}

	void Set(float vx, float vy, float vz, float vw);

	float Normalize(void);
	Vector4 ToNormalize(float* pLength = nullptr) const;

	void Saturate(void);
	Vector4 ToSaturate(void) const;

	float GetLengthSqr(void) const;
	float GetLength(void) const;

	static float Dot(const Vector4& lv, const Vector4& rv);
	static Vector4 Cross(const Vector4& lv, const Vector4& rv);

	static Vector4 Minimum(const Vector4& v0, const Vector4& v1);
	static Vector4 Maximum(const Vector4& v0, const Vector4& v1);

	static Vector4 Lerp(const Vector4& vs, const Vector4& ve, float t);
	static Vector4 Slerp(const Vector4& vs, const Vector4& ve, float t);

public:
	static const Vector4& Zero(void)
	{
		static const Vector4 zero(0.0f, 0.0f, 0.0f, 1.0f);
		return zero;
	}

	static const Vector4& XAxis(void)
	{
		static const Vector4 xAxis(1.0f, 0.0f, 0.0f, 1.0f);
		return xAxis;
	}

	static const Vector4& YAxis(void)
	{
		static const Vector4 yAxis(0.0f, 1.0f, 0.0f, 1.0f);
		return yAxis;
	}

	static const Vector4& ZAxis(void)
	{
		static const Vector4 zAxis(0.0f, 0.0f, 1.0f, 1.0f);
		return zAxis;
	}
};
