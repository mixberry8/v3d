#include "ViewCameraWindow.h"

ViewCameraWindow::ViewCameraWindow() :
m_CameraDistance(5.0f),
m_CameraAt(Vector3::Zero()),
m_isDragMouse(false),
m_MousePos({})
{
	m_CameraRotation.SetRotationAxis(Vector3::YAxis(), DEG_TO_RAD(0.0f));
}

void ViewCameraWindow::SetCameraRotation(const Vector3& axis, float angle)
{
	m_CameraRotation.SetRotationAxis(axis, angle);
}

void ViewCameraWindow::SetCameraDistance(float distance)
{
	m_CameraDistance = MAXIMUM(1.0f, distance);
}

void ViewCameraWindow::SetCameraAt(const Vector3& at)
{
	m_CameraAt = at;
}

bool ViewCameraWindow::OnIdle()
{
	const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

	m_Camera.SetView(m_CameraRotation, m_CameraAt, m_CameraDistance);

	float aspectRatio = TO_F32(swapChainDesc.imageWidth) / TO_F32(swapChainDesc.imageHeight);
	m_Camera.SetProjection(DEG_TO_RAD(60.0f), aspectRatio, 0.1f, 1000.0f);

	m_Camera.Update();

	return true;
}

bool ViewCameraWindow::OnMouseButtonDown(WPARAM keyFlags, const POINT& pos)
{
	if (keyFlags & MK_LBUTTON)
	{
		if (m_isDragMouse == false)
		{
			m_isDragMouse = true;
			m_MousePos = pos;
		}
	}

	return true;
}

bool ViewCameraWindow::OnMouseButtonUp(WPARAM keyFlags, const POINT& pos)
{
	if (keyFlags & MK_LBUTTON)
	{
		m_isDragMouse = false;
	}

	return true;
}

bool ViewCameraWindow::OnMouseMove(WPARAM keyFlags, const POINT& pos)
{
	static constexpr float distanceCoiffe = 0.05f;
	static constexpr float moveCoiffe = 0.015f;
	static constexpr float rotationCoiffe = 0.25f;

	POINT vec;

	vec.x = pos.x - m_MousePos.x;
	vec.y = pos.y - m_MousePos.y;

	if (keyFlags & MK_LBUTTON)
	{
		if (keyFlags & MK_CONTROL)
		{
			m_CameraDistance += static_cast<float>(vec.y) * -distanceCoiffe;
			m_CameraDistance = MAXIMUM(1.0f, m_CameraDistance);
		}
		else if (keyFlags & MK_SHIFT)
		{
			Matrix4x4 mat = m_CameraRotation;

			Vector3 dirX(1.0f, 0.0f, 0.0f);
			Vector3 dirY(0.0f, 1.0f, 0.0f);

			dirX = mat * dirX;
			dirY = mat * dirY;

			m_CameraAt += dirX * static_cast<float>(vec.x) * -moveCoiffe;
			m_CameraAt += dirY * static_cast<float>(vec.y) * -moveCoiffe;
		}
		else
		{
			Quaternion rot;

			rot.SetRotationAxis(Vector3::YAxis(), DEG_TO_RAD(static_cast<float>(vec.x) * -rotationCoiffe));
			rot.RotationAxis(Vector3::XAxis(), DEG_TO_RAD(static_cast<float>(vec.y) * rotationCoiffe));

			m_CameraRotation = rot * m_CameraRotation;
		}
	}

	m_MousePos = pos;

	return true;
}

bool ViewCameraWindow::OnMouseWheel(WPARAM keyFlags, int16_t delta, const POINT& pos)
{
	static constexpr float distanceCoiffe = 0.5f;

	if (delta > 0)
	{
		m_CameraDistance -= distanceCoiffe;
		m_CameraDistance = MAXIMUM(1.0f, m_CameraDistance);
	}
	else if (delta < 0)
	{
		m_CameraDistance += distanceCoiffe;
	}

	m_Camera.SetView(m_CameraRotation, m_CameraAt, m_CameraDistance);

	return true;
}
