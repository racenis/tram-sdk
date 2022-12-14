#version 300 es
out vec4 fragColor;

in vec2 vertUV;
in vec3 color;
flat in uint texindex;

uniform sampler2D sampler[16];

void main()
{
	vec4 sampledColor = texture(sampler[texindex], vertUV);
	if (sampledColor.w < 0.99) discard;
	fragColor = sampledColor * color;
	//fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
