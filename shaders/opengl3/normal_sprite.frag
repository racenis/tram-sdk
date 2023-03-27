#version 400 core
out vec4 fragColor;
  
in vec2 vertUV;
flat in uint texIndex;

uniform sampler2D sampler[16];

void main()
{
    fragColor = texture(sampler[texIndex], vertUV);
	if (fragColor.w < 0.9) discard;
}