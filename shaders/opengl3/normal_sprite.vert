// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

layout (location = 0) in vec3 Position;		// vertex position
layout (location = 3) in vec2 VOffset;		// screen space transform
layout (location = 1) in vec2 VertUV;		// texture coordinates
layout (location = 4) in float Verticality; // how vertical should a sprite be
layout (location = 5) in uint TexIndex;		// texture index

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

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

out vec2 vert_uv;
flat out uint vert_tex_index;

void main() {
	vec4 screen_pos = projection * view * model * vec4(Position, 1.0);
	
    gl_Position = screen_pos + vec4(VOffset, 0.0, 0.0);

    vert_uv = VertUV;
	vert_tex_index = TexIndex;
}