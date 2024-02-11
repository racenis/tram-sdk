// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// Tramway SDK -- Radiosity tool

// TODO:
// - add option to use multiple worldcells in the same bake
// - allow loading multiple models in the same bake
// - make program go vroom vroom
// - add indirect lighting
// - add emissive materials

#include <iostream>
#include <vector>

#include <framework/file.h>
#include <framework/math.h>
#include <templates/aabb.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace tram;

struct Vertex {
	vec3 pos;
	vec3 nrm;
	vec2 tex;
	vec2 map;
	
	bool operator==(const Vertex& other) {
		return pos == other.pos && nrm == other.nrm && tex == other.tex && map == other.map;
	}
};

struct Triangle {
	Vertex v1, v2, v3;
	name_t mat;
};

struct Texel {
	vec3 color;
};

struct Lightmap {
	Lightmap(int width, int height) {
		t = new Texel[width*height];
		for (int i = 0; i < width*height; i++) {
			t[i] = Texel {.color = {0.0f, 0.0f, 0.0f}};
		}
		w = width;
		h = height;
	}
	
	void Blit(int x, int y, Texel tex) {
		if (x < 0 || y < 0 || x >= w || y >= h) return;
		t[y*w+x] = tex;
	}
	
	void BlitMix(int x, int y, Texel tex) {
		if (x < 0 || y < 0 || x >= w || y >= h) return;
		t[y*w+x].color += tex.color;
	}
	
	Texel Sample(int x, int y) {
		return t[y*w+x];
	}
	
	Texel* t;
	int w, h;
};

struct Light {
	vec3 pos;
	vec3 color;
	float radius;
};

struct Entity {
	vec3 pos;
	quat rot;
	name_t id;
	name_t name;
	name_t model;
	name_t lightmap;
};

vec3 GetBarycentric(Triangle tri, float x, float y) {
	const vec2 v1 = tri.v2.map - tri.v1.map;
	const vec2 v2 = tri.v3.map - tri.v1.map;
	const vec2 v3 = vec2(x, y) - tri.v1.map;
	const float dominator = v1.x * v2.y - v2.x * v1.y;
	const float d2 = (v3.x * v2.y - v2.x * v3.y) / dominator;
	const float d3 = (v1.x * v3.y - v3.x * v1.y) / dominator;
	const float d1 = 1.0f - d2 - d3;
	return {d1, d2, d3};
}

static vec3 TriangleAABBMin (Triangle t) {
    return {
        t.v1.pos.x < t.v2.pos.x ? (t.v1.pos.x < t.v3.pos.x ? t.v1.pos.x : t.v3.pos.x) : (t.v2.pos.x < t.v3.pos.x ? t.v2.pos.x : t.v3.pos.x),
        t.v1.pos.y < t.v2.pos.y ? (t.v1.pos.y < t.v3.pos.y ? t.v1.pos.y : t.v3.pos.y) : (t.v2.pos.y < t.v3.pos.y ? t.v2.pos.y : t.v3.pos.y),
        t.v1.pos.z < t.v2.pos.z ? (t.v1.pos.z < t.v3.pos.z ? t.v1.pos.z : t.v3.pos.z) : (t.v2.pos.z < t.v3.pos.z ? t.v2.pos.z : t.v3.pos.z)
    };
}

static vec3 TriangleAABBMax (Triangle t) {
    return {
        t.v1.pos.x > t.v2.pos.x ? (t.v1.pos.x > t.v3.pos.x ? t.v1.pos.x : t.v3.pos.x) : (t.v2.pos.x > t.v3.pos.x ? t.v2.pos.x : t.v3.pos.x),
        t.v1.pos.y > t.v2.pos.y ? (t.v1.pos.y > t.v3.pos.y ? t.v1.pos.y : t.v3.pos.y) : (t.v2.pos.y > t.v3.pos.y ? t.v2.pos.y : t.v3.pos.y),
        t.v1.pos.z > t.v2.pos.z ? (t.v1.pos.z > t.v3.pos.z ? t.v1.pos.z : t.v3.pos.z) : (t.v2.pos.z > t.v3.pos.z ? t.v2.pos.z : t.v3.pos.z)
    };
}

