#version 400 core
out vec4 fragColor;

in vec2 vertUV;
in vec3 color;
flat in uint texindex;

uniform sampler2D sampler[16];

void main()
{
	fragColor = texture(sampler[texindex], vertUV);
	//fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
