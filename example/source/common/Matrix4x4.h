#pragma once

#include "Vector4.h"
#include "Quaternion.h"

class Matrix4x4
{
public:
	Vector4 x;
	Vector4 y;
	Vector4 z;
	Vector4 w;

	Matrix4x4();
	Matrix4x4(const Quaternion rotation);
	Matrix4x4(const Vector4& mx, const Vector4& my, const Vector4& mz, const Vector4& mw);

	void SetIdentity();
	void SetScale(const Vector3& scale);
	void SetRotation(const Quaternion& quat);
	void SetTranslation(const Vector3& translation);

	Vector4 GetRow(uint32_t row) const;
	void SetRow(uint32_t row, const Vector4& vector);

	Matrix4x4& Inverse();
	Matrix4x4 ToInverse() const;

	Matrix4x4& Transpose();
	Matrix4x4 ToTranspose() const;

	void Transform(uint32_t vectorCount, Vector3* vectors);
	void Transform(uint32_t vectorCount, Vector4* vectors);

	bool operator == (const Matrix4x4& matrix) const;
	bool operator != (const Matrix4x4& matrix) const;

	Matrix4x4& operator = (const Matrix4x4& matrix);
	Matrix4x4& operator *= (const Matrix4x4& matrix);
	Matrix4x4 operator * (const Matrix4x4& matrix) const;

	Vector3 operator * (const Vector3& vector) const;
	Vector4 operator * (const Vector4& vector) const;

	Vector4& operator [](uint32_t column);
	float& operator ()(uint32_t column, uint32_t row);

	static const Matrix4x4& Identity()
	{
		static Matrix4x4 identity;
		return identity;
	}

	static Matrix4x4 Scale(const Vector3& scale)
	{
		Matrix4x4 result;
		result.SetScale(scale);
		return result;
	}

	static Matrix4x4 Rotation(const Quaternion& quat)
	{
		Matrix4x4 result;
		result.SetRotation(quat);
		return result;
	}

	static Matrix4x4 Translation(const Vector3& translation)
	{
		Matrix4x4 result;
		result.SetTranslation(translation);
		return result;
	}
};
