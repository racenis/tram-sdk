#version 300 es

// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;

layout (location = 0) in vec3 Position;		// vertex position
layout (location = 1) in vec3 Normal;		// vertex normal
layout (location = 2) in vec2 VertUV;		// texture coordinate
layout (location = 3) in vec2 VertLightUV;	// lightmap coordinate
layout (location = 4) in uint TexIndex;		// texture index

struct Light {
	vec4 aa;
	vec4 bb;
	vec4 cc;
	vec4 dd;
};

layout (std140) uniform Lights {
	Light scene_lights[50];
};

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
	vec3 view_pos;
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
};

out vec2 vert_uv;
out vec2 vert_light_uv;
out vec3 vert_color;
flat out uint vert_tex_index;

void main() {
	vec4 world_position = model * vec4(Position, 1.0);

	world_position.y += 0.1 * (sin(time + world_position.x)+1.0);
	
    gl_Position = projection * view * world_position;
	
	
	
	//vec3 n = normalize(vec3(model * vec4(Normal, 0.0)));
	vec3 n = vec3(model * vec4(Normal, 0.0));
	vec3 v = vec3(world_position);
	
	n.x += 0.333 * sin(time + world_position.x);
	n = normalize(n);
	
	vert_color = vec3(ambient_color);
	vert_color += sun_color * max(dot(n, normalize(sun_direction)), 0.0);
	vert_color *= sun_weight;
	vert_color += 1.0 - sun_weight;
	
	
	vec3 view_dir = normalize(view_pos - v);
	float specular1 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.x].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular2 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.y].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular3 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.z].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular4 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.w].aa) - v), n)), 0.0), specular[TexIndex].y);
	vert_color += specular[TexIndex].x * specular1 * vec3(scene_lights[model_lights.x].bb);
	vert_color += specular[TexIndex].x * specular2 * vec3(scene_lights[model_lights.y].bb);
	vert_color += specular[TexIndex].x * specular3 * vec3(scene_lights[model_lights.z].bb);
	vert_color += specular[TexIndex].x * specular4 * vec3(scene_lights[model_lights.w].bb);
	
    vert_uv = VertUV + 0.1 * vec2(sin(time + 0.351 * world_position.x), cos(time + 0.351 * world_position.z));
	vert_tex_index = TexIndex;
	vert_light_uv = VertLightUV;
}