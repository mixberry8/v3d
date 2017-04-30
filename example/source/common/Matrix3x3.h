#pragma once

#include "Vector2.h"
#include "Vector3.h"

class Matrix3x3
{
public:
	Vector3 x;
	Vector3 y;
	Vector3 z;

	Matrix3x3();
	Matrix3x3(const Vector3& mx, const Vector3& my, const Vector3& mz);

	void SetIdentity();
	void SetScale(const Vector2& scale);
	void SetSkew(const Vector2& skew);
	void SetRotation(float theta);
	void SetTranslation(const Vector2& translation);

	Vector3 GetRow(uint32_t row);
	void SetRow(uint32_t row, const Vector3& vector);

	Matrix3x3& Inverse();
	Matrix3x3 ToInverse() const;

	Matrix3x3& Transpose();
	Matrix3x3 ToTranspose() const;

	void Transform(uint32_t vectorCount, Vector2* vectors);
	void Transform(uint32_t vectorCount, Vector3* vectors);

	bool operator == (const Matrix3x3& matrix) const;
	bool operator != (const Matrix3x3& matrix) const;

	Matrix3x3& operator = (const Matrix3x3& matrix);
	Matrix3x3& operator *= (const Matrix3x3& matrix);
	Matrix3x3 operator * (const Matrix3x3& matrix) const;

	Vector2 operator * (const Vector2& vector) const;
	Vector3 operator * (const Vector3& vector) const;

	Vector3& operator [](uint32_t column);
	float& operator ()(uint32_t column, uint32_t row);

	static const Matrix3x3& Identity()
	{
		static Matrix3x3 identity;
		return identity;
	}

	static Matrix3x3 Scale(const Vector2& scale)
	{
		Matrix3x3 result;
		result.SetScale(scale);
		return result;
	}

	static Matrix3x3 Skew(const Vector2& skew)
	{
		Matrix3x3 result;
		result.SetSkew(skew);
		return result;
	}

	static Matrix3x3 Rotation(float theta)
	{
		Matrix3x3 result;
		result.SetRotation(theta);
		return result;
	}

	static Matrix3x3 Translation(const Vector2& translation)
	{
		Matrix3x3 result;
		result.SetTranslation(translation);
		return result;
	}

};
