#include "pch.h"
#include "camera.h"
#include "mymath.h"

Camera::Camera(const int width, const int height, const float fov_y, const Vector3& view_from, const Vector3& view_at) {
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	viewFrom = view_from;
	viewAt = view_at;

	UpdateViewport(width, height);

	Update();
}

void Camera::Update() {
	viewZ = (viewFrom - viewAt).normalize();
	viewX = up_.cross(viewZ).normalize();
	viewY = viewZ.cross(viewX).normalize();

	V = mat4f(viewX, viewY, viewZ, viewFrom).euclideanInverse();
	VP = P * V;
}

void Camera::UpdateProjection(float _n, float _f) {
	n = _n;
	f = _f;

	float a = (n + f) / (n - f);
	float b = (2 * n * f) / (n - f);

	mat4f M = mat4f(
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, a, b,
		0, 0, -1, 0
	);

	float h = 2 * n * tanf(deg2rad(fov_y_) * 0.5f);
	float w = h * (width_ / (float)height_);

	mat4f N = mat4f(
		2 / w, 0, 0, 0,
		0, -2 / h, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	P = N * M;
}

void Camera::UpdateViewport(int w, int h) {
	width_ = w;
	height_ = h;
	f_y_ = height_ / (2.0f * tanf(fov_y_ * 0.5f));
	UpdateProjection(n, f);
}

void Camera::MoveForward(float dt) {
	Vector3 ds = viewAt - viewFrom;
	ds.Normalize();
	ds *= dt;

	viewFrom += ds;
	viewAt += ds;
}