#pragma once

class Vector2
{
public:
	union
	{
		struct
		{
			float x;
			float y;
		};

		struct
		{
			float r;
			float g;
		};

		float data[2];
	};

public:
	Vector2(void);
	Vector2(const Vector2& v);
	Vector2(float vx, float vy);

	bool operator == (const Vector2& v) const;
	bool operator != (const Vector2& v) const;

	Vector2& operator = (const Vector2& v);
	Vector2& operator += (const Vector2& v);
	Vector2& operator -= (const Vector2& v);
	Vector2& operator *= (const Vector2& v);
	Vector2& operator *= (float s);
	Vector2& operator /= (const Vector2& v);
	Vector2& operator /= (float s);

	Vector2 operator + (const Vector2& v) const;
	Vector2 operator - (const Vector2& v) const;
	Vector2 operator * (const Vector2& v) const;
	Vector2 operator * (float s) const;
	Vector2 operator / (const Vector2& v) const;
	Vector2 operator / (float s) const;

	Vector2 operator +(void) const;
	Vector2 operator -(void) const;

	friend const Vector2 operator * (float s, const Vector2& v)
	{
		Vector2 tmp(v);
		tmp *= s;
		return tmp;
	}

	friend const Vector2 operator / (float s, const Vector2& v)
	{
		Vector2 tmp(v);
		tmp /= s;
		return tmp;
	}

	void Set(float vx, float vy);

	float Normalize(void);
	Vector2 ToNormalize(float* pLength = nullptr) const;

	void Saturate(void);
	Vector2 ToSaturate(void) const;

	float GetLengthSqr(void) const;
	float GetLength(void) const;
	float GetLengthF(void) const;

	static float Dot(const Vector2& l, const Vector2& r);
	static float Cross(const Vector2& l, const Vector2& r);

	static Vector2 Minimum(const Vector2& v0, const Vector2& v1);
	static Vector2 Maximum(const Vector2& v0, const Vector2& v1);

	static Vector2 Lerp(const Vector2& vs, const Vector2& ve, float t);
	static Vector2 Slerp(const Vector2& vs, const Vector2& ve, float t);

public:
	static const Vector2& Zero(void)
	{
		static const Vector2 zero(0.0f, 0.0f);
		return zero;
	}

	static const Vector2& XAxis(void)
	{
		static const Vector2 xAxis(1.0f, 0.0f);
		return xAxis;
	}

	static const Vector2& YAxis(void)
	{
		static const Vector2 yAxis(0.0f, 1.0f);
		return yAxis;
	}
};
