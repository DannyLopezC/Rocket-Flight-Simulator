#version 330
out vec4 color;
uniform vec4 btnColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    color = texture(tex, TexCoord) * btnColor;
}