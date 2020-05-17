#include "pch.h"
#include "tutorials.h"

#include "rasterizer.h"

constexpr int width = 640;
constexpr int height = 480;

#define SCENE_TYPE 2
#define SHADER_TYPE 1

//scenes = 0= triangle, 1= avenger, 2= piece02
//shaders = 0= normal, 1= cookTorrance

int main() {
	printf("PG2 OpenGL, (c)2019 Tomas Fabian\n\n");

#if SCENE_TYPE == 0
	Rasterizer rasterizer(width, height, 45.f, vec3f{ 0.f, 0.f, -10.f }, vec3f{ 0.f, 0.f, 0.f }, 0.1f, 100.f);
	rasterizer.LoadScene("default");
#elif SCENE_TYPE == 1
	Rasterizer rasterizer(width, height, 45.f, vec3f{ 100, -200, 100 }, vec3f{ 0.f, 20.f, 20.f }, 1.f, 1000.f);
	rasterizer.LoadScene("res/models/avenger/6887_allied_avenger_gi2.obj");
	rasterizer.SceneLight().position = vec3f{ 50.f, 50.f, 30.f };
	rasterizer.SceneLight().attenuation = vec3f{ 1.f, 0.f, 0.f };
#elif SCENE_TYPE == 2
	Rasterizer rasterizer(width, height, 45.f, vec3f{ 30.f, -30.f, 15.f }, vec3f{ 0.f, 0.f, 0.f }, 1.f, 1000.f);
	rasterizer.LoadScene("res/models/piece_02/piece_02.obj");
	rasterizer.SceneLight().position = vec3f{ 20.f, 20.f, 15.f };
#endif

#if SHADER_TYPE == 0
	rasterizer.LoadShader("res/shaders/normal_shader.vert", "res/shaders/normal_shader.frag");
#elif SHADER_TYPE == 1
	rasterizer.LoadShader("res/shaders/ct_shader.vert", "res/shaders/ct_shader.frag");
#elif SHADER_TYPE == 2
	rasterizer.LoadShader("res/shaders/phong_shader.vert", "res/shaders/phong_shader.frag");
#endif

	rasterizer.LoadIrradianceMap("res/maps/lebombo_irradiance_map.exr");
	rasterizer.LoadPrefilteredEnvMap({
		"res/maps/lebombo_prefiltered_env_map_001_2048.exr",
		"res/maps/lebombo_prefiltered_env_map_010_1024.exr",
		"res/maps/lebombo_prefiltered_env_map_100_512.exr",
		"res/maps/lebombo_prefiltered_env_map_250_256.exr",
		"res/maps/lebombo_prefiltered_env_map_500_128.exr",
		"res/maps/lebombo_prefiltered_env_map_750_64.exr",
		"res/maps/lebombo_prefiltered_env_map_999_32.exr"
	});
	rasterizer.LoadGGXIntegrationMap("res/maps/brdf_integration_map_ct_ggx.exr");

	return rasterizer.MainLoop();
	//return tutorial_1();
}
