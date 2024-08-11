#version 300 es

// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;

layout (location = 0) in vec3 Position;		// vertex position
layout (location = 1) in vec3 Normal;		// vertex normal
layout (location = 2) in vec2 VertUV;		// vertex texture coordinate
layout (location = 3) in uvec4 BoneIndex;	// bone index in pose matrix list
layout (location = 4) in vec4 BoneWeight;	// bone weight
layout (location = 5) in uint TexIndex;		// texture index

// what the fuck
struct Light {
	vec4 aa; // light position
	vec4 bb; // light color
	vec4 cc;
	vec4 dd;
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

layout (std140) uniform Lights {
	Light scene_lights[50];
};

layout (std140) uniform Bones {
	mat4 bone[30];
};

out vec3 vert_color;
out vec2 vert_uv;
flat out uint vert_tex_index;

void main() {
	// transform vertex with each pose matrix
	vec4 posed1 = BoneWeight.x * bone[BoneIndex.x] * vec4(Position, 1.0);
	vec4 posed2 = BoneWeight.y * bone[BoneIndex.y] * vec4(Position, 1.0);
	vec4 posed3 = BoneWeight.z * bone[BoneIndex.z] * vec4(Position, 1.0);
	vec4 posed4 = BoneWeight.w * bone[BoneIndex.w] * vec4(Position, 1.0);
	
	// get final vertex posed position in model space
	vec4 posed = posed1 + posed2 + posed3 + posed4;
	
	// transform to world space
    gl_Position = projection * view * model * posed;
	
	
	// do the same, but with the normal vector
	vec4 normal1 = BoneWeight.x * bone[BoneIndex.x] * vec4(Normal, 1.0);
	vec4 normal2 = BoneWeight.y * bone[BoneIndex.y] * vec4(Normal, 1.0);
	vec4 normal3 = BoneWeight.z * bone[BoneIndex.z] * vec4(Normal, 1.0);
	vec4 normal4 = BoneWeight.w * bone[BoneIndex.w] * vec4(Normal, 1.0);
	
	// get posed normal in model space
	vec4 normal = normal1 + normal2 + normal3 + normal4;
	
	// get both vertex position and vertex normal into world space
	vec3 v = vec3(model * posed);
	vec3 n = normalize(vec3(model * vec4(normal.xyz, 0.0)));
	
	// light math
	float distance1 = length(vec3(scene_lights[model_lights.x].aa) - v);
	float distance2 = length(vec3(scene_lights[model_lights.y].aa) - v);
	float distance3 = length(vec3(scene_lights[model_lights.z].aa) - v);
	float distance4 = length(vec3(scene_lights[model_lights.w].aa) - v);
	
	// ambrient color light math
	vert_color = vec3(ambient_color);
	vert_color += sun_color * max(dot(n, normalize(sun_direction)), 0.0);
	vert_color *= sun_weight;
	
	// more light math
	vert_color += vec3(scene_lights[model_lights.x].bb) * max(dot(n, normalize(vec3(scene_lights[model_lights.x].aa) - v)), 0.0) * (1.0 / (1.0 + 0.09 * distance1 + 0.032 * (distance1 * distance1)));
	vert_color += vec3(scene_lights[model_lights.y].bb) * max(dot(n, normalize(vec3(scene_lights[model_lights.y].aa) - v)), 0.0)* (1.0 / (1.0 + 0.09 * distance2 + 0.032 * (distance2 * distance2)));
	vert_color += vec3(scene_lights[model_lights.z].bb) * max(dot(n, normalize(vec3(scene_lights[model_lights.z].aa) - v)), 0.0)* (1.0 / (1.0 + 0.09 * distance3 + 0.032 * (distance3 * distance3)));
	vert_color += vec3(scene_lights[model_lights.w].bb) * max(dot(n, normalize(vec3(scene_lights[model_lights.w].aa) - v)), 0.0)* (1.0 / (1.0 + 0.09 * distance4 + 0.032 * (distance4 * distance4)));
	
	// do specular stuff
	vec3 view_dir = normalize(view_pos - v);
	float specular1 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.x].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular2 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.y].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular3 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.z].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular4 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.w].aa) - v), n)), 0.0), specular[TexIndex].y);
	vert_color += specular[TexIndex].x * specular1 * vec3(scene_lights[model_lights.x].bb);
	vert_color += specular[TexIndex].x * specular2 * vec3(scene_lights[model_lights.y].bb);
	vert_color += specular[TexIndex].x * specular3 * vec3(scene_lights[model_lights.z].bb);
	vert_color += specular[TexIndex].x * specular4 * vec3(scene_lights[model_lights.w].bb);
	
	// add material color
	vert_color *= vec3(colors[TexIndex]);
	
    vert_uv = VertUV;
	vert_tex_index = TexIndex;
}