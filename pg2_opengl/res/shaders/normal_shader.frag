#version 460 core

out vec4 FragColor;

in vec3 v_normal;

void main( void ) {
//	vec3 clr = vec3(v_normal.b, v_normal.r, v_normal.g);	
	vec3 clr = (v_normal+1)*0.5f;
	FragColor = vec4(clr, 1.f);
}
