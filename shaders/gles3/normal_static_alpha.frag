#version 300 es

precision highp float;

out vec4 fragColor;
  
in vec2 vertUV;
in vec2 vertLightUV;
in vec3 vertColor;
flat in uint texIndex;

uniform sampler2D sampler[16];

void main()
{
	vec4 texcolor;
	
	switch (texIndex) {
	case 0u:
		texcolor = texture(sampler[0], vertUV);
		break;
	case 1u:
		texcolor = texture(sampler[1], vertUV);
		break;
	case 2u:
		texcolor = texture(sampler[2], vertUV);
		break;
	case 3u:
		texcolor = texture(sampler[3], vertUV);
		break;
	case 4u:
		texcolor = texture(sampler[4], vertUV);
		break;
	case 5u:
		texcolor = texture(sampler[5], vertUV);
		break;
	case 6u:
		texcolor = texture(sampler[6], vertUV);
		break;
	case 7u:
		texcolor = texture(sampler[7], vertUV);
		break;
	case 8u:
		texcolor = texture(sampler[8], vertUV);
		break;
	case 9u:
		texcolor = texture(sampler[9], vertUV);
		break;
	case 10u:
		texcolor = texture(sampler[10], vertUV);
		break;
	case 11u:
		texcolor = texture(sampler[11], vertUV);
		break;
	case 12u:
		texcolor = texture(sampler[12], vertUV);
		break;
	case 13u:
		texcolor = texture(sampler[13], vertUV);
		break;
	case 14u:
		texcolor = texture(sampler[14], vertUV);
		break;
	case 15u:
		texcolor = texture(sampler[15], vertUV);
		break;			
	}

	fragColor = texcolor;
	if (fragColor.a < 0.5) discard;
	fragColor *= (texture(sampler[15], vertLightUV) * vec4(vertColor, 1.0));
}
