#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

class Camera
{
public:
	Camera() {}

	void Update();

	const Matrix4x4& GetInverseViewMatrix() const;
	const Matrix4x4& GetViewProjectionMatrix() const;
	const Matrix4x4& GetInverseViewProjectionMatrix() const;

	const Vector3& GetEye() const;
	const Vector3& GetAt() const;
	const Matrix4x4& GetViewMatrix() const;
	void SetView(const Vector3& eye, const Vector3& at, const Vector3& up);
	void SetView(const Quaternion& rotation, const Vector3& at, float distance);

	const Matrix4x4& GetProjectionMatrix() const;
	void SetProjection(float fov, float aspect, float nearClip, float farClip);

private:
	Vector3 m_Eye;
	Vector3 m_At;
	Matrix4x4 m_ViewMatrix;

	Matrix4x4 m_ProjectionMatrix;

	Matrix4x4 m_InverseViewMatrix;
	Matrix4x4 m_ViewProjectionMatrix;
	Matrix4x4 m_InverseViewProjectionMatrix;
};
