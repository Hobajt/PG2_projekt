#include "pch.h"
#include "Curves.h"

mat4f BezierCubic::F = mat4f(
	-1, 3, -3, 1,
	3, -6, 3, 0,
	-3, 3, 0, 0,
	1, 0, 0, 0
);

BezierSpline::BezierSpline() {
	//sort of circle on xz plane
	/*segments.push_back(BezierCubic(vec3f{ 0.f,0.f, 1.f }, vec3f{ 1.33f,0.f, 1.f }, vec3f{ 1.33f, 0.f, -1.f }, vec3f{ 0.f, 0.f, -1.f }));
	AddSegment(vec3f{ -1.33f, 0.f, 1.f }, vec3f{ 0.f, 0.f, 1.f });*/

	//sort of circle tilted in y axis
	segments.push_back(BezierCubic(vec3f{ 0.f, -1.f, 0.f }, vec3f{ 1.33f, -1.f, -0.77f }, vec3f{ 1.33f, 1.f, -0.77f }, vec3f{ 0.f, 1.f, 0.f }));
	AddSegment(vec3f{ -1.33f, -1.f, 0.77f }, vec3f{ 0.f, -1.f, 0.f });
}

BezierSpline::BezierSpline(const BezierCubic& first) {
	segments.push_back(first);
}

void BezierSpline::AddSegment(const vec3f& p2, const vec3f& p3) {
	segments.push_back(segments.back().Append(p2, p3));
}

vec3f BezierSpline::Sample(float t) {
	t = t * segments.size();
	int i = 0;
	while (t > 1.f) {
		t -= 1.f;
		i++;
	}

	return segments[i].Sample(t);
}

BezierCubic::BezierCubic(const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec3f& p3) : pts(mat4f(p0, p1, p2, p3)) {
	pts(3, 3) = 0.f;
	pts.transpose();
}

BezierCubic::BezierCubic(const mat4f& s, const vec3f& p2, const vec3f& p3) {
	vec3f p0 = vec3f{ s(3,0), s(3,1), s(3,2) };
	vec3f p1 = 2 * p0 - vec3f{ s(2,0), s(2,1), s(2,2) };

	pts = mat4f(p0, p1, p2, p3);
	pts(3, 3) = 0.f;
	pts.transpose();
}

BezierCubic BezierCubic::Append(const vec3f& p2, const vec3f& p3) {
	return BezierCubic(pts, p2, p3);
}

vec3f BezierCubic::Sample(float t) {
	vec4f tVec = vec4f{ t * t * t, t * t, t, 1 };
	vec4f res = (tVec * F * pts);
	return vec3f{ res.x, res.y, res.z };
}