vec3 FindNearestIntersection(AABBTree& tree, std::vector<Triangle>& tris, vec3 pos, vec3 dir) {
	std::vector<uint32_t> results;
	tree.FindIntersection(pos, dir, tree.root, results);

	vec3 closest = {INFINITY, INFINITY, INFINITY};
	
	for (auto res : results) {
		vec3 intr = RayTriangleIntersection(pos, dir, tris[res].v1.pos, tris[res].v2.pos, tris[res].v3.pos);
		if (intr.x == INFINITY) continue;			
		if(glm::dot(tris[res].v1.nrm, dir) > -0.01f /*&& glm::distance(pos, intr) < 0.1f*/) continue;
		if (glm::distance(pos, intr) < glm::distance(pos, closest)) closest = intr;
	}
	
	return closest;
};

vec3 FindTexelColorFromLights(AABBTree& tree, std::vector<Triangle>& tris, std::vector<Light>& lights, vec3 pos, vec3 normal) {
	vec3 color = {0.0f, 0.0f, 0.0f};
	
	// we might get a collision with the triangle, on which the texel is located
	// on, so we move it off of the surface a little bit
	pos += 0.01f * normal;
	
	for (const auto& light : lights) {
		if (light.radius < glm::distance(pos, light.pos)) continue;
		
		vec3 light_dir = glm::normalize(light.pos - pos);
		
		vec3 nearest = FindNearestIntersection(tree, tris, pos, light_dir);

		if (glm::distance(nearest, pos) > glm::distance(light.pos, pos)) {
			float distance1 = glm::length(light.pos - pos);
			color += light.color * glm::max(glm::dot(normal, glm::normalize(light.pos - pos)), 0.0f) * (1.0f / (1.0f + 0.09f * distance1 + 0.032f * (distance1 * distance1)));
		} 
	}
	
	return color;
};

struct RasterParams {
	int l_w;
	int l_h;
	int stretch_low_x;
	int stretch_low_y;
	int stretch_high_x;
	int stretch_high_y;
};

