#include "pch.h"
#include "tutorials.h"

#include "rasterizer.h"

#define AVENGER_SCENE

int main() {
	printf("PG2 OpenGL, (c)2019 Tomas Fabian\n\n");

#ifdef AVENGER_SCENE
	Rasterizer rasterizer(640, 480, 45.f, vec3f{ 0,300, 200 }, vec3f{ 0.f, 0.f, 30.f }, 1.f, 1000.f);
	//Rasterizer rasterizer(640, 480, 45.f, vec3f{ 75, 140, -150 }, vec3f{ 0.f, 0.f, 30.f }, 1.f, 1000.f);
	//Rasterizer rasterizer(640, 480, 45.f, vec3f{ 0,0,0 }, vec3f{ 0.f, 0.f, 100.f }, 1.f, 1000.f);
	rasterizer.LoadScene("res/models/avenger/6887_allied_avenger_gi2.obj");
	rasterizer.LoadShader("res/shaders/shader2.vert", "res/shaders/shader2.frag");
#else
	Rasterizer rasterizer(640, 480, 45.f, vec3f{ 0.f, 0.f, -10.f }, vec3f{ 0.f, 0.f, 0.f }, 0.1f, 100.f);
	rasterizer.LoadScene("default");
	rasterizer.LoadShader("res/shaders/basic_shader.vert", "res/shaders/basic_shader.frag");
#endif

	return rasterizer.MainLoop();
	//return tutorial_1();
}
