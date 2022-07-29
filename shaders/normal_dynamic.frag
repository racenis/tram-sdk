#version 400 core
out vec4 fragColor;

in vec2 vertUV;
in vec3 vertColor;
flat in uint texIndex;

uniform sampler2D sampler[16];

void main()
{
    fragColor = texture(sampler[texIndex], vertUV) * vec4(vertColor, 1.0);
}