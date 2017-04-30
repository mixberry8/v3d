#include "Camera.h"

void Camera::Update()
{
	// ビュー の逆行列
	m_InverseViewMatrix = m_ViewMatrix.ToInverse();

	// ビュー * プロジェクションの行列
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

	// ビュー * プロジェクションの逆行列
	m_InverseViewProjectionMatrix = m_ViewProjectionMatrix.ToInverse();
}

const Matrix4x4& Camera::GetInverseViewMatrix() const
{
	return m_InverseViewMatrix;
}

const Matrix4x4& Camera::GetViewProjectionMatrix() const
{
	return m_ViewProjectionMatrix;
}

const Matrix4x4& Camera::GetInverseViewProjectionMatrix() const
{
	return m_InverseViewProjectionMatrix;
}

const Vector3& Camera::GetEye() const
{
	return m_Eye;
}

const Vector3& Camera::GetAt() const
{
	return m_At;
}

const Matrix4x4& Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

void Camera::SetView(const Vector3& eye, const Vector3& at, const Vector3& up)
{
	m_Eye = eye;
	m_At = at;

	Vector3 n = (m_Eye - m_At).ToNormalize();
	Vector3 u = Vector3::Cross(up, n);
	Vector3 v = Vector3::Cross(n, u);

	// |u.x|u.y|u.z|-eye dot u|
	// |v.x|v.y|v.z|-eye dot v|
	// |n.x|n.y|n.z|-eye dot n|
	// |0.0|0.0|0.0|       1.0|

	m_ViewMatrix.x.Set(u.x, v.x, n.x, 0.0f);
	m_ViewMatrix.y.Set(u.y, v.y, n.y, 0.0f);
	m_ViewMatrix.z.Set(u.z, v.z, n.z, 0.0f);
	m_ViewMatrix.w.Set(-Vector3::Dot(eye, u), -Vector3::Dot(eye, v), -Vector3::Dot(eye, n), 1.0f);
}

void Camera::SetView(const Quaternion& rotation, const Vector3& at, float distance)
{
	Matrix4x4 rotationMatrix(rotation);

	// ローカル視点
	Vector3 localEye(0.0f, 0.0f, distance);
	rotationMatrix.Transform(1, &localEye);

	// 上方
	Vector3 up(rotationMatrix.y.x, rotationMatrix.y.y, rotationMatrix.y.z);
	up.Normalize();

	//各種ベクトル
	Vector3 n = localEye.ToNormalize();
	Vector3 u = Vector3::Cross(up, n);
	Vector3 v = Vector3::Cross(n, u);

	// 視点
	m_Eye = at + localEye;

	// 注視点
	m_At = at;

	// ビュー行列
	m_ViewMatrix.x.Set(u.x, v.x, n.x, 0.0f);
	m_ViewMatrix.y.Set(u.y, v.y, n.y, 0.0f);
	m_ViewMatrix.z.Set(u.z, v.z, n.z, 0.0f);
	m_ViewMatrix.w.Set(-Vector3::Dot(m_Eye, u), -Vector3::Dot(m_Eye, v), -Vector3::Dot(m_Eye, n), 1.0f);
}

const Matrix4x4& Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

void Camera::SetProjection(float fov, float aspect, float nearClip, float farClip)
{
	float t = ::tanf(fov * 0.5f);
	float w = FLOAT_RECIPROCAL(aspect * t);
	float h = FLOAT_RECIPROCAL(t);
	float z1 = -FLOAT_DIV((farClip + nearClip), (farClip - nearClip));
	float z2 = -FLOAT_DIV((2.0f * farClip * nearClip), (farClip - nearClip));

	// |  w|0.0| 0.0|0.0|
	// |0.0|  h| 0.0|0.0|
	// |0.0|0.0|  z1| z2|
	// |0.0|0.0|-1.0|0.0|

	m_ProjectionMatrix.x.Set(w, 0.0f, 0.0f, 0.0f);
	m_ProjectionMatrix.y.Set(0.0f, h, 0.0f, 0.0f);
	m_ProjectionMatrix.z.Set(0.0f, 0.0f, z1, -1.0f);
	m_ProjectionMatrix.w.Set(0.0f, 0.0f, z2, 0.0f);
}
