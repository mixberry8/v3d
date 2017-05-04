#include "Plane.h"

Plane::Plane() :
	d(0.0f)
{
}

Plane::Plane(const Vector3& normal, float d)
{
	this->normal = normal;
	this->d = -d;
}

void Plane::Normalize(void)
{
	float distance = ::sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

	if (FLOAT_IS_ZERO(distance) == false)
	{
		distance = 1.0f / distance;

		normal.x *= distance;
		normal.y *= distance;
		normal.z *= distance;

		d *= distance;
	}
	else
	{
		normal.x = 0.0f;
		normal.y = 0.0f;
		normal.z = 0.0f;

		d = 0.0f;
	}
}
