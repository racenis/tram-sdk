// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core
out vec4 fragment;

in vec2 vert_uv;
in vec3 vert_color;
in vec3 vert_color_add;
in vec3 vert_reflection;
in float vert_reflectivity;
flat in uint vert_tex_index;

uniform sampler2D sampler[16];

void main() {
    fragment = texture(sampler[vert_tex_index], vert_uv) * vec4(vert_color, 1.0);
    //fragment = vec4(0.0,  0.0, 0.0, 1.0);
	fragment += vec4(vert_color_add, 1.0);
	
	vec2 reflection_coords = vec2(vert_reflection);
	if (vert_reflection.z > 0.0) reflection_coords.x += 0.5;
	
	fragment += vert_reflectivity * texture(sampler[15], reflection_coords);
}