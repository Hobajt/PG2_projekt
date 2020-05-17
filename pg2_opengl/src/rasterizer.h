#pragma once

#include "vector3.h"
#include "camera.h"
#include "scene.h"
#include "shader.h"
#include "Light.h"
#include "texture.h"

struct GLFWwindow;

class Rasterizer {
public:
	Rasterizer(int width, int height, float fovY_deg, const vec3f& viewFrom, const vec3f& viewAt, float nearPlane, float farPlane);

	void LoadScene(const char* filepath);
	void LoadShader(const char* vShaderPath, const char* fShaderPath);

	void LoadIrradianceMap(const char* filepath);
	void LoadPrefilteredEnvMap(const std::initializer_list<const char*>& filepaths);
	void LoadGGXIntegrationMap(const char* filepath);

	int MainLoop();

	void OnFramebufferResize(int width, int height);

	Light& SceneLight() { return light; }
private:
	//OpenGL context initialization.
	void InitDevice();
	void UpdateDeltaTime();
public:
	GLFWwindow* window;
	Camera camera;
	Scene scene;
	ShaderProgram shader;
	Light light;

	BindlessTexture tex_irrMap;
	BindlessTexture tex_envMap;
	BindlessTexture tex_intMap;

	float deltaTime;
};
