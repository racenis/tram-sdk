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

uniform sampler2D sampler[15];
uniform sampler2DArray samplerArray;

void main() {
    fragment = texture(sampler[vert_tex_index], vert_uv) * vec4(vert_color, 1.0);
    //fragment = vec4(0.0,  0.0, 0.0, 1.0);
	fragment += vec4(vert_color_add, 1.0);
	
	vec3 reflection_coords = vert_reflection;
	if (reflection_coords.z > 0.0) {
		reflection_coords.z = 1.0;
	} else {
		reflection_coords.z = 0.0;
	}
	
	fragment += vert_reflectivity * texture(samplerArray, reflection_coords);
}