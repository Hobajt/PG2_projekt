#version 460 core
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 MVN;
//uniform mat4 P;
uniform mat4 M;

out vec3 normal_es;

void main( void ) {
	gl_Position = MVP * in_position;

//	normal_es = in_normal;


	normal_es = normalize((MVN * vec4(in_normal.xyz, 0.0f)).xyz);

//	vec4 tmp = MVN * vec4(in_normal.xyz, 1.0f);
//	normal_es = normalize(tmp.xyz / tmp.w);

	//normal_es = normalize((MVN * vec4(in_normal.xyz, 1.0f)).xyz);
//	vec4 hit_es = MV * in_position;
//	vec3 omegaI_es = normalize(hit_es.xyz / hit_es.w);
//	if(dot(normal_es, omegaI_es) > 0.0f) {
//		normal_es *= -1.0f;
//	}


}
