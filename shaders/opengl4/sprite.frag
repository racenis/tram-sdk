// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

out vec4 fragment;
  
in vec2 vert_uv;
flat in uint vert_tex_index;

uniform sampler2D sampler[15];
uniform sampler2DArray samplerArray;

void main() {
    fragment = texture(sampler[vert_tex_index], vert_uv);
	if (fragment.w < 0.9) discard;
}