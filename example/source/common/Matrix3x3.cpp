#include "Matrix3x3.h"

Matrix3x3::Matrix3x3() :
	x(1.0f, 0.0f, 0.0f),
	y(0.0f, 1.0f, 0.0f),
	z(0.0f, 0.0f, 1.0f)
{
}

Matrix3x3::Matrix3x3(const Vector3& mx, const Vector3& my, const Vector3& mz) :
	x(mx),
	y(my),
	z(mz)
{
}

Vector3 Matrix3x3::GetRow(uint32_t row)
{
	ASSERT(row < 3);

	switch (row)
	{
	case 0:
		return Vector3(x.x, y.x, z.x);
	case 1:
		return Vector3(x.y, y.y, z.y);
	}

	return Vector3(x.z, y.z, z.z);
}

void Matrix3x3::SetRow(uint32_t row, const Vector3& vector)
{
	ASSERT(row < 3);

	switch (row)
	{
	case 0:
		x.x = vector.x;
		y.x = vector.y;
		z.x = vector.z;
		break;
	case 1:
		x.y = vector.x;
		y.y = vector.y;
		z.y = vector.z;
		break;
	}

	x.z = vector.x;
	y.z = vector.y;
	z.z = vector.z;
}

void Matrix3x3::SetIdentity()
{
	x.Set(1.0f, 0.0f, 0.0f);
	y.Set(0.0f, 1.0f, 0.0f);
	z.Set(0.0f, 0.0f, 1.0f);
}

void Matrix3x3::SetScale(const Vector2& scale)
{
	x.Set(scale.x, 0.0f, 0.0f);
	y.Set(0.0f, scale.y, 0.0f);
	z.Set(0.0f, 0.0f, 1.0f);
}

void Matrix3x3::SetSkew(const Vector2& skew)
{
	x.Set(0.0f, skew.x, 0.0f);
	y.Set(skew.y, 0.0f, 0.0f);
	z.Set(0.0f, 0.0f, 1.0f);
}

void Matrix3x3::SetRotation(float theta)
{
	float c = ::cosf(theta);
	float s = ::sinf(theta);

	x.Set(c, s, 0.0f);
	y.Set(-s, c, 0.0f);
	z.Set(0.0f, 0.0f, 1.0f);
}

Matrix3x3& Matrix3x3::Inverse()
{
	float a11 = x.x;
	float a12 = y.x;
	float a13 = z.x;
	float a21 = x.y;
	float a22 = y.y;
	float a23 = z.y;
	float a31 = x.z;
	float a32 = y.z;
	float a33 = z.z;

	float detA = a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 - a11 * a32 * a23 - a31 * a22 * a13 - a21 * a12 * a33;

	if (FLOAT_IS_ZERO(detA) == true)
	{
		return *this;
	}

	detA = 1.0f / detA;

	x.Set(a22 * a33 - a23 * a32, a23 * a31 - a21 * a33, a21 * a32 - a22 * a31);
	y.Set(a13 * a32 - a12 * a33, a11 * a33 - a13 * a31, a12 * a31 - a11 * a32);
	z.Set(a12 * a23 - a13 * a22, a13 * a21 - a11 * a23, a11 * a22 - a12 * a21);

	x *= detA;
	y *= detA;
	z *= detA;

	return *this;
}

Matrix3x3 Matrix3x3::ToInverse() const
{
	Matrix3x3 result(*this);
	result.Inverse();
	return result;
}

Matrix3x3& Matrix3x3::Transpose()
{
	Matrix3x3 temp;

	temp.x.Set(x.x, y.x, z.x);
	temp.y.Set(x.y, y.y, z.y);
	temp.z.Set(x.z, y.z, z.z);

	*this = temp;

	return *this;
}

Matrix3x3 Matrix3x3::ToTranspose() const
{
	Matrix3x3 result(*this);
	result.Transpose();
	return result;
}

void Matrix3x3::SetTranslation(const Vector2& translation)
{
	x.Set(1.0f, 0.0f, 0.0f);
	y.Set(0.0f, 1.0f, 0.0f);
	z.Set(translation.x, translation.y, 1.0f);
}