void RasterizeTriangle(RasterParams p, Triangle tri, auto raster_f) {
	// find the lowest, highest and middle vertices
	Vertex lowest = tri.v1;
	if (tri.v1.map.y < lowest.map.y) lowest = tri.v1;
	if (tri.v2.map.y < lowest.map.y) lowest = tri.v2;
	if (tri.v3.map.y < lowest.map.y) lowest = tri.v3;
	
	Vertex highest = tri.v1;
	if (tri.v1.map.y > highest.map.y) highest = tri.v1;
	if (tri.v2.map.y > highest.map.y) highest = tri.v2;
	if (tri.v3.map.y > highest.map.y) highest = tri.v3;
	
	// this should not happen!
	if (lowest == highest) std::cout << "what the fuck" << std::endl;
	
	Vertex middle;
	if ((tri.v2 == lowest || tri.v3 == lowest) && (tri.v2 == highest || tri.v3 == highest)) middle = tri.v1;
	if ((tri.v1 == lowest || tri.v3 == lowest) && (tri.v1 == highest || tri.v3 == highest)) middle = tri.v2;
	if ((tri.v1 == lowest || tri.v2 == lowest) && (tri.v1 == highest || tri.v2 == highest)) middle = tri.v3;
	
	// position of vertices on the raster image
	int lowest_y = lowest.map.y * (float)p.l_h;
	int middle_y = middle.map.y * (float)p.l_h;
	int highest_y = highest.map.y * (float)p.l_h;
	
	// stretch the triangle a little bit
	if (middle_y == highest_y) {
		middle_y += p.stretch_high_y;
		highest_y += p.stretch_high_y;
		lowest_y -= p.stretch_low_y;
	} else {
		highest_y += p.stretch_high_y;
		lowest_y -= p.stretch_low_y;
	}

	int low_high_lines = highest_y - lowest_y;
	int low_mid_lines = middle_y - lowest_y;
	int mid_high_lines = highest_y - middle_y;
	
	int highest_x = highest.map.x * (float)p.l_w;
	int middle_x = middle.map.x * (float)p.l_w;
	int lowest_x = lowest.map.x * (float)p.l_w;
	
	if (highest_x > middle_x || highest_x > lowest_x) highest_x += p.stretch_high_x;
	if (middle_x > highest_x || middle_x > lowest_x) middle_x += p.stretch_high_x;
	if (lowest_x > middle_x || lowest_x > highest_x) lowest_x += p.stretch_high_x;

	if (highest_x < middle_x || highest_x < lowest_x) highest_x -= p.stretch_low_x;
	if (middle_x < highest_x || middle_x < lowest_x) middle_x -= p.stretch_low_x;
	if (lowest_x < middle_x || lowest_x < highest_x) lowest_x -= p.stretch_low_x;

	float low_high_dir = (float)(highest_x - lowest_x) / (float)low_high_lines;
	float low_mid_dir = (float)(middle_x - lowest_x) / (float)low_mid_lines;
	float mid_high_dir = (float)(highest_x - middle_x) / (float)mid_high_lines;
	
	float left_pos = lowest_x;
	float right_pos = lowest_x;
			
	for (int row = lowest_y; row < middle_y; row++) {
		left_pos += low_high_dir;
		right_pos += low_mid_dir;

		int from = left_pos;
		int to = right_pos;
		
		if (from > to) std::swap(from, to);
		
		for (int col = from; col < to; col++) {
			vec3 d = GetBarycentric(tri, (float)col / (float)p.l_w, (float)row / (float)p.l_h);
			
			vec3 pos = d.x * tri.v1.pos + d.y * tri.v2.pos + d.z * tri.v3.pos;
			vec3 nrm = d.x * tri.v1.nrm + d.y * tri.v2.nrm + d.z * tri.v3.nrm;
			
			raster_f(col, row, pos, nrm);
		}
	}
	
	for (int row = middle_y; row < highest_y; row++) {
		left_pos += low_high_dir;
		right_pos += mid_high_dir;
		
		int from = left_pos;
		int to = right_pos;
		
		if (from > to) std::swap(from, to);

		for (int col = from; col < to; col++) {
			vec3 d = GetBarycentric(tri, (float)col / (float)p.l_w, (float)row / (float)p.l_h);
			
			vec3 pos = d.x * tri.v1.pos + d.y * tri.v2.pos + d.z * tri.v3.pos;
			vec3 nrm = d.x * tri.v1.nrm + d.y * tri.v2.nrm + d.z * tri.v3.nrm;
			
			raster_f(col, row, pos, nrm);
		}
	}
}

int main(int argc, const char** argv) {
	stbi_flip_vertically_on_write(true);
	SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);
	
	std::cout << "Tramway SDK -- Radiosity lightmapper" << std::endl;
	
	if (argc < 5) {
		std::cout << "Usage: trad worldcell entity width height";
		std::cout << "\n\tworldcell is the name of the worldcell, that contains the entity and";
		std::cout << "\n\t\tthe light with which it will be illuminated with";
		std::cout << "\n\tentity is the name or the id of an entity, for which the lightmap will";
		std::cout << "\n\t\tbe baked";
		std::cout << "\n\twidth is the width of the lightmap, in texels";
		std::cout << "\n\theight is the height of the lightmap, in texels" << std::endl;
		return 0;
	}
	
	const char* entity = argv[2];
	const char* worldcell = argv[1];
	
	int lightmap_width = atoi(argv[3]);
	int lightmap_height = atoi(argv[4]); 
	
	bool paint_coords = false;
	bool paint_verts = false;
	
	bool force_fullbright = false;
	
	// stretch the raster a little bit, to help with color bleeding
	int stretch_low_x = 1;
	int stretch_low_y = 1;
	int stretch_high_x = 2;
	int stretch_high_y = 2;
	
	if (lightmap_width < 1 || lightmap_height < 1) {
		std::cout << "Lightmap size has to be at least something!!! NOT NEGATIVE!!!" << std::endl;
		return 0;
	}
	
	if ((lightmap_width & (lightmap_width - 1)) != 0 || (lightmap_height & (lightmap_height - 1)) != 0) {
		std::cout << "Lightmap size has to be a power of two." << std::endl;
		return 0;
	}
	
	for (int i = 5; i < argc; i++) {
		if (strcmp(argv[i], "-coords") == 0) {
			paint_coords = true;
		}
		
		if (strcmp(argv[i], "-verts") == 0) {
			paint_verts = true;
		}
		
		if (strcmp(argv[i], "-pad") == 0) {
			int ammount = atoi(argv[++i]);
			stretch_low_x = ammount;
			stretch_low_y = ammount;
			stretch_high_x = ammount + 1;
			stretch_high_y = ammount + 1;
		}
		
		if (strcmp(argv[i], "-fullbright") == 0) {
			force_fullbright = true;
		}
		
	}
	
	
	std::vector<Entity> entities;
	std::vector<Light> lights;
	
	
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                           WORLDCELL LOADER                            +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	std::cout << "Loading worldcell " << worldcell << "... " << std::flush;
	
	std::string worldcell_path = "data/worldcells/";
	worldcell_path += worldcell;
	worldcell_path += ".cell";
	
	File cell(worldcell_path.c_str(), MODE_READ | MODE_PAUSE_LINE);
	
	if (!cell.is_open()) {
		std::cout << "\nError opening file: " << worldcell_path << "!" <<std::endl;
		return 0;
	}
	
	name_t file_version = cell.read_name(); cell.skip_linebreak();
	
	if (file_version != "CELLv2") {
		std::cout << "\nUnrecognized worldcell file version: " << file_version << "!" << std::endl;
		return 0;
	}
	
	while (cell.is_continue()) {
		name_t entity_type = cell.read_name();
		
		// right now I will hard-code the entity types directly into the program
		// later it might be a good idea to load them from the entity definition
		// file, when the format stabilizes
		if (entity_type == "staticwobj") {
			Entity entity;
			
			entity.id = cell.read_name();
			entity.name = cell.read_name();
			entity.pos = {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			entity.rot = vec3 {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			entity.model = cell.read_name();
			entity.lightmap = cell.read_name();
			
			entities.push_back(entity);
		}
		
		if (entity_type == "light") {
			Light light;
			
			cell.read_name();
			cell.read_name();
			light.pos = {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			cell.read_float32(); cell.read_float32(); cell.read_float32();
			light.color = {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			light.radius = cell.read_float32();
			
			lights.push_back(light);	
		}
		
		cell.skip_linebreak();
	}

	std::cout << "done!" << std::endl;
	
	int entity_index = -1;
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i].name == entity || entities[i].id == entity) entity_index = i;
	}
	
	if (entity_index == -1) {
		std::cout << "Could not find entity " << entity << " in worldcell!"  << std::endl;
		return 0;
	}
	
	name_t model_name = entities[entity_index].model;
	name_t lightmap_name = entities[entity_index].lightmap;
	vec3 inv_pos = -entities[entity_index].pos;
	quat inv_rot = glm::inverse(entities[entity_index].rot);
	
	for (auto& light : lights) {
		light.pos = inv_pos + light.pos;
		light.pos = inv_rot * light.pos;
	}
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                             MODEL LOADER                              +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
		
	std::string model_path = "data/models/";
	model_path += (const char*)model_name;
	model_path += ".stmdl";
	
	File file(model_path.c_str(), MODE_READ);
	
	if (!file.is_open()) {
		std::cout << "Error opening model file " << model_path << std::endl;
		return 0;
	}
	
	int vrt_c = file.read_int32();
	int tri_c = file.read_int32();
	int mat_c = file.read_int32();
		
	std::vector<name_t> materials;
	for (int i = 0; i < mat_c; i++) {
		materials.push_back(file.read_name());
	}
	
	std::vector<Vertex> vertices;
	for (int i = 0; i < vrt_c; i++) {
		Vertex v;
		
		v.pos = {file.read_float32(), file.read_float32(), file.read_float32()};
		v.nrm = {file.read_float32(), file.read_float32(), file.read_float32()};
		v.tex = {file.read_float32(), file.read_float32()};
		v.map = {file.read_float32(), file.read_float32()};
		
		vertices.push_back(v);
	}
	
	std::vector<Triangle> triangles;
	for (int i = 0; i < tri_c; i++) {
		Triangle t;
		
		t.v1 = vertices[file.read_int32()];
		t.v2 = vertices[file.read_int32()];
		t.v3 = vertices[file.read_int32()];
		t.mat = materials[file.read_int32()];
		
		triangles.push_back(t);
	}
	
	std::cout << "Loaded model " << model_name << ", it has " << vrt_c << " verts, " << tri_c << " tris, " << mat_c << " materials." << std::endl;
	
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                        RADIOSITY CALCULATIONS                         +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	// this tree here will contain all of the triangles in the scene, at the
	// time of the radiosity computation.
	// we will use it to determine if there is a clear path between two points
	AABBTree all_tree;
	std::vector<Triangle> all_tris;
	
	for (const auto& tri : triangles) {
		all_tree.InsertLeaf(all_tris.size(), TriangleAABBMin(tri), TriangleAABBMax(tri));
		all_tris.push_back(tri);
	}
	
	
	
	std::cout << "Baking a lightmap with " << lights.size() <<" lights, " << lightmap_width << " by " << lightmap_height << " texels in size." << std::endl;
	
	if (lights.size() == 0) {
		std::cout << "Scene contains no lights!" << std::endl;
		force_fullbright = true;
	}
	
	Lightmap l(lightmap_width, lightmap_height);
		
	RasterParams image_params = {
		l.w,
		l.h,
		stretch_low_x,
		stretch_low_y,
		stretch_high_x,
		stretch_high_y
	};
	
	std::cout << "Computing " << std::flush;
	
	// iterate through each triangle in the lightmap and rasterize it
	int tri_index = 0;
	for (const auto& tri : triangles) {
		if (tri_index++ % (triangles.size() / 60) == 0) {
			std::cout << "." << std::flush;
		}
		
		if (paint_coords) {
			
			// rasterize triangle and set color to position
			RasterizeTriangle(image_params, tri, [&](int col, int row, vec3 pos, vec3 nrm){
				l.Blit(col, row, {pos});
			});
			
		} else if (force_fullbright) {
			
			// rasterize triangle to fullbright
			RasterizeTriangle(image_params, tri, [&](int col, int row, vec3 pos, vec3 nrm){
				l.Blit(col, row, {{1.0f, 1.0f, 1.0f}});
			});
			
		} else {
			
			// rasterize triangle and set color to light value
			RasterizeTriangle(image_params, tri, [&](int col, int row, vec3 pos, vec3 nrm){
				vec3 color = FindTexelColorFromLights(all_tree, all_tris, lights, pos, nrm);
				l.Blit(col, row, {color});
			});
			
		}
		
		// paint in green dots in the triangle vertex positions
		if (paint_verts) {
			l.Blit(tri.v1.map.x * (float)l.w, tri.v1.map.y * (float)l.h, {{0.0f, 1.0f, 0.0f}});
			l.Blit(tri.v2.map.x * (float)l.w, tri.v2.map.y * (float)l.h, {{0.0f, 1.0f, 0.0f}});
			l.Blit(tri.v3.map.x * (float)l.w, tri.v3.map.y * (float)l.h, {{0.0f, 1.0f, 0.0f}});
		}
		
	}
	
	std::cout << " done!" << std::endl;
	std::cout << "Saving to disk... " << std::flush;
	
	// convert finished lightmap from floating-point to byte
	unsigned char* img = new unsigned char[l.w*l.h*3];
	for (int i = 0; i < l.w*l.h; i++) {
		float r = l.t[i].color.x;
		float g = l.t[i].color.y;
		float b = l.t[i].color.z;
		
		if (r > 1.0f) r = 1.0f;
		if (g > 1.0f) g = 1.0f;
		if (b > 1.0f) b = 1.0f;
		
		if (r < 0.0f) r = 0.0f;
		if (g < 0.0f) g = 0.0f;
		if (b < 0.0f) b = 0.0f;
		
		img[i*3+0] = r * 255.0f;
		img[i*3+1] = g * 255.0f;
		img[i*3+2] = b * 255.0f;
	}
	
	// then write it to a png
	std::string output_path = "data/textures/lightmap/";
	output_path += (const char*)lightmap_name;
	output_path += ".png";
	
	if (!stbi_write_png(output_path.c_str(), l.w, l.h, 3, img, 0)) {
		std::cout << "failed! Couldn't write file to disk." << std::endl;
		return 0;
	}
	
	std::cout << "done!" << std::endl;
	
	return 0;
}