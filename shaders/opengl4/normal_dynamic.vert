// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

layout (location = 0) in vec3 Position;		// vertex position
layout (location = 1) in vec3 Normal;		// vertex normal
layout (location = 2) in vec2 VertUV;		// vertex texture coordinate
layout (location = 3) in ivec4 BoneIndex;	// bone index in pose matrix list
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
	vec4 texture_transforms[15];
	vec3 l00;
    vec3 l1m1; vec3 l10; vec3 l11;
    vec3 l2m2; vec3 l2m1; vec3 l20; vec3 l21; vec3 l22;
};

layout (std140) uniform Lights {
	Light scene_lights[50];
};

layout (std140) uniform Bones {
	mat4 bone[30];
};

out vec3 vert_color;
out vec3 vert_color_add;
out vec2 vert_uv;
out vec3 vert_reflection;
out float vert_reflectivity;
flat out uint vert_tex_index;

// spherical harmonic consts
// http://graphics.stanford.edu/papers/envmap/envmap.pdf
const float c1 = 0.429043;
const float c2 = 0.511664;
const float c3 = 0.743125;
const float c4 = 0.886227;
const float c5 = 0.247708;

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
	
	// calculate distances to light sources
	float distance1 = length(vec3(scene_lights[model_lights.x].aa) - v);
	float distance2 = length(vec3(scene_lights[model_lights.y].aa) - v);
	float distance3 = length(vec3(scene_lights[model_lights.z].aa) - v);
	float distance4 = length(vec3(scene_lights[model_lights.w].aa) - v);
	
	// calculate vertex -> light vectors
	vec3 v_to_light1 = normalize(vec3(scene_lights[model_lights.x].aa) - v);
	vec3 v_to_light2 = normalize(vec3(scene_lights[model_lights.y].aa) - v);
	vec3 v_to_light3 = normalize(vec3(scene_lights[model_lights.z].aa) - v);
	vec3 v_to_light4 = normalize(vec3(scene_lights[model_lights.w].aa) - v);
	
	// calculate light attenuation by distance
	float attenuation1 = max(dot(n, v_to_light1), 0.0) * (1.0 / (1.0 + 0.09 * distance1 + 0.032 * (distance1 * distance1)));
	float attenuation2 = max(dot(n, v_to_light2), 0.0) * (1.0 / (1.0 + 0.09 * distance2 + 0.032 * (distance2 * distance2)));
	float attenuation3 = max(dot(n, v_to_light3), 0.0) * (1.0 / (1.0 + 0.09 * distance3 + 0.032 * (distance3 * distance3)));
	float attenuation4 = max(dot(n, v_to_light4), 0.0) * (1.0 / (1.0 + 0.09 * distance4 + 0.032 * (distance4 * distance4)));
	
	float directionality1 = clamp(pow(max(dot(vec3(scene_lights[model_lights.x].cc), -v_to_light1), 0.0), scene_lights[model_lights.x].cc.w), 0.0, 1.0);
	float directionality2 = clamp(pow(max(dot(vec3(scene_lights[model_lights.y].cc), -v_to_light2), 0.0), scene_lights[model_lights.y].cc.w), 0.0, 1.0);
	float directionality3 = clamp(pow(max(dot(vec3(scene_lights[model_lights.z].cc), -v_to_light3), 0.0), scene_lights[model_lights.z].cc.w), 0.0, 1.0);
	float directionality4 = clamp(pow(max(dot(vec3(scene_lights[model_lights.w].cc), -v_to_light4), 0.0), scene_lights[model_lights.w].cc.w), 0.0, 1.0);

	// take in ambient color and add sun color
	vert_color = vec3(ambient_color);
	vert_color += sun_color * max(dot(n, normalize(sun_direction)), 0.0);
	vert_color *= sun_weight;
	
	// add in light probe lights
	vert_color += c1 * l22 * (n.x * n.x - n.y * n.y) + c3 * l20 * n.z * n.z + c4 * l00 - c5 * l20
					+ 2.0 * c1 * (l2m2 * n.x * n.y + l21 * n.x * n.z + l2m1 * n.y * n.z)
					+ 2.0 * c2 * (l11 * n.x + l1m1 * n.y + l10 * n.z);
	
	// add in light colors
	vert_color += vec3(scene_lights[model_lights.x].bb) * attenuation1 * directionality1;
	vert_color += vec3(scene_lights[model_lights.y].bb) * attenuation2 * directionality2;
	vert_color += vec3(scene_lights[model_lights.z].bb) * attenuation3 * directionality3;
	vert_color += vec3(scene_lights[model_lights.w].bb) * attenuation4 * directionality4;
	
	// calculate specular intensities
	vec3 view_dir = normalize(view_pos - v);
	float specular1 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.x].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular2 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.y].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular3 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.z].aa) - v), n)), 0.0), specular[TexIndex].y);
	float specular4 = pow(max(dot(view_dir, reflect(-normalize(vec3(scene_lights[model_lights.w].aa) - v), n)), 0.0), specular[TexIndex].y);
	
	// calculate specular color
	vec3 specular_color = vec3(0.0, 0.0, 0.0);
	specular_color += specular[TexIndex].x * specular1 * vec3(scene_lights[model_lights.x].bb) * directionality1;
	specular_color += specular[TexIndex].x * specular2 * vec3(scene_lights[model_lights.y].bb) * directionality2;
	specular_color += specular[TexIndex].x * specular3 * vec3(scene_lights[model_lights.z].bb) * directionality3;
	specular_color += specular[TexIndex].x * specular4 * vec3(scene_lights[model_lights.w].bb) * directionality4;
	
	// add material color
	vert_color *= vec3(colors[TexIndex]);
	
	// add specular color
	vert_color_add = mix(vec3(0.0, 0.0, 0.0), specular_color, specular[TexIndex].z);
	vert_color += mix(specular_color, vec3(0.0, 0.0, 0.0), specular[TexIndex].z);
	
	
	vec3 reflection = reflect(-view_dir, n);
	vert_reflection = vec3(reflection.x /4.0 + 0.25, reflection.y /2.0 + 0.5, reflection.z);
	vert_reflectivity = specular[TexIndex].w;
    vert_uv = VertUV + vec2(texture_transforms[TexIndex]);
	vert_tex_index = TexIndex;
}