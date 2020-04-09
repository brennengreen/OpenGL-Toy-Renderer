#version 330 core

out vec4 FragColor;

in vec3 FragPos;


void main()
{
	float r = sin(FragPos.x)/2 + 0.5;
	float g = cos(FragPos.y)/2 + 0.5;
	float b = tan(FragPos.z)/2 + 0.5;
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}