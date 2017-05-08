#pragma once

class AABB
{
public:
	Vector3 minimum;
	Vector3 maximum;
	Vector3 center;
	Vector3 points[8];

	AABB(void);
	AABB(const Vector3& minimum, const Vector3& maximum);

	AABB& UpdateCenterAndPoints();
};
