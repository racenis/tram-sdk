// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

out vec4 fragment;
  
in vec2 vert_uv;
in vec3 vert_color;
flat in uint vert_tex_index;

uniform sampler2D sampler[15];
uniform sampler2DArray samplerArray;

void main() {
    fragment = texture(sampler[vert_tex_index], vert_uv);
	if (fragment.w < 0.9) discard;
	fragment *= vec4(vert_color, 1.0);
}