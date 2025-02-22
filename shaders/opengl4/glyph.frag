// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

out vec4 fragment;

in vec2 vert_uv;
in vec3 vert_color;
flat in uint vert_tex_index;

uniform sampler2D sampler[16];

void main() {
	vec4 sampled_color = texture(sampler[vert_tex_index], vert_uv);
	
	if (sampled_color.w < 1.0) discard;
	
	fragment = sampled_color * vec4(vert_color, 1.0);
}
