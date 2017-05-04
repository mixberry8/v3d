#pragma once

class Plane
{
public:
	Vector3 normal;
	float d;

	Plane();
	Plane(const Vector3& normal, float d);

	void Normalize();

	static Plane Calculate(const Vector3& p1, const Vector3& p2, const Vector3& p3)
	{
		Vector3 v1;
		Vector3 v2;

		v1 = p2 - p1;
		v2 = p3 - p1;

		Plane plane;
		plane.normal = Vector3::Cross(v1, v2);
		plane.normal.Normalize();

		plane.d = Vector3::Dot(plane.normal, p1);

		return plane;
	}
};