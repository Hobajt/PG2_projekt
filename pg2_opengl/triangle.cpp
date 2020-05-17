#include "pch.h"
#include "triangle.h"

vec3f GetTangent(const vec3f& t, const vec3f& b, const vec3f& n);

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, Surface* surface) {
	vertices_[0] = v0;
	vertices_[1] = v1;
	vertices_[2] = v2;

	const vec3f e1 = v1.position - v0.position;
	const vec3f e2 = v2.position - v0.position;

	//delta u,v 1,2
	Coord2f dt1 = v1.texture_coords[0] - v0.texture_coords[0];
	Coord2f dt2 = v2.texture_coords[0] - v0.texture_coords[0];

	float _1_det = 1.f / (dt1.u*dt2.v - dt2.u*dt1.v);

	vec3f t = _1_det * ( dt2.v * e1 - dt1.v * e2);
	vec3f b = _1_det * (-dt2.u * e1 + dt1.u * e2);
	//vec3f b = v0.normal.cross(t);

	vertices_[0].tangent = GetTangent(t, b, v0.normal);
	vertices_[1].tangent = GetTangent(t, b, v1.normal);
	vertices_[2].tangent = GetTangent(t, b, v2.normal);
}

vec3f GetTangent(const vec3f& t, const vec3f& b, const vec3f& n) {
	return (t - t.dot(n) * n).normalize();
}

Vertex Triangle::vertex(const int i) {
	return vertices_[i];
}
