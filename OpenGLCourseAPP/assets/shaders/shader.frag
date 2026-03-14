#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

struct Material {
	float specularIntensity;
	float shininess;
};

uniform sampler2D tex;
uniform Material material;

void main()
{
	//vec4 finalColor = calcDirectionalLight();
	//finalColor += calcPointLights();
	//finalColor += calcSpotLights();
	//color = texture(tex, TexCoord) * finalColor;
	color = texture(tex, TexCoord) * vec4(1, 1, 1, 1); // blanco fijo
}