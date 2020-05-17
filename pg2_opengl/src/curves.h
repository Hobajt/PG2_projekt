#pragma once

#include <vector>

#include "vector3.h"
#include "matrix4x4.h"

class BezierCubic {
public:
	BezierCubic(const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3);
	BezierCubic(const mat4f& prevSegment, const vec3f& p2, const vec3f& p3);
	//Creates new cubic which starts where this one ends (+ has C1 continuity)
	BezierCubic Append(const vec3f& p2, const vec3f& p3);

	vec3f Sample(float t);
private:
	mat4f pts;

	static mat4f F;
};

class BezierSpline {
public:
	BezierSpline();
	BezierSpline(const BezierCubic& first);
	void AddSegment(const vec3f& p2, const vec3f& p3);

	vec3f Sample(float t);
private:
	std::vector<BezierCubic> segments;
};
