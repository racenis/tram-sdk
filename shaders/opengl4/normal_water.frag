// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

out vec4 fragment;

in vec2 vert_uv;
in vec2 vert_light_uv;
in vec3 vert_color;
flat in uint vert_tex_index;

layout (std140) uniform ModelMatrices {
    mat4 model;
	uvec4 model_lights;
	vec3 sun_direction;
	vec3 sun_color;
	vec4 ambient_color;
	float time;
	float sun_weight;
	float screen_width;
	float screen_height;
	vec4 colors[15];
	vec4 specular[15];
	vec4 texture_transforms[15];
};

uniform sampler2D sampler[16];

void main() {
    float offse = mod(time / 5.0, 1.0);
	vec2 uv = vec2(vert_uv.x + offse, vert_uv.y + offse);
	//vec2 uv2 = vec2(vert_uv.x, vert_uv.y + mod(time.x / 60.0, 1.0));
    fragment = texture(sampler[vert_tex_index], uv)/* * texture(sampler[vert_tex_index], uv2) */* texture(sampler[15], vert_light_uv) * vec4(vert_color, 1.0);
	// TODO: break the shader again and make water pretty
}
