#pragma once

#include "vector3.h"
#include "camera.h"

struct GLFWwindow;

class Rasterizer {
public:
	Rasterizer(int width, int height, float fovY_deg, const vec3f& viewFrom, const vec3f& viewAt, float nearPlane, float farPlane);

	void LoadScene(const char* filepath);

	int MainLoop();

	void OnFramebufferResize(int width, int height);
private:
	//OpenGL context initialization.
	void InitDevice();
	void UpdateDeltaTime();
public:
	Camera camera;
	GLFWwindow* window;

	float deltaTime;
};
