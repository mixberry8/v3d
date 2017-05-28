#pragma once

#include "common.h"
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

	void TransformSR(uint32_t vectorCount, Vector3* vectors);
	void TransformSR(uint32_t vectorCount, Vector4* vectors);

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

	static Matrix4x4 LookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
	{
		Matrix4x4 result;

		Vector3 n = (eye - at).ToNormalize();
		Vector3 u = Vector3::Cross(up, n);
		Vector3 v = Vector3::Cross(n, u);

		result.x.Set(u.x, v.x, n.x, 0.0f);
		result.y.Set(u.y, v.y, n.y, 0.0f);
		result.z.Set(u.z, v.z, n.z, 0.0f);
		result.w.Set(-Vector3::Dot(eye, u), -Vector3::Dot(eye, v), -Vector3::Dot(eye, n), 1.0f);

		return result;
	}

	static Matrix4x4 Ortho(float width, float height, float nearClip, float farClip)
	{
		Matrix4x4 result;

		result.x.x = 2.0f / width;
		result.y.y = 2.0f / height;
		result.z.z = 1.0f / (nearClip - farClip);
		result.w.z = (1.0f / (nearClip - farClip)) * nearClip;

		return result;
	}

	static Matrix4x4 Ortho(const Vector3& minimum, const Vector3& maximum)
	{
		Matrix4x4 result;

		result.x.x = 2.0f / (maximum.x - minimum.x);
		result.y.y = 2.0f / (maximum.y - minimum.y);
		result.z.z = 2.0f / (minimum.z - maximum.z);
		result.w.x = (maximum.x + minimum.x) / (maximum.x - minimum.x);
		result.w.y = (maximum.y + minimum.y) / (maximum.y - minimum.y);
		result.w.z = (minimum.z + maximum.z) / (minimum.z - maximum.z);

		return result;
	}

	static Matrix4x4 OrthoOffCenter(const Vector3& minimum, const Vector3& maximum)
	{
		Matrix4x4 result;

		result.x.x = 2.0f / (maximum.x - minimum.x);
		result.y.y = 2.0f / (maximum.y - minimum.y);
		result.z.z = 1.0f / (minimum.z - maximum.z);
		result.w.x = (maximum.x + minimum.x) / (maximum.x - minimum.x);
		result.w.y = (maximum.y + minimum.y) / (maximum.y - minimum.y);
		result.w.z = minimum.z / (minimum.z - maximum.z);

		return result;
	}
};
