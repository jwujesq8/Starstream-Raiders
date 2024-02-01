#version 430 core

uniform vec3 color;
uniform float shotPower;

out vec4 out_color;
void main()
{
	
	out_color = vec4(mix(vec3(0.0), color, shotPower), 1.0);
}
