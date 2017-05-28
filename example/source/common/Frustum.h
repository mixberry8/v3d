#pragma once

#include "Plane.h"
#include "Sphere.h"
#include "AABB.h"

class Frustum final
{
public:
	enum PLANE_TYPE
	{
		PLANE_TYPE_NEAR = 0,
		PLANE_TYPE_FAR = 1,
		PLANE_TYPE_TOP = 2,
		PLANE_TYPE_BOTTOM = 3,
		PLANE_TYPE_LEFT = 4,
		PLANE_TYPE_RIGHT = 5,
	};

	void Update(const Matrix4x4& viewProjMatrix);

	const Plane& GetPlane(Frustum::PLANE_TYPE type) const;

	bool Contains(const Sphere& sphere) const;
	bool Contains(const AABB& aabb) const;

private:
	Plane m_Planes[6];
};
