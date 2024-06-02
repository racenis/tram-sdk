// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

out vec4 fragment;

in vec2 vert_uv;
in vec3 vert_color;
in vec3 vert_color_add;
flat in uint vert_tex_index;

uniform sampler2D sampler[16];

void main() {
	fragment = texture(sampler[vert_tex_index], vert_uv);
	
	if (fragment.a < 0.5) discard;
	
    fragment *= vec4(vert_color, 1.0);
	fragment += vec4(vert_color_add, 1.0);
}