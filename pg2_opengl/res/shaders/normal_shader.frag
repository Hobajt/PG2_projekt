#version 460 core
out vec4 FragColor;

in vec3 normal_es;

void main( void ) {
	FragColor = vec4( normal_es.rgb, 1.0f );
//	FragColor = vec4( normal_es.r, normal_es.g, normal_es.r, 1.f);
}