void Matrix3x3::Transform(uint32_t vectorCount, Vector2* vectors)
{
	ASSERT(vectorCount > 0);
	ASSERT(vectors != nullptr);

	Vector2* src = vectors;
	Vector2* srcEnd = src + vectorCount;

	while (src != srcEnd)
	{
		float vx = (x.x * src->x) + (y.x * src->y) + z.x;
		float vy = (x.y * src->x) + (y.y * src->y) + z.y;

		src->x = vx;
		src->y = vy;

		src++;
	}
}

void Matrix3x3::Transform(uint32_t vectorCount, Vector3* vectors)
{
	ASSERT(vectorCount > 0);
	ASSERT(vectors != nullptr);

	Vector3* src = vectors;
	Vector3* srcEnd = src + vectorCount;

	while (src != srcEnd)
	{
		float vx = (x.x * src->x) + (y.x * src->y) + (z.x * src->z);
		float vy = (x.y * src->x) + (y.y * src->y) + (z.y * src->z);
		float vz = (x.z * src->x) + (y.z * src->y) + (z.z * src->z);

		src->x = vx;
		src->y = vy;
		src->z = vz;

		src++;
	}
}

bool Matrix3x3::operator == (const Matrix3x3& matrix) const
{
	return (x == matrix.x) && (y == matrix.y) && (z == matrix.z);
}

bool Matrix3x3::operator != (const Matrix3x3& matrix) const
{
	return (x != matrix.x) || (y != matrix.y) || (z != matrix.z);
}

Matrix3x3& Matrix3x3::operator = (const Matrix3x3& matrix)
{
	x = matrix.x;
	y = matrix.y;
	z = matrix.z;

	return *this;
}

Matrix3x3& Matrix3x3::operator *= (const Matrix3x3& matrix)
{
	Matrix3x3 temp(*this);
	*this = temp * matrix;
	return *this;
}

Matrix3x3 Matrix3x3::operator * (const Matrix3x3& matrix) const
{
	Matrix3x3 result;

	result.x.x = (x.x * matrix.x.x) + (y.x * matrix.x.y) + (z.x * matrix.x.z);
	result.y.x = (x.x * matrix.y.x) + (y.x * matrix.y.y) + (z.x * matrix.y.z);
	result.z.x = (x.x * matrix.z.x) + (y.x * matrix.z.y) + (z.x * matrix.z.z);

	result.x.y = (x.y * matrix.x.x) + (y.y * matrix.x.y) + (z.y * matrix.x.z);
	result.y.y = (x.y * matrix.y.x) + (y.y * matrix.y.y) + (z.y * matrix.y.z);
	result.z.y = (x.y * matrix.z.x) + (y.y * matrix.z.y) + (z.y * matrix.z.z);

	result.x.z = (x.z * matrix.x.x) + (y.z * matrix.x.y) + (z.z * matrix.x.z);
	result.y.z = (x.z * matrix.y.x) + (y.z * matrix.y.y) + (z.z * matrix.y.z);
	result.z.z = (x.z * matrix.z.x) + (y.z * matrix.z.y) + (z.z * matrix.z.z);

	return result;
}

Vector2 Matrix3x3::operator * (const Vector2& vector) const
{
	Vector2 result;

	result.x = (x.x * vector.x) + (y.x * vector.y) + z.x;
	result.y = (x.y * vector.x) + (y.y * vector.y) + z.y;

	return result;
}

Vector3 Matrix3x3::operator * (const Vector3& vector) const
{
	Vector3 result;

	result.x = (x.x * vector.x) + (y.x * vector.y) + (z.x * vector.z);
	result.y = (x.y * vector.x) + (y.y * vector.y) + (z.y * vector.z);
	result.z = (x.z * vector.x) + (y.z * vector.y) + (z.z * vector.z);

	return result;
}

Vector3& Matrix3x3::operator [](uint32_t column)
{
	ASSERT(column < 3);

	switch (column)
	{
	case 0:
		return x;
	case 1:
		return y;
	}

	return z;
}

float& Matrix3x3::operator ()(uint32_t column, uint32_t row)
{
	ASSERT(column < 3);
	ASSERT(row < 3);

	switch (column)
	{
	case 0:
		return x.data[row];
	case 1:
		return y.data[row];
	}

	return z.data[row];
}
