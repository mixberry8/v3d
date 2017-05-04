#pragma once

class Sphere
{
public:
	Vector3 center;
	float radius;

public:
	Sphere(void);
	Sphere(const Vector3& center, float radius);
};
