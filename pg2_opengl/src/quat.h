#pragma once

#include "vector3.h"
#include "matrix3x3.h"

struct quat {
public:
	union {
		float value[4];
		struct { float x, y, z, w; };
		struct { vec3f v; float real; };
	};
public:
	//creates unit quatertion
	quat();
	quat(const vec3f& xyz, float w);
	quat(float x, float y, float z, float w);

	//convert axis & angle rotation to quaternion
	quat(float alphaRad, const vec3f& axis);
	quat(const mat3f& mat);

	//addition
	quat operator +(const quat& rhs) const;
	quat& operator +=(const quat& rhs);

	//multiplication (rotation cummulation)
	quat operator *(const quat& rhs) const;
	quat& operator *=(const quat& rhs);

	//multiplication by scalar
	quat operator *(float rhs) const;
	quat& operator *=(float rhs);
	quat operator /(float rhs) const;
	quat& operator /=(float rhs);

	//komplexne sdruzene cislo myslim
	quat& conjugate();
	quat conjugated() const;

	quat& normalize();
	quat normalized() const;

	float length() const;
	float lengthSquared() const;

	float dot(const quat& b) const;

	quat& inverse();
	quat inversed() const;

	//Returns axis angle equal to this quaternion (v = axis; real = angle, 0-2pi)
	//No zero devision checks are done.
	quat toAxisAngle() const;

	static quat slerp(const quat& a, const quat& b, float t);
	static quat fromEuler(const vec3f& eulerAngle);
	static quat fromEuler(float yaw, float pitch);

	friend std::ostream& operator << (std::ostream& os, const quat& q);
};

quat operator *(float lhs, const quat& rhs);