#pragma once

#include "Window.h"
#include "Camera.h"

class ViewCameraWindow : public Window
{
public:
	ViewCameraWindow();
	virtual ~ViewCameraWindow() {}

	Camera* GetCamera() { return &m_Camera; }
	void SetCameraRotation(const Vector3& axis, float angle);
	void SetCameraDistance(float distance);
	void SetCameraAt(const Vector3& at);

protected:
	virtual bool OnIdle() override;
	virtual bool OnMouseButtonDown(WPARAM keyFlags, const POINT& pos) override;
	virtual bool OnMouseButtonUp(WPARAM keyFlags, const POINT& pos) override;
	virtual bool OnMouseMove(WPARAM keyFlags, const POINT& pos) override;
	virtual bool OnMouseWheel(WPARAM keyFlags, int16_t delta, const POINT& pos) override;

private:
	Camera m_Camera;
	Quaternion m_CameraRotation;
	float m_CameraDistance;
	Vector3 m_CameraAt;

	bool m_isDragMouse;
	POINT m_MousePos;
};
