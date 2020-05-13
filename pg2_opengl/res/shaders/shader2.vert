#version 460 core
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;
layout (location = 3) in vec2 in_texCoord;
layout (location = 4) in vec3 in_tangent;
layout (location = 5) in int in_materialIdx;

uniform mat4 MVP;
uniform mat4 MVN;

out vec3 normal_es;
out vec2 texCoord;
flat out int matIdx;

void main( void ) {
	gl_Position = MVP * in_position;
	
	vec4 tmp = MVN * vec4(in_normal.xyz, 1.0f);
	normal_es = normalize(tmp.xyz / tmp.w);

	matIdx = in_materialIdx;
	texCoord = vec2(in_texCoord.x, 1.f - in_texCoord.y);
	//texCoord = in_texCoord;
}
