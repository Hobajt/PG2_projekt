#version 460 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;

uniform mat4 MVP;

void main( void ) {
	gl_Position = MVP * position;
}
