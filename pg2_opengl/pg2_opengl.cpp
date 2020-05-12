#include "pch.h"
#include "tutorials.h"

#include "rasterizer.h"

int main() {
	printf("PG2 OpenGL, (c)2019 Tomas Fabian\n\n");

	Rasterizer rasterizer(640, 480, 45.f, vec3f{ 0.f, 0.f, -10.f }, vec3f{ 0.f, 0.f, 0.f }, 0.1f, 100.f);
	rasterizer.LoadScene("res/models/6887_allied_avenger_gi2.obj");
	return rasterizer.MainLoop();

	//return tutorial_1();
}
