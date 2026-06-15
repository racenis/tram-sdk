// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

out vec4 fragment;
  
in vec2 vert_uv;
in vec2 vert_light_uv;
in vec3 vert_color;
in float vert_opacity;
flat in uint vert_tex_index;

uniform sampler2D sampler[15];
uniform sampler2DArray samplerArray;

void main() {
    fragment = texture(sampler[vert_tex_index], vert_uv) * texture(samplerArray, vec3(vert_light_uv, 0.0)) * vec4(vert_color, vert_opacity);
}