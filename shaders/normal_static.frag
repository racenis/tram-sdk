#version 400 core
out vec4 fragColor;
  
in vec2 vertUV;
in vec2 vertLightUV;
in vec3 vertColor;
flat in uint texIndex;

uniform sampler2D sampler[16];

void main()
{
    fragColor = texture(sampler[texIndex], vertUV) * texture(sampler[15], vertLightUV) * vec4(vertColor, 1.0);
}