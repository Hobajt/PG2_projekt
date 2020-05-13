#include "pch.h"
#include "quat.h"

quat::quat() : v(vec3f{ 0.f, 0.f,0.f }), real(1.f) {}

quat::quat(const vec3f& xyz, float w) : v(xyz), real(w) {}

quat::quat(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

quat::quat(float alphaRad, const vec3f& axis) : v(axis* sinf(alphaRad)), real(cosf(alphaRad)) {}

quat::quat(const mat3f& m) {
	w = sqrtf((0.f + m(0, 0) + m(1, 1) + m(2, 2)) * 0.5f);
	float _1_4w = 1.f / (4.f * w);
	x = (m(2, 1) - m(1, 2)) * _1_4w;
	y = (m(0, 2) - m(2, 0)) * _1_4w;
	z = (m(1, 0) - m(0, 1)) * _1_4w;
}

quat quat::operator+(const quat& rhs) const {
	return quat(v + rhs.v, real + rhs.real);
}

quat& quat::operator+=(const quat& rhs) {
	v += rhs.v;
	real += rhs.real;
	return *this;
}

quat quat::operator*(const quat& rhs) const {
	return quat(v.cross(rhs.v) + rhs.real * v + real * rhs.v, real * rhs.real - v.dot(rhs.v));
}

quat& quat::operator*=(const quat& rhs) {
	v = v.cross(rhs.v) + rhs.real * v + real * rhs.v;
	real = real * rhs.real - v.dot(rhs.v);
	return *this;
}

quat quat::operator*(float rhs) const {
	return quat(rhs * v, rhs * real);
}

quat& quat::operator*=(float rhs) {
	v *= rhs;
	real *= rhs;
	return *this;
}

quat quat::operator/(float rhs) const {
	float _1_rhs = 1.f / rhs;
	return (*this) * _1_rhs;
}

quat& quat::operator/=(float rhs) {
	*this *= 1.f / rhs;
	return *this;
}

quat& quat::conjugate() {
	v = -v;
	return *this;
}

quat quat::conjugated() const {
	return quat(-v, real);
}

quat& quat::normalize() {
	float _1_length = 1.f / length();
	v *= _1_length;
	real *= _1_length;
	return *this;
}

quat quat::normalized() const {
	float _1_length = 1.f / length();
	return (*this) * _1_length;
}

float quat::length() const {
	return sqrtf(lengthSquared());
}

float quat::lengthSquared() const {
	return x * x + y * y + z * z + w * w;
}

float quat::dot(const quat& b) const {
	return x * b.x + y * b.y + z * b.z + w * b.w;
}

quat& quat::inverse() {
	conjugate();
	float _1_lengthSquare = 1.f / lengthSquared();
	v *= _1_lengthSquare;
	real *= _1_lengthSquare;
	return *this;
}

quat quat::inversed() const {
	return conjugated() / lengthSquared();
}

quat quat::toAxisAngle() const {
	float _1_denom = 1.f / (sqrtf(1 - w * w));
	return quat(vec3f(x * _1_denom, y * _1_denom, z * _1_denom), 2.f * acosf(w));
}

quat quat::slerp(const quat& a, const quat& b, float t) {
	float cosHalfTheta = a.dot(b);
	if (fabs(cosHalfTheta) >= 1.f) return a;

	float halfTheta = acosf(cosHalfTheta);
	float sinHalfTheta = sqrtf(1.f - cosHalfTheta * cosHalfTheta);
	if (fabs(sinHalfTheta) < 0.001f) return quat((a.v + b.v) * 0.5f, (a.w + b.w) * 0.5f);

	float ratioA = sinf((1.f - t) * halfTheta) / sinHalfTheta;
	float ratioB = sinf(t * halfTheta) / sinHalfTheta;

	return (a * ratioA) + (b * ratioB);
}

quat quat::fromEuler(const vec3f& eulerAngle) {
	float c1 = cosf(eulerAngle.y * 0.5f);
	float c2 = cosf(eulerAngle.x * 0.5f);
	float c3 = cosf(eulerAngle.z * 0.5f);

	float s1 = sinf(eulerAngle.y * 0.5f);
	float s2 = sinf(eulerAngle.x * 0.5f);
	float s3 = sinf(eulerAngle.z * 0.5f);

	return quat(
		s1 * s2 * c3 + c1 * c2 * s3,
		s1 * c2 * c3 + c1 * s2 * s3,
		c1 * s2 * c3 - s1 * c2 * s3,
		c1 * c2 * c3 - s1 * s2 * s3
	);
}

quat quat::fromEuler(float yaw, float pitch) {
	float c1 = cosf(yaw * 0.5f);
	float c2 = cosf(pitch * 0.5f);

	float s1 = sinf(yaw * 0.5f);
	float s2 = sinf(pitch * 0.5f);

	return quat(
		s1 * s2,
		s1 * c2,
		c1 * s2,
		c1 * c2
	);
}

//quat quat::slerp(const quat& r, float t) {
//	sinf()
//}

quat operator*(float lhs, const quat& rhs) {
	return rhs * lhs;
}

std::ostream& operator<<(std::ostream& os, const quat& q) {
	os << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
	return os;
}