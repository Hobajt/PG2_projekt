#include "pch.h"
#include "rasterizer.h"

#include "log.h"
#include "utils.h"
#include "glutils.h"

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

/* load shader code from text file */
char* LoadShader(const char* file_name) {
	FILE* file = fopen(file_name, "rt");

	if (file == NULL) {
		printf("IO error: File '%s' not found.\n", file_name);

		return NULL;
	}

	size_t file_size = static_cast<size_t>(GetFileSize64(file_name));
	char* shader = NULL;

	if (file_size < 1) {
		printf("Shader error: File '%s' is empty.\n", file_name);
	}
	else {
		/* v glShaderSource nezadáváme v posledním parametru délku,
		takže øetìzec musí být null terminated, proto +1 a reset na 0*/
		shader = new char[file_size + 1];
		memset(shader, 0, sizeof(*shader) * (file_size + 1));

		size_t bytes = 0; // poèet již naètených bytù

		do {
			bytes += fread(shader, sizeof(char), file_size, file);
		} while (!feof(file) && (bytes < file_size));

		if (!feof(file) && (bytes != file_size)) {
			printf("IO error: Unexpected end of file '%s' encountered.\n", file_name);
		}
	}

	fclose(file);
	file = NULL;

	return shader;
}

/* check shader for completeness */
GLint CheckShader(const GLenum shader) {
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	printf("Shader compilation %s.\n", (status == GL_TRUE) ? "was successful" : "FAILED");

	if (status == GL_FALSE) {
		int info_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
		char* info_log = new char[info_length];
		memset(info_log, 0, sizeof(*info_log) * info_length);
		glGetShaderInfoLog(shader, info_length, &info_length, info_log);

		printf("Error log: %s\n", info_log);

		SAFE_DELETE_ARRAY(info_log);
	}

	return status;
}

//================================= Rasterizer =================================

Rasterizer::Rasterizer(int width, int height, float fovY_deg, const vec3f& viewFrom, const vec3f& viewAt, float nearPlane, float farPlane)
	: camera(Camera(width, height, fovY_deg, viewFrom, viewAt, nearPlane, farPlane)) {
	InitDevice();
}

//TODO: delet dis
GLuint shader_program;

void Rasterizer::LoadScene(const char* filepath) {
	scene = Scene(filepath);

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_shader_source = LoadShader("res/shaders/basic_shader.vert");
	glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);
	SAFE_DELETE_ARRAY(vertex_shader_source);
	CheckShader(vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_shader_source = LoadShader("res/shaders/basic_shader.frag");
	glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	SAFE_DELETE_ARRAY(fragment_shader_source);
	CheckShader(fragment_shader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	// TODO check linking
	glUseProgram(shader_program);
}

int Rasterizer::MainLoop() {

	lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		UpdateDeltaTime();
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		//wireframe input toggle
		wireframeToggle.update(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
		if (wireframeToggle.pressed()) {
			wireframeState = !wireframeState;
			glPolygonMode(GL_FRONT_AND_BACK, wireframeState ? GL_LINE : GL_FILL);
		}

		camera.Update();
		//======================

		//do stuff here
		SetMatrix4x4(shader_program, camera.VP.data(), "MVP");
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.MoveForward(0.1f);
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.MoveForward(-0.1f);

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