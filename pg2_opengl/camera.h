#ifndef CAMERA_H_
#define CAMERA_H_

#include "vector3.h"
#include "matrix3x3.h"
#include "matrix4x4.h"

#include "utils.h"
#include "quat.h"

struct GLFWwindow;

/*! \class Camera
\brief A simple pin-hole camera.

\author Tomáš Fabián
\version 1.0
\date 2018-2019
*/
class Camera {
	friend class CameraController;
public:
	Camera() {}

	Camera(const int width, const int height, const float fov_y, const Vector3& view_from, const Vector3& view_at, float nearPlane = 1.f, float farPlane = 100.f);

	//Updates view & viewProjection matrices.
	void Update();

	//Recalculates projection matrix.
	void UpdateProjection(float nearPlane, float farPlane);

	//Call on viewport change (window resize).
	void UpdateViewport(int width, int height);

	inline int GetWidth() const { return width_; }
	inline int GetHeight() const { return height_; }
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

	float n = 0.1f;
	float f = 100.f;
};


class CameraController {
public:
	CameraController(Camera& cam, GLFWwindow* window);
	void Update(float deltaTime);
	void Reset();
private:
	void ManualMovement(float deltaTime);
private:
	Camera* camera;
	GLFWwindow* window;

	InputButton resetBtn = {};
	InputButton movementToggle = {};
	bool curveMovement = false;

	vec2f mouseLast = vec2f{ 0.f, 0.f };

	//current rotation
	vec2f rotation;

	//original camera viewFrom/viewAt coords
	vec3f originInitial;
	vec3f targetInitial;

	vec3f viewOffset;		//vector from viewAt to viewFrom
	float viewDistance;		//original distance between viewAt & from
	float zoom;
};
#endif
