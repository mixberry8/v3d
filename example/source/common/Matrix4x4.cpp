#include "Matrix4x4.h"

Matrix4x4::Matrix4x4() :
	x(1.0f, 0.0f, 0.0f, 0.0f),
	y(0.0f, 1.0f, 0.0f, 0.0f),
	z(0.0f, 0.0f, 1.0f, 0.0f),
	w(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Matrix4x4::Matrix4x4(const Quaternion rotation)
{
	SetRotation(rotation);
}

Matrix4x4::Matrix4x4(const Vector4& mx, const Vector4& my, const Vector4& mz, const Vector4& mw) :
	x(mx),
	y(my),
	z(mz),
	w(mw)
{
}

void Matrix4x4::SetIdentity()
{
	x.Set(1.0f, 0.0f, 0.0f, 0.0f);
	y.Set(0.0f, 1.0f, 0.0f, 0.0f);
	z.Set(0.0f, 0.0f, 1.0f, 0.0f);
	w.Set(0.0f, 0.0f, 0.0f, 1.0f);
}

void Matrix4x4::SetScale(const Vector3& scale)
{
	x.Set(scale.x, 0.0f, 0.0f, 0.0f);
	y.Set(0.0f, scale.y, 0.0f, 0.0f);
	z.Set(0.0f, 0.0f, scale.z, 0.0f);
	w.Set(0.0f, 0.0f, 0.0f, 1.0f);
}

void Matrix4x4::SetRotation(const Quaternion& quat)
{
	const float& a = quat.x;
	const float& b = quat.y;
	const float& c = quat.z;
	const float& d = quat.w;

	float a2 = a * a;
	float b2 = b * b;
	float c2 = c * c;
	float d2 = d * d;

	float ab = 2.0f * a * b;
	float ac = 2.0f * a * c;
	float ad = 2.0f * a * d;
	float bc = 2.0f * b * c;
	float bd = 2.0f * b * d;
	float cd = 2.0f * c * d;

	x.x = d2 + a2 - b2 - c2;
	x.y = ab + cd;
	x.z = ac - bd;
	x.w = 0.0f;

	y.x = ab - cd;
	y.y = d2 - a2 + b2 - c2;
	y.z = bc + ad;
	y.w = 0.0f;

	z.x = ac + bd;
	z.y = bc - ad;
	z.z = d2 - a2 - b2 + c2;
	z.w = 0.0f;

	w.x = 0.0f;
	w.y = 0.0f;
	w.z = 0.0f;
	w.w = 1.0f;
}

void Matrix4x4::SetTranslation(const Vector3& translation)
{
	x.Set(1.0f, 0.0f, 0.0f, 0.0f);
	y.Set(0.0f, 1.0f, 0.0f, 0.0f);
	z.Set(0.0f, 0.0f, 1.0f, 0.0f);
	w.Set(translation.x, translation.y, translation.z, 1.0f);
}

Vector4 Matrix4x4::GetRow(uint32_t row) const
{
	ASSERT(row < 4);

	switch (row)
	{
	case 0:
		return Vector4(x.x, y.x, z.x, w.x);
	case 1:
		return Vector4(x.y, y.y, z.y, w.y);
	case 2:
		return Vector4(x.z, y.z, z.z, w.z);
	}

	return Vector4(x.w, y.w, z.w, w.w);
}

void Matrix4x4::SetRow(uint32_t row, const Vector4& vector)
{
	ASSERT(row < 4);

	switch (row)
	{
	case 0:
		x.x = vector.x;
		y.x = vector.y;
		z.x = vector.z;
		w.x = vector.w;
		break;
	case 1:
		x.y = vector.x;
		y.y = vector.y;
		z.y = vector.z;
		w.y = vector.w;
		break;
	case 2:
		x.z = vector.x;
		y.z = vector.y;
		z.z = vector.z;
		w.z = vector.w;
		break;
	}

	x.w = vector.x;
	y.w = vector.y;
	z.w = vector.z;
	w.w = vector.w;
}

Matrix4x4& Matrix4x4::Inverse()
{
	float a11 = x.x;
	float a21 = x.y;
	float a31 = x.z;
	float a41 = x.w;
	float a12 = y.x;
	float a22 = y.y;
	float a32 = y.z;
	float a42 = y.w;
	float a13 = z.x;
	float a23 = z.y;
	float a33 = z.z;
	float a43 = z.w;
	float a14 = w.x;
	float a24 = w.y;
	float a34 = w.z;
	float a44 = w.w;

	float detA =
		(a11 * a22 * a33 * a44) + (a11 * a23 * a34 * a42) + (a11 * a24 * a32 * a43) +
		(a12 * a21 * a34 * a43) + (a12 * a23 * a31 * a44) + (a12 * a24 * a33 * a41) +
		(a13 * a21 * a32 * a44) + (a13 * a23 * a31 * a44) + (a13 * a24 * a31 * a42) +
		(a14 * a21 * a33 * a42) + (a14 * a22 * a31 * a43) + (a14 * a23 * a32 * a41) -
		(a11 * a22 * a34 * a43) - (a11 * a23 * a32 * a44) - (a11 * a24 * a33 * a42) -
		(a12 * a21 * a33 * a44) - (a12 * a23 * a34 * a41) - (a12 * a24 * a31 * a43) -
		(a13 * a21 * a34 * a42) - (a13 * a22 * a31 * a44) - (a13 * a24 * a32 * a41) -
		(a14 * a21 * a32 * a43) - (a14 * a22 * a33 * a41) - (a14 * a23 * a31 * a42);

	if (FLOAT_IS_ZERO(detA) == true)
	{
		return *this;
	}

	x.x = (a22 * a33 * a44) + (a23 * a34 * a42) + (a24 * a32 * a43) - (a22 * a34 * a43) - (a23 * a32 * a44) - (a24 * a33 * a42);
	y.x = (a12 * a34 * a43) + (a13 * a32 * a44) + (a14 * a33 * a42) - (a12 * a33 * a44) - (a13 * a34 * a42) - (a14 * a32 * a43);
	z.x = (a12 * a23 * a44) + (a13 * a24 * a42) + (a14 * a22 * a43) - (a12 * a24 * a43) - (a13 * a22 * a44) - (a14 * a23 * a42);
	w.x = (a12 * a24 * a33) + (a13 * a22 * a34) + (a14 * a23 * a32) - (a12 * a23 * a34) - (a13 * a24 * a32) - (a14 * a22 * a33);

	x.y = (a21 * a34 * a43) + (a23 * a31 * a44) + (a24 * a33 * a41) - (a21 * a33 * a44) - (a23 * a34 * a41) - (a24 * a31 * a43);
	y.y = (a11 * a33 * a44) + (a13 * a34 * a41) + (a14 * a31 * a43) - (a11 * a34 * a43) - (a13 * a31 * a44) - (a14 * a33 * a41);
	z.y = (a11 * a24 * a43) + (a13 * a21 * a44) + (a14 * a23 * a41) - (a11 * a23 * a44) - (a13 * a24 * a41) - (a14 * a21 * a43);
	w.y = (a11 * a23 * a34) + (a13 * a24 * a31) + (a14 * a21 * a33) - (a11 * a24 * a33) - (a13 * a21 * a34) - (a14 * a23 * a31);

	x.z = (a21 * a32 * a44) + (a22 * a34 * a41) + (a24 * a31 * a42) - (a21 * a34 * a42) - (a22 * a31 * a44) - (a24 * a32 * a41);
	y.z = (a11 * a34 * a42) + (a12 * a31 * a44) + (a14 * a32 * a41) - (a11 * a32 * a44) - (a12 * a34 * a41) - (a14 * a31 * a42);
	z.z = (a11 * a22 * a44) + (a12 * a24 * a41) + (a14 * a21 * a42) - (a11 * a24 * a42) - (a12 * a21 * a44) - (a14 * a22 * a41);
	w.z = (a11 * a24 * a32) + (a12 * a21 * a34) + (a14 * a22 * a31) - (a11 * a22 * a34) - (a12 * a24 * a31) - (a14 * a21 * a32);

	x.w = (a21 * a33 * a42) + (a22 * a31 * a43) + (a23 * a32 * a41) - (a21 * a32 * a43) - (a22 * a33 * a41) - (a23 * a31 * a42);
	y.w = (a11 * a32 * a43) + (a12 * a33 * a41) + (a13 * a31 * a42) - (a11 * a33 * a42) - (a12 * a31 * a43) - (a13 * a32 * a41);
	z.w = (a11 * a23 * a42) + (a12 * a21 * a43) + (a13 * a22 * a41) - (a11 * a22 * a43) - (a12 * a23 * a41) - (a13 * a21 * a42);
	w.w = (a11 * a22 * a33) + (a12 * a23 * a31) + (a13 * a21 * a32) - (a11 * a23 * a32) - (a12 * a21 * a33) - (a13 * a22 * a31);

	detA = 1.0f / detA;

	x *= detA;
	y *= detA;
	z *= detA;
	w *= detA;

	return *this;
}

Matrix4x4 Matrix4x4::ToInverse() const
{
	Matrix4x4 result(*this);
	result.Inverse();
	return result;
}

Matrix4x4& Matrix4x4::Transpose()
{
	Matrix4x4 temp;

	temp.x.Set(x.x, y.x, z.x, w.x);
	temp.y.Set(x.y, y.y, z.y, w.y);
	temp.z.Set(x.z, y.z, z.z, w.z);
	temp.w.Set(x.w, y.w, z.w, w.w);

	*this = temp;

	return *this;
}

void Matrix4x4::Transform(uint32_t vectorCount, Vector3* vectors)
{
	ASSERT(vectorCount > 0);
	ASSERT(vectors != nullptr);

	Vector3* src = vectors;
	Vector3* srcEnd = src + vectorCount;

	while (src != srcEnd)
	{
		float vw = (x.w * src->x) + (y.w * src->y) + (z.w * src->z) + w.w;

		if (FLOAT_IS_ZERO(vw) == false)
		{
			vw = FLOAT_RECIPROCAL(vw);

			float vx = ((x.x * src->x) + (y.x * src->y) + (z.x * src->z) + w.x) * vw;
			float vy = ((x.y * src->x) + (y.y * src->y) + (z.y * src->z) + w.y) * vw;
			float vz = ((x.z * src->x) + (y.z * src->y) + (z.z * src->z) + w.z) * vw;

			src->x = vx;
			src->y = vy;
			src->z = vz;
		}
		else
		{
			*src = Vector3::Zero();
		}

		src++;
	}
}

void Matrix4x4::Transform(uint32_t vectorCount, Vector4* vectors)
{
	ASSERT(vectorCount > 0);
	ASSERT(vectors != nullptr);

	Vector4* src = vectors;
	Vector4* srcEnd = src + vectorCount;

	while (src != srcEnd)
	{
		float vx = (x.x * src->x) + (y.x * src->y) + (z.x * src->z) + (w.x * src->w);
		float vy = (x.y * src->x) + (y.y * src->y) + (z.y * src->z) + (w.y * src->w);
		float vz = (x.z * src->x) + (y.z * src->y) + (z.z * src->z) + (w.z * src->w);
		float vw = (x.w * src->x) + (y.w * src->y) + (z.w * src->z) + (w.w * src->w);

		src->x = vx;
		src->y = vy;
		src->z = vz;
		src->w = vw;

		src++;
	}
}

void Matrix4x4::TransformSR(uint32_t vectorCount, Vector3* vectors)
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

void Matrix4x4::TransformSR(uint32_t vectorCount, Vector4* vectors)
{
	ASSERT(vectorCount > 0);
	ASSERT(vectors != nullptr);

	Vector4* src = vectors;
	Vector4* srcEnd = src + vectorCount;

	while (src != srcEnd)
	{
		float vx = (x.x * src->x) + (y.x * src->y) + (z.x * src->z);
		float vy = (x.y * src->x) + (y.y * src->y) + (z.y * src->z);
		float vz = (x.z * src->x) + (y.z * src->y) + (z.z * src->z);
		float vw = (x.w * src->x) + (y.w * src->y) + (z.w * src->z);

		src->x = vx;
		src->y = vy;
		src->z = vz;
		src->w = vw;

		src++;
	}
}

Matrix4x4 Matrix4x4::ToTranspose() const
{
	Matrix4x4 result(*this);
	result.Transpose();
	return result;
}

bool Matrix4x4::operator == (const Matrix4x4& matrix) const
{
	if ((x != matrix.x) ||
		(y != matrix.y) ||
		(z != matrix.z) ||
		(w != matrix.w))
	{
		return false;
	}

	return true;
}

bool Matrix4x4::operator != (const Matrix4x4& matrix) const
{
	if ((x != matrix.x) ||
		(y != matrix.y) ||
		(z != matrix.z) ||
		(w != matrix.w))
	{
		return true;
	}

	return false;
}

Matrix4x4& Matrix4x4::operator = (const Matrix4x4& matrix)
{
	x = matrix.x;
	y = matrix.y;
	z = matrix.z;
	w = matrix.w;

	return *this;
}

Matrix4x4& Matrix4x4::operator *= (const Matrix4x4& matrix)
{
	Matrix4x4 temp(*this);
	*this = temp * matrix;
	return *this;
}

Matrix4x4 Matrix4x4::operator * (const Matrix4x4& matrix) const
{
	Matrix4x4 result;

	result.x.x = (x.x * matrix.x.x) + (y.x * matrix.x.y) + (z.x * matrix.x.z) + (w.x * matrix.x.w);
	result.y.x = (x.x * matrix.y.x) + (y.x * matrix.y.y) + (z.x * matrix.y.z) + (w.x * matrix.y.w);
	result.z.x = (x.x * matrix.z.x) + (y.x * matrix.z.y) + (z.x * matrix.z.z) + (w.x * matrix.z.w);
	result.w.x = (x.x * matrix.w.x) + (y.x * matrix.w.y) + (z.x * matrix.w.z) + (w.x * matrix.w.w);

	result.x.y = (x.y * matrix.x.x) + (y.y * matrix.x.y) + (z.y * matrix.x.z) + (w.y * matrix.x.w);
	result.y.y = (x.y * matrix.y.x) + (y.y * matrix.y.y) + (z.y * matrix.y.z) + (w.y * matrix.y.w);
	result.z.y = (x.y * matrix.z.x) + (y.y * matrix.z.y) + (z.y * matrix.z.z) + (w.y * matrix.z.w);
	result.w.y = (x.y * matrix.w.x) + (y.y * matrix.w.y) + (z.y * matrix.w.z) + (w.y * matrix.w.w);

	result.x.z = (x.z * matrix.x.x) + (y.z * matrix.x.y) + (z.z * matrix.x.z) + (w.z * matrix.x.w);
	result.y.z = (x.z * matrix.y.x) + (y.z * matrix.y.y) + (z.z * matrix.y.z) + (w.z * matrix.y.w);
	result.z.z = (x.z * matrix.z.x) + (y.z * matrix.z.y) + (z.z * matrix.z.z) + (w.z * matrix.z.w);
	result.w.z = (x.z * matrix.w.x) + (y.z * matrix.w.y) + (z.z * matrix.w.z) + (w.z * matrix.w.w);

	result.x.w = (x.w * matrix.x.x) + (y.w * matrix.x.y) + (z.w * matrix.x.z) + (w.w * matrix.x.w);
	result.y.w = (x.w * matrix.y.x) + (y.w * matrix.y.y) + (z.w * matrix.y.z) + (w.w * matrix.y.w);
	result.z.w = (x.w * matrix.z.x) + (y.w * matrix.z.y) + (z.w * matrix.z.z) + (w.w * matrix.z.w);
	result.w.w = (x.w * matrix.w.x) + (y.w * matrix.w.y) + (z.w * matrix.w.z) + (w.w * matrix.w.w);

	return result;
}

Vector3 Matrix4x4::operator * (const Vector3& vector) const
{
	float vw = (x.w * vector.x) + (y.w * vector.y) + (z.w * vector.z) + w.w;

	if (FLOAT_IS_ZERO(vw) == true)
	{
		return Vector3::Zero();
	}

	vw = FLOAT_RECIPROCAL(vw);

	Vector3 result;

	result.x = ((x.x * vector.x) + (y.x * vector.y) + (z.x * vector.z) + w.x) * vw;
	result.y = ((x.y * vector.x) + (y.y * vector.y) + (z.y * vector.z) + w.y) * vw;
	result.z = ((x.z * vector.x) + (y.z * vector.y) + (z.z * vector.z) + w.z) * vw;

	return result;
}

Vector4 Matrix4x4::operator * (const Vector4& vector) const
{
	Vector4 result;

	result.x = (x.x * vector.x) + (y.x * vector.y) + (z.x * vector.z) + (w.x * vector.w);
	result.y = (x.y * vector.x) + (y.y * vector.y) + (z.y * vector.z) + (w.y * vector.w);
	result.z = (x.z * vector.x) + (y.z * vector.y) + (z.z * vector.z) + (w.z * vector.w);
	result.w = (x.w * vector.x) + (y.w * vector.y) + (z.w * vector.z) + (w.w * vector.w);

	return result;
}

Vector4& Matrix4x4::operator [](uint32_t column)
{
	ASSERT(column < 4);

	switch (column)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	}

	return w;
}

float& Matrix4x4::operator ()(uint32_t column, uint32_t row)
{
	ASSERT(column < 4);
	ASSERT(row < 4);

	switch (column)
	{
	case 0:
		return x.data[row];
	case 1:
		return y.data[row];
	case 2:
		return z.data[row];
	}

	return w.data[row];
}
