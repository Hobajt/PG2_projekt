#include "pch.h"
#include "tutorials.h"

#include "rasterizer.h"

constexpr int width = 1280;
constexpr int height = 960;

#define SCENE_TYPE 2

//scenes = 0= triangle, 1= avenger, 2= piece02

int main() {
	printf("PG2 OpenGL, (c)2019 Tomas Fabian\n\n");

#if SCENE_TYPE == 0
	Rasterizer rasterizer(width, height, 45.f, vec3f{ 0.f, 0.f, -10.f }, vec3f{ 0.f, 0.f, 0.f }, 0.1f, 100.f);
	rasterizer.LoadScene("default");
	rasterizer.LoadShader("res/shaders/basic_shader.vert", "res/shaders/basic_shader.frag");
#elif SCENE_TYPE == 1
	//Rasterizer rasterizer(width, height, 45.f, vec3f{200,300,400}, vec3f{ 0.f, 0.f, 30.f }, 1.f, 1000.f);
	Rasterizer rasterizer(width, height, 45.f, vec3f{ 200, 160, 200 }, vec3f{ 0.f, 0.f, -20.f }, 1.f, 1000.f);
	//Rasterizer rasterizer(width, height, 45.f, vec3f{ 0,0,0 }, vec3f{ 0.f, 0.f, 100.f }, 1.f, 1000.f);
	rasterizer.LoadScene("res/models/avenger/6887_allied_avenger_gi2.obj");
	rasterizer.LoadShader("res/shaders/shader2.vert", "res/shaders/shader2.frag");
	//rasterizer.LoadShader("res/shaders/normal_shader.vert", "res/shaders/normal_shader.frag");
#elif SCENE_TYPE == 2
	Rasterizer rasterizer(width, height, 45.f, vec3f{ 40, 20, 40.f }, vec3f{ 0.f, 0.f, 0.f }, 1.f, 1000.f);
	rasterizer.LoadScene("res/models/piece_02/piece_02.obj");
	//rasterizer.LoadShader("res/shaders/normal_shader.vert", "res/shaders/normal_shader.frag");
	rasterizer.LoadShader("res/shaders/shader2.vert", "res/shaders/shader2.frag");
#endif

	return rasterizer.MainLoop();
	//return tutorial_1();
}
