#version 330 core
out vec4 color;

in vec2 TexCoord;

uniform sampler2D text;
uniform vec3 textColor;


void main()
{
    float alpha = texture(text, TexCoord).r;
    color = vec4(textColor, alpha);
}