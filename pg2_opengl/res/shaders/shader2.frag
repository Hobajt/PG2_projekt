#version 460 core
out vec4 FragColor;
flat in int matIdx;

struct Material {
	vec3 diffuse;
};

layout(std430, binding = 0) readonly buffer Materials {
	Material materials[];
};

void main( void ) {
//	FragColor = vec4( 1.0f, 0.25f, 0.0f, 1.0f );
	FragColor = vec4(materials[matIdx].diffuse.rgb, 1.0f);
}
