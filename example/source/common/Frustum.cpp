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

bool Frustum::Contains(const AABB& aabb) const
{
	const Plane* pPlane = &(m_Planes[0]);
	const Plane* pPlaneEnd = pPlane + 6;

	const Vector3* pPointBegin = &(aabb.points[0]);
	const Vector3* pPointEnd = pPointBegin + 8;
	const Vector3* pPoint = NULL;

	float dist;
	uint32_t count;

	while (pPlane != pPlaneEnd)
	{
		const Vector3& norm = pPlane->normal;

		count = 8;
		pPoint = pPointBegin;
		while (pPoint != pPointEnd)
		{
			dist = (norm.x * pPoint->x) + (norm.y * pPoint->y) + (norm.z * pPoint->z) + pPlane->d;
			if (dist < 0.0f)
			{
				ASSERT(count > 0);
				count--;
			}

			pPoint++;
		}

		if (count == 0)
		{
			return false;
		}

		pPlane++;
	}

	return true;
}
