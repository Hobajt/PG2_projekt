#include "pch.h"
#include "rasterizer.h"

#include "log.h"
#include "utils.h"
#include "glutils.h"

#include "texture.h"

double lastTime = 0.0;

InputButton wireframeToggle;
bool wireframeState = false;

//initialization functions
bool initGLFW();
bool CreateGLFWWindow(int width, int height, const char* name, GLFWwindow** out_window);
bool initGlad();
void printBasicInfo();
void GLSettings();

//callbacks
bool checkGL(const GLenum error = glGetError());
void glfwCallback(const int error, const char* description);
void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param);
void framebufferResizeCallback(GLFWwindow* window, int width, int height);

//================================= Rasterizer =================================

Rasterizer::Rasterizer(int width, int height, float fovY_deg, const vec3f& viewFrom, const vec3f& viewAt, float nearPlane, float farPlane)
	: camera(Camera(width, height, fovY_deg, viewFrom, viewAt, nearPlane, farPlane)) {
	InitDevice();
}

void Rasterizer::LoadScene(const char* filepath) {
	scene = Scene(filepath);
}

void Rasterizer::LoadShader(const char* vShaderPath, const char* fShaderPath) {
	shader = ShaderProgram(vShaderPath, fShaderPath);
}

void Rasterizer::LoadIrradianceMap(const char* filepath) {
	tex_irrMap = Texture3f::LoadBindless(filepath);
	shader.UploadARBHandle("tex_irradianceMap", tex_irrMap.handle);
}

void Rasterizer::LoadPrefilteredEnvMap(const std::initializer_list<const char*>& filepaths) {
	tex_envMap = LoadLODTextures(filepaths);
	shader.UploadARBHandle("tex_environmentMap", tex_envMap.handle);
	shader.UploadInt("envMap_maxLevel", filepaths.size());
}

void Rasterizer::LoadGGXIntegrationMap(const char* filepath) {
	tex_intMap = Texture3f::LoadBindless(filepath);
	shader.UploadARBHandle("tex_integrationMap", tex_intMap.handle);
}

int Rasterizer::MainLoop() {
	errlog("--------------------------------\n");

	CameraController camCtrl = CameraController(camera, window);
	shader.Bind();

	//Upload light
	shader.UploadFloat3("light_attenuation", light.attenuation.data);
	shader.UploadFloat3("light_color", light.color.data);
	shader.UploadFloat3("p_light", light.position.data);

	mat4f M, N, MVP, MVN, MV;

	M = mat4f(); 
	//M.so3(mat3f::EulerX((float)(M_PI * 0.5f)));
	N = mat4f::EuclideanInverse(M).transpose();

	shader.UploadMat4("M", M.data());
	shader.UploadMat4("MN", N.data());

	lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		UpdateDeltaTime();
		//glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		//wireframe input toggle
		if(wireframeToggle.update(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)) {
			wireframeState = !wireframeState;
			glPolygonMode(GL_FRONT_AND_BACK, wireframeState ? GL_LINE : GL_FILL);
		}

		//camera update - movement & matrices
		camCtrl.Update(deltaTime);
		camera.Update();
		//======================

		MV = camera.V * M;
		MVN = camera.V * N;
		MVP = camera.VP * M;

		shader.UploadMat4("MV", MV.data(), false);
		shader.UploadMat4("MVN", MVN.data(), false);
		shader.UploadMat4("MVP", MVP.data());

		shader.UploadFloat3("p_eye", camera.ViewFrom().data, false);

		scene.Draw();

		//======================
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}

void Rasterizer::OnFramebufferResize(int _width, int _height) {
	glViewport(0, 0, _width, _height);
	camera.UpdateViewport(_width, _height);
}

void Rasterizer::InitDevice() {
	if (!initGLFW()) {
		errlog("Failed to initialize GLFW.\n");
		throw std::exception("Failed to initialize GLFW.");
	}

	if (!CreateGLFWWindow(camera.GetWidth(), camera.GetHeight(), "PG2 OpenGL", &window)) {
		glfwTerminate();
		errlog("Failed to create window.\n");
		throw std::exception("Failed to create window.");
	}
	glfwSetWindowUserPointer(window, this);

	if (!initGlad()) {
		errlog("Failed to initialize Glad.\n");
		throw std::exception("Failed to initialize Glad.");
	}

	printBasicInfo();
	checkGL();

	GLSettings();
	OnFramebufferResize(camera.GetWidth(), camera.GetHeight());
}

void Rasterizer::UpdateDeltaTime() {
	double currTime = glfwGetTime();
	deltaTime = static_cast<float>(currTime - lastTime);
	lastTime = currTime;
}


//================================= Initialization functions =================================

bool initGLFW() {
	glfwSetErrorCallback(glfwCallback);
	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	return true;
}

bool CreateGLFWWindow(int width, int height, const char* name, GLFWwindow** out_window) {
	GLFWwindow*& window = *out_window;

	window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	if (window) {
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwMakeContextCurrent(window);
		return true;
	}

	return false;
}

bool initGlad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		if (!gladLoadGL()) {
			return false;
		}
	}
	return true;
}

void printBasicInfo() {
	printf("OpenGL %s, ", glGetString(GL_VERSION));
	printf("%s", glGetString(GL_RENDERER));
	printf(" (%s)\n", glGetString(GL_VENDOR));
	printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void GLSettings() {
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);		//ignore unimportant debug messages
	glDebugMessageCallback(glDebugCallback, nullptr);

	glEnable(GL_MULTISAMPLE);

	// GL_LOWER_LEFT (OpenGL) or GL_UPPER_LEFT (DirectX, Windows) and GL_NEGATIVE_ONE_TO_ONE or GL_ZERO_TO_ONE
	glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
	//glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);

	//enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glPointSize(10.0f);
	glLineWidth(2.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//================================= Callbacks =================================

/* glfw callback */
void glfwCallback(const int error, const char* description) {
	errlog("GLFW Error (%d): %s\n", error, description);
}

/* OpenGL messaging callback */
void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param) {
	errlog("GL %s type = 0x%x, severity = 0x%x, message = %s\n", (type == GL_DEBUG_TYPE_ERROR ? "Error" : "Message"), type, severity, message);
}

/* invoked when window is resized */
void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	static_cast<Rasterizer*>(glfwGetWindowUserPointer(window))->OnFramebufferResize(width, height);
}

/* OpenGL check state */
bool checkGL(const GLenum error) {
	if (error != GL_NO_ERROR) {
		//const GLubyte * error_str;
		//error_str = gluErrorString( error );
		//printf( "OpenGL error: %s\n", error_str );
		return false;
	}

	return true;
}