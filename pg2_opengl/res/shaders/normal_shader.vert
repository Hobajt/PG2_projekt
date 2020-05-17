#version 460 core
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;

uniform mat4 MVP;
uniform mat4 MVN;
uniform mat4 MV;

out vec3 v_normal;

void main( void ) {
	gl_Position = MVP * in_position;

	v_normal = normalize(MVN * vec4(in_normal, 0.f)).xyz;
	vec3 pos = gl_Position.xyz / gl_Position.w;

	vec4 hit_es = MV * in_position;
	vec3 omegaI_es = hit_es.xyz / hit_es.w;
	if(dot(v_normal, omegaI_es) > 0.f)
		v_normal *= -1.f;
}
