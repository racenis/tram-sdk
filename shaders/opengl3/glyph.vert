// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

layout (location = 0) in vec2 Position;		// vertex position
layout (location = 1) in vec2 TexCoord;		// texture coordinate
layout (location = 2) in vec3 VertColor;	// vertex color
layout (location = 5) in uint Texture;		// texture index

out vec2 vert_uv;
out vec3 vert_color;
flat out uint vert_tex_index;

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
};

void main() {
	// the Position variable actually contains the position in normal GUI
	// coordinates, where each unit is one pixel and the origin is in the
	// upper left corner.

	// we first need to convert these coordinates to OpenGL coordinates
	float pos_x = (Position.x / (screen_width / 2.0)) - 1.0;
	float pos_y = (Position.y / (screen_height / -2.0)) + 1.0;

	// then we can output these coordinates
    gl_Position = vec4(pos_x, pos_y, -0.5, 1.0);
	
	// what the fuck
	vert_uv = vec2(TexCoord.x / 256.0, TexCoord.y / -256.0);
	
	vert_color = VertColor;
	vert_tex_index = Texture;
}