#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform vec3 objColor;

vec3 ambientColor = objColor * ambientStrength;

const int levels = 5;
const float scaleFactor = 1.0 / levels;
vec3 diffuseColor = objColor;


void main()
{
	vec3 norm = normalize(Normal);
	vec3 L = normalize(lightPos - FragPos);
	vec3 V = normalize(viewPos - FragPos);

	float diffuse = max(0, dot(L, norm));
	diffuseColor = diffuseColor * 0.5 * floor(diffuse * levels) * scaleFactor;

	vec3 H = normalize(L + V);

	float specular = 0.0;

	if (dot(L, Normal) > 0.0)
	{
		specular = 0.5 * pow(max(0, dot(H, norm)), 16);
	}

	float specMask = (pow(dot(H, norm), 16) > 0.4) ? 1 : 0;

	float edgeDetection = (dot(V, norm) > 0.2) ? 1 : 0;

	vec3 color = edgeDetection * (diffuseColor + specular * specMask) + ambientColor;

	FragColor = vec4(color, 1);

}