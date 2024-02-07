#version 430 core

uniform sampler2D texture1;

out vec4 out_color;
in vec2 fragmentTexCoord;
void main()
{
	vec3 color = texture2D(texture1, fragmentTexCoord).rgb;
	out_color = vec4(0.9, 0.9, 0.05, 1.0);
}