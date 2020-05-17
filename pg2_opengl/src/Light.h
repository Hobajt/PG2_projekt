#pragma once

#include "vector3.h"

struct Light {
	vec3f position;
	vec3f attenuation = vec3f{ 1.f, 0.07f, 0.017f };	//quadratic attenuation
	vec3f color = vec3f{ 1.f, 1.f, 1.f };
};