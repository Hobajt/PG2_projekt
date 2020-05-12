#ifndef CAMERA_H_
#define CAMERA_H_

#include "vector3.h"
#include "matrix3x3.h"
#include "matrix4x4.h"

/*! \class Camera
\brief A simple pin-hole camera.

\author Tomáš Fabián
\version 1.0
\date 2018-2019
*/
class Camera {
public:
	Camera() {}

	Camera(const int width, const int height, const float fov_y, const Vector3& view_from, const Vector3& view_at);

	//Updates view & viewProjection matrices.
	void Update();

	//Recalculates projection matrix.
	void UpdateProjection(float nearPlane, float farPlane);

	//Call on viewport change (window resize).
	void UpdateViewport(int width, int height);

	void MoveForward(float dt);
public:
	mat4f P;
	mat4f V;

	mat4f VP;

	Vector3 viewX;
	Vector3 viewY;
	Vector3 viewZ;
private:
	int width_{ 640 }; // image width (px)
	int height_{ 480 };  // image height (px)
	float fov_y_{ 0.785f }; // vertical field of view (rad)

	Vector3 viewFrom; // ray origin or eye or O
	Vector3 viewAt; // target T
	Vector3 up_{ Vector3(0.0f, 1.0f, 0.0f) }; // up vector

	float f_y_{ 1.0f }; // focal lenght (px)

	Matrix3x3 M_c_w_; // transformation matrix from CS -> WS	

	float n = 1.f;
	float f = 100.f;
};

#endif
