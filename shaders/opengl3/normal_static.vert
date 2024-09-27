// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

layout (location = 0) in vec3 Position;		// vertex position
layout (location = 1) in vec3 Normal;		// vertex normal
layout (location = 2) in vec2 VertUV;		// texture coordinate
layout (location = 3) in vec2 VertLightUV;	// lightmap coordinate
layout (location = 4) in uint TexIndex;		// texture index

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
out vec2 vert_light_uv;
out vec3 vert_color;
flat out uint vert_tex_index;

void main() {
    gl_Position = projection * view * model * vec4(Position, 1.0);
	
	vec3 n = normalize(vec3(model * vec4(Normal, 0.0)));
	
	vert_color = vec3(ambient_color);
	vert_color += sun_color * max(dot(n, normalize(sun_direction)), 0.0);
	vert_color *= sun_weight;
	vert_color += 1.0 - sun_weight;
	
    vert_uv = VertUV + vec2(texture_transforms[TexIndex]);
	vert_tex_index = TexIndex;
	vert_light_uv = VertLightUV;
}