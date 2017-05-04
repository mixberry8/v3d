#include "Sphere.h"

Sphere::Sphere() :
	radius(0.0f)
{
}

Sphere::Sphere(const Vector3& center, float radius)
{
	this->center = center;
	this->radius = radius;
}
