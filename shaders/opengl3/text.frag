#version 400 core

out vec4 fragColor;

in vec2 vertUV;
in vec3 color;
in vec2 cscale;
in float thick;
flat in uint texindex;

uniform sampler2D sampler[16];

void main()
{
	vec4 t = texture(sampler[texindex], vertUV);
	vec2 c = vec2(max(min(t.r, t.g), min(max(t.r, t.g), t.b)));
	float scale = min(cscale.x, cscale.y);
	c.y = texture(sampler[texindex], vertUV + vec2(-2.0/256.0/scale/scale, 2.0/256.0/scale/scale)).w;
	c = scale * (c - 0.5);
	c.x += thick;
	c = clamp(c + 0.5, 0.0, 1.0);
	//if (c.x == 0 || c.y == 0) discard;
	if (c.x == 0) discard;
	fragColor = vec4(mix(vec3(0.0, 0.0, 0.0), color * 2.0, c.x), 1.0);
}
