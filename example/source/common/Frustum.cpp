#include "Frustum.h"

void Frustum::Update(const Matrix4x4& viewProjMatrix)
{
	Plane* plane;

	// NearPlane
	plane = &m_Planes[Frustum::PLANE_TYPE_NEAR];
	plane->normal.x = viewProjMatrix.x.w + viewProjMatrix.x.z;
	plane->normal.y = viewProjMatrix.y.w + viewProjMatrix.y.z;
	plane->normal.z = viewProjMatrix.z.w + viewProjMatrix.z.z;
	plane->d = viewProjMatrix.w.w + viewProjMatrix.w.z;
	plane->Normalize();

	// FarPlane
	plane = &m_Planes[Frustum::PLANE_TYPE_FAR];
	plane->normal.x = viewProjMatrix.x.w - viewProjMatrix.x.z;
	plane->normal.y = viewProjMatrix.y.w - viewProjMatrix.y.z;
	plane->normal.z = viewProjMatrix.z.w - viewProjMatrix.z.z;
	plane->d = viewProjMatrix.w.w - viewProjMatrix.w.z;
	plane->Normalize();

	// TopPlane
	plane = &m_Planes[Frustum::PLANE_TYPE_TOP];
	plane->normal.x = viewProjMatrix.x.w + viewProjMatrix.x.y;
	plane->normal.y = viewProjMatrix.y.w + viewProjMatrix.y.y;
	plane->normal.z = viewProjMatrix.z.w + viewProjMatrix.z.y;
	plane->d = viewProjMatrix.w.w + viewProjMatrix.w.y;
	plane->Normalize();

	// BottomPlane
	plane = &m_Planes[Frustum::PLANE_TYPE_BOTTOM];
	plane->normal.x = viewProjMatrix.x.w - viewProjMatrix.x.y;
	plane->normal.y = viewProjMatrix.y.w - viewProjMatrix.y.y;
	plane->normal.z = viewProjMatrix.z.w - viewProjMatrix.z.y;
	plane->d = viewProjMatrix.w.w - viewProjMatrix.w.y;
	plane->Normalize();

	// LeftPlane
	plane = &m_Planes[Frustum::PLANE_TYPE_LEFT];
	plane->normal.x = viewProjMatrix.x.w + viewProjMatrix.x.x;
	plane->normal.y = viewProjMatrix.y.w + viewProjMatrix.y.x;
	plane->normal.z = viewProjMatrix.z.w + viewProjMatrix.z.x;
	plane->d = viewProjMatrix.w.w + viewProjMatrix.w.x;
	plane->Normalize();

	// RightPlane
	plane = &m_Planes[Frustum::PLANE_TYPE_RIGHT];
	plane->normal.x = viewProjMatrix.x.w - viewProjMatrix.x.x;
	plane->normal.y = viewProjMatrix.y.w - viewProjMatrix.y.x;
	plane->normal.z = viewProjMatrix.z.w - viewProjMatrix.z.x;
	plane->d = viewProjMatrix.w.w - viewProjMatrix.w.x;
	plane->Normalize();
}

const Plane& Frustum::GetPlane(Frustum::PLANE_TYPE type) const
{
	return m_Planes[type];
}

bool Frustum::Contains(const Sphere& sphere) const
{
	const Vector3& center = sphere.center;
	const float& radius = sphere.radius;

	const Plane* plane = m_Planes;
	const Plane* planeEnd = plane + 6;

	while (plane != planeEnd)
	{
		const Vector3& normal = plane->normal;
		float distance = (normal.x * center.x) + (normal.y * center.y) + (normal.z * center.z) + plane->d;

		if (-radius > distance)
		{
			return false;
		}

		plane++;
	}

	return true;
}
