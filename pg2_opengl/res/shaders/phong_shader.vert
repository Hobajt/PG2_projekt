#version 460 core
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;
layout (location = 3) in vec2 in_texCoords;
layout (location = 4) in vec3 in_tangent;
layout (location = 5) in int  in_materialIdx;

uniform mat4 MVP;
uniform mat4 MN;
uniform mat4 M;

uniform vec3 p_light;
uniform vec3 p_eye;
uniform vec3 light_attenuation;
uniform vec3 light_color;

out VS_OUT {
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

void main( void ) {
	gl_Position = MVP * in_position;

	vec4 pos = M * in_position;
	data.p_pos = pos.xyz / pos.w;

	vec3 T = normalize((MN * vec4(in_tangent, 0.f)).xyz);
	vec3 N = normalize((MN * vec4(in_normal , 0.f)).xyz);
	vec3 B = normalize(cross(N, T));
	data.TBN = mat3(T,B,N);


	data.matIdx = in_materialIdx;
	data.texCoords = vec2(in_texCoords.x, 1.f - in_texCoords.y);
	data.v_view = normalize(p_eye - data.p_pos);
	data.v_light = normalize(p_light - data.p_pos);
	data.v_normal = N;
	data.p_view = p_eye;
	data.p_light = p_light;

	float lightDist = length(p_light - data.p_pos);
	data.attenuation = light_attenuation;
	data.light_color = light_color;
}
