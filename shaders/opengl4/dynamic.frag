// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

out vec4 fragment;

in vec2 vert_uv;
in vec3 vert_color;
in vec3 vert_color_add;
in vec3 vert_reflection;
in float vert_opacity;
in float vert_reflectivity;
flat in uint vert_tex_index;

uniform sampler2D sampler[15];
uniform sampler2DArray samplerArray;

void main() {
    fragment = texture(sampler[vert_tex_index], vert_uv) * vec4(vert_color, vert_opacity);
	fragment += vec4(vert_color_add, 0.0);
	
#ifdef FLAG_ALPHA_TEST
	if (fragment.a < 0.5) discard;
#endif
	
	vec3 reflection_coords = vert_reflection;
	if (reflection_coords.z > 0.0) {
		reflection_coords.z = 1.0;
	} else {
		reflection_coords.z = 0.0;
	}
	
	vec3 reflection_color = vert_reflectivity * vec3(texture(samplerArray, reflection_coords));
	float reflection_brightness = dot(vec3(0.299, 0.587, 0.114), vec3(reflection_color));
	
	fragment += vec4(reflection_color, reflection_brightness);
}