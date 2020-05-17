#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require	//uint64_t

//====== Constants ======
float PI = 3.14159;
float _1_PI = (1.0 / PI);
float _1_2PI = (1.0 / (2*PI));
//=======================

in VS_OUT {
	flat int matIdx;
	vec2 texCoords;

	vec3 v_normal;
	vec3 v_light;
	vec3 v_view;

	vec3 p_pos;
	vec3 p_view;
	vec3 p_light;

	vec3 attenuation;
	vec3 light_color;

	mat3 TBN;
} data;

out vec4 FragColor;

uniform uint64_t tex_irradianceMap;
uniform uint64_t tex_environmentMap;

//====== Material structure ======
struct Material {
	vec3 diffuse;
	uint64_t texDiffuse;

	vec3 rma;
	uint64_t texRma;

	vec3 normal;
	uint64_t texNormal;
};

layout(std430, binding = 0) readonly buffer Materials {
	Material materials[];
};

//====== Functions ======
vec3 Tex2D(uint64_t tex, vec2 coords);

//=================================

float shininess = 16.f;

void main( void ) {
	Material mat = materials[data.matIdx];

	vec3 v_light = normalize(data.p_light - data.p_pos);
	vec3 v_view = normalize(data.p_view - data.p_pos);

	vec3 normal = data.v_normal;
	if(mat.texNormal != 0) normal = data.TBN * normalize(2.f * Tex2D(mat.texNormal, data.texCoords) - 1f);
	if(dot(normal, v_view) < 0)
		normal *= -1.f;

	vec3 v_lightRefl = reflect(-data.v_light, normal);

	vec3 clrDiffuse = mat.diffuse * Tex2D(mat.texDiffuse, data.texCoords);

	vec3 diffuse = clrDiffuse * max(dot(normal, data.v_light), 0.f);
	vec3 specular = vec3(1,1,1) * pow(max(dot(data.v_view, v_lightRefl), 0.f), shininess);
	vec3 ambient = vec3(1,1,1) * 0.1 * clrDiffuse;

	vec3 color = diffuse + specular + ambient;
	FragColor = vec4(color, 1.0f);
}

vec3 Tex2D(uint64_t tex, vec2 coords) {
	return texture(sampler2D(tex), coords).rgb;
}