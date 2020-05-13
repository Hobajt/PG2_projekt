#version 460 core
layout (location = 0) in vec4 position;
layout (location = 5) in int materialIdx;

uniform mat4 MVP;

flat out int matIdx;

void main( void ) {
	gl_Position = MVP * position;
	matIdx = materialIdx;
}
