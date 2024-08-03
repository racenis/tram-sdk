// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

layout (location = 0) in vec3 Position;		// vertex positon
layout (location = 1) in vec3 VertColor;	// vertex color

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
	vec3 view_pos;
};

out vec3 vert_color;

void main() {
    gl_Position = projection * view * vec4(Position, 1.0);
	
	vert_color = VertColor;
}