#pragma once

#include "vector3.h"
#include "camera.h"
#include "scene.h"
#include "shader.h"

struct GLFWwindow;

class Rasterizer {
public:
	Rasterizer(int width, int height, float fovY_deg, const vec3f& viewFrom, const vec3f& viewAt, float nearPlane, float farPlane);

	void LoadScene(const char* filepath, const char* vShaderPath, const char* fShaderPath);

	int MainLoop();

	void OnFramebufferResize(int width, int height);
private:
	//OpenGL context initialization.
	void InitDevice();
	void UpdateDeltaTime();
public:
	GLFWwindow* window;
	Camera camera;
	Scene scene;
	ShaderProgram shader;

	float deltaTime;
};
