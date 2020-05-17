#include "pch.h"
#include "camera.h"
#include "mymath.h"

constexpr float cam_rotationYLimit = (float)(M_PI_2 * 0.95);
constexpr float cam_maxMouseSpeed = 80.f;
constexpr float cam_movementSpeed = 50.f;
constexpr float cam_rotationSpeed = 10.f;
constexpr float cam_zoomSpeed = 10.f;
constexpr float cam_curveMovementSpeed = 0.1f;

//variables that pass info from callbacks to CamCtrl
vec2f mousePos;
bool isScrolling;
float mouseScroll;

//================================= Camera =================================

Camera::Camera(const int width, const int height, const float fov_y, const Vector3& view_from, const Vector3& view_at, float nearPlane, float farPlane) {
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	viewFrom = view_from;
	viewAt = view_at;

	n = nearPlane;
	f = farPlane;

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

//================================= CameraController =================================

void GLFWcursorCallback(GLFWwindow* window, double x, double y) {
	mousePos = { static_cast<float>(x), static_cast<float>(y) };
}

void GLFWscrollCallback(GLFWwindow* window, double x, double y) {
	mouseScroll = -static_cast<float>(y);
	isScrolling = true;
}

CameraController::CameraController(Camera& cam, GLFWwindow* win) : camera(&cam), window(win) {
	glfwSetCursorPosCallback(window, GLFWcursorCallback);
	glfwSetScrollCallback(window, GLFWscrollCallback);

	originInitial = camera->viewFrom;
	targetInitial = camera->viewAt;

	Reset();
}

void CameraController::Update(float deltaTime) {
	//camera reset button update
	if (resetBtn.update(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)) {
		Reset();
		return;
	}

	//toggle update
	if (movementToggle.update(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)) {
		curveMovement = !curveMovement;
	}

	if (!curveMovement) {
		//manual movement controls
		ManualMovement(deltaTime);
	}
	else {
		//movement on curve
		CurveMovement(deltaTime);
	}
}

void CameraController::Reset() {
	//reset target/origin positions
	camera->viewFrom = originInitial;
	camera->viewAt = targetInitial;
	viewOffset = targetInitial - originInitial;

	//get initial distance
	viewDistance = viewOffset.L2Norm();

	//get initial rotation
	vec3f d = viewOffset.normalized();
	//rotation = vec2f{atan2f(d.y, d.x) + (float)M_PI_2, acosf(d.z) };
	rotation = vec2f{ (float)M_PI, 0.f };

	zoom = 1.f;

	mat3f M = mat3f::EulerZ(rotation.x) * mat3f::EulerX(-rotation.y);
	viewOffset = M * vec3f{ 0.f, viewDistance, 0.f };
}

void CameraController::ManualMovement(float dt) {
	Camera& cam = *camera;

	//read WASD controls
	float y = static_cast<float>((int)(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) - (int)(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS));
	float x = static_cast<float>((int)(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) - (int)(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS));

	//read mouse input
	vec2f mouseDelta = (mousePos - mouseLast).clamp(-cam_maxMouseSpeed, cam_maxMouseSpeed) * (1.f / cam_maxMouseSpeed);
	mouseLast = mousePos;

	//WASD movement - XZ plane
	if (fabsf(x) + fabsf(y) > 0.1f) {
		vec3f move = (cam.viewZ * -y + cam.viewX * x).normalize() * (dt * cam_movementSpeed);
		cam.viewFrom += move;
		cam.viewAt += move;
	}

	//mouse movement (hold LMB) - XY plane
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		vec3f move = (cam.viewY * mouseDelta.y + cam.viewX * -mouseDelta.x).normalize() * (dt * cam_movementSpeed);
		cam.viewFrom += move;
		cam.viewAt += move;
	}

	//mouse rotation (hold RMB)
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		rotation += mouseDelta * dt * cam_rotationSpeed;
		rotation.y = std::clamp(rotation.y, -cam_rotationYLimit, cam_rotationYLimit);

		mat3f M = mat3f::EulerZ(rotation.x) * mat3f::EulerX(-rotation.y);
		viewOffset = M * vec3f{ 0.f, viewDistance, 0.f };
		cam.viewFrom = cam.viewAt + (viewOffset * zoom);
	}

	//zoom by scrolling
	if (isScrolling) {
		zoom += mouseScroll * dt * cam_zoomSpeed;
		if (zoom < 0.1f) zoom = 0.1f;
		cam.viewFrom = cam.viewAt + (viewOffset * zoom);
		isScrolling = false;
	}
}

void CameraController::CurveMovement(float dt) {
	t += dt * cam_curveMovementSpeed;
	while (t > 1.f)
		t -= 1.f;

	Camera& cam = *camera;
	cam.viewFrom = cam.viewAt + (viewDistance * movementCurve.Sample(t));
}