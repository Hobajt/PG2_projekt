#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require	//uint64_t

out vec4 FragColor;

in vec3 normal_es;
in vec2 texCoord;
flat in int matIdx;

struct Material {
	vec3 diffuse;
	uint64_t texDiffuse;
};

layout(std430, binding = 0) readonly buffer Materials {
	Material materials[];
};

void main( void ) {
	//FragColor = vec4(materials[matIdx].diffuse.rgb, 1.0f);
	vec3 clr = materials[matIdx].diffuse.rgb * texture(sampler2D(materials[matIdx].texDiffuse), texCoord).rgb;
	FragColor = vec4(clr.rgb, 1.0f);
}
