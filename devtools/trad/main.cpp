// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// Tramway SDK -- Radiosity tool

// TODO:
// - add option to use multiple worldcells in the same bake
// - allow loading multiple models in the same bake
// - make program go vroom vroom
// - add indirect lighting
// - add emissive materials

/*
 * Idea for new lightmapping process
 *
 * 1. Load in entity definitions and worldcell 
 * 2. Load in models for every entity
 * 3. Allocate a lightmap for every entity
 * 4. Instantiate every triangle in scene
 * 	  - Add triangle to a triangle array
 * 	  - Add a reference to lightmap in it
 * 	  - Add the triangle to the scene AABB tree
 * 5. Compute visibility and filters
 *    - Check which triangles are not too far away from each other
 *    - Check which triangles are intersecting
 * 6. Compute lighting for every triangle
 * 
 * For radiosity we could append low-res surface texels during step 4.
 * Then compute radiosities after that.
 * 
 */

#include <iostream>
#include <vector>

#include <framework/file.h>
#include <framework/math.h>
#include <templates/aabb.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace tram;

struct Entity;

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

struct SceneTriangle {
	Triangle triangle;
	Entity* entity;
	
	// * visibility matrix *
	// * skip matrix *
	
	// what else?
};

struct Lightmap {
	Lightmap() {
		t = nullptr;
		b = nullptr;
		w = 0;
		h = 0;
	}
	
	Lightmap(int width, int height) {
		t = new Texel[width*height];
		b = new bool[width*height];
		for (int i = 0; i < width*height; i++) {
			//t[i] = Texel {.color = {0.0f, 0.0f, 0.0f}};
			t[i] = Texel {.color = {1.0f, 0.5f, 0.5f}};
			b[i] = false;
		}
		w = width;
		h = height;
	}
	
	void Blit(int x, int y, Texel tex) {
		if (x < 0 || y < 0 || x >= w || y >= h) return;
		t[y*w+x] = tex;
		b[y*w+x] = true;
	}
	
	void BlitMix(int x, int y, Texel tex) {
		if (x < 0 || y < 0 || x >= w || y >= h) return;
		t[y*w+x].color += tex.color;
		b[y*w+x] = true;
	}
	
	Texel Sample(int x, int y) {
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		if (x >= w) x = w - 1;
		if (y >= h) y = h - 1;
		return t[y*w+x];
	}
	
	bool Blitted(int x, int y) {
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		if (x >= w) x = w - 1;
		if (y >= h) y = h - 1;
		return b[y*w+x];
	}
	
	void Fill(int pad) {
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				
				if (Blitted(x, y)) continue;
				
				float nearest = INFINITY;
				vec3 color = {0.0f, 0.0f, 0.0f};
				
				for (int s_x = x-pad; s_x < x+pad+1; s_x++) {
					for (int s_y = y-pad; s_y < y+pad+1; s_y++) {
						if (!Blitted(s_x, s_y)) continue;
						
						float dist = glm::distance(vec2(x, y), vec2(s_x, s_y));
						
						if (dist < nearest) {
							nearest = dist;
							color = Sample(s_x, s_y).color;
						}
					}
				}
				
				if (nearest == INFINITY) continue;
				
				t[y*w+x].color = color;
				
				/*
				vec3 color = {0.0f, 0.0f, 0.0f};
				int count = 0;
				
				for (int s_x = x-1; s_x < x+3; s_x++) {
					for (int s_y = y-1; s_y < y+3; s_y++) {
						if (!Blitted(s_x, s_y)) continue;
						color += Sample(s_x, s_y).color;
						count++;
					}
				}

				if (!count) continue;
				
				t[y*w+x].color = color / (float)count;*/
			}
		}
	}
	
	Texel* t;
	bool* b;
	int w, h;
};

struct Model {
	int lightmap_width;
	int lightmap_height;
	
	std::vector<name_t> materials;
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
};

struct Light {
	vec3 pos;
	vec3 color;
	float radius;
};

struct Entity {
	vec3 pos;
	quat rot;
	uint32_t flags;
	name_t id;
	name_t name;
	
	name_t model_name;
	name_t lightmap_name;
	
	Model model;
	Lightmap lightmap;
};

Model LoadModel(const char* model_name) {
	std::string model_path = "data/models/";
	model_path += (const char*)model_name;
	model_path += ".stmdl";
	
	File file(model_path.c_str(), File::READ);
	
	if (!file.is_open()) {
		std::cout << "Error opening model file " << model_path << std::endl;
		abort();
	}
	
	name_t header = file.read_name();
	
	if (header != "STMDLv1") {
		std::cout << "Unrecognized header " << header << " in " << model_path << std::endl;
		abort();
	}
	
	int vrt_c = file.read_int32();
	int tri_c = file.read_int32();
	int mat_c = file.read_int32();
	int metadata_count = file.read_int32();
	
	int lightmap_width = 0;
	int lightmap_height = 0;
	
	for (int i = 0; i < metadata_count; i++) {
		name_t field = file.read_name();

		if (field == "lightmap") {
			lightmap_width = file.read_int32();
			lightmap_height = file.read_int32();
		} else if (field == "near") {
			file.read_float32();
		} else if (field == "far") {
			file.read_float32();
		} else if (field == "origin") {
			file.read_float32();
			file.read_float32();
			file.read_float32();
		} else {
			std::cout << "File" << model_path << "has unrecognized metadata" << field << "skipping entry" << std::endl;
			file.skip_linebreak();
		}
	}
	
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
	
	return {lightmap_width, lightmap_height, materials, vertices, triangles};
}

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

vec3 FindNearestIntersection(AABBTree& tree, std::vector<SceneTriangle>& tris, vec3 pos, vec3 dir) {
	std::vector<uint32_t> results;
	tree.FindIntersection(pos, dir, tree.root, results);

	vec3 closest = {INFINITY, INFINITY, INFINITY};
	
	for (auto res : results) {
		vec3 intr = RayTriangleIntersection(pos, dir, tris[res].triangle.v1.pos, tris[res].triangle.v2.pos, tris[res].triangle.v3.pos);
		if (intr.x == INFINITY) continue;			
		//if(glm::dot(tris[res].v1.nrm, dir) > -0.01f /*&& glm::distance(pos, intr) < 0.1f*/) continue;
		if (glm::distance(pos, intr) < glm::distance(pos, closest)) closest = intr;
	}
	
	return closest;
};

vec3 FindTexelColorFromLights(AABBTree& tree, std::vector<SceneTriangle>& tris, std::vector<Light>& lights, vec3 pos, vec3 normal, vec3 mid) {
	vec3 color = {0.0f, 0.0f, 0.0f};
	
	// we might get a collision with the triangle, on which the texel is located
	// on, so we move it off of the surface a little bit
	pos += 0.01f * normal;
	mid += 0.01f * normal;
	
	// check if clear path to center
	
	vec3 mid_dir = glm::normalize(mid - pos);
	vec3 nearest = FindNearestIntersection(tree, tris, pos, mid_dir);
	if (glm::distance(nearest, pos) < glm::distance(mid, pos)) {
		//return {1.0f, 0.0f, 0.0f};
		//pos += mid_dir * 0.3f;
		//pos = nearest + 0.01f * normal;
		//pos += mid_dir * (glm::distance(nearest, pos) + 0.02f);
		pos = nearest + (mid_dir * 0.1f);
	}
	
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
	//if (lowest == highest) std::cout << "what the fuck" << std::endl;
	if (lowest == highest) return;
	
	Vertex middle;
	if ((tri.v2 == lowest || tri.v3 == lowest) && (tri.v2 == highest || tri.v3 == highest)) middle = tri.v1;
	if ((tri.v1 == lowest || tri.v3 == lowest) && (tri.v1 == highest || tri.v3 == highest)) middle = tri.v2;
	if ((tri.v1 == lowest || tri.v2 == lowest) && (tri.v1 == highest || tri.v2 == highest)) middle = tri.v3;
	
	// position of vertices on the raster image
	int lowest_y = floor(lowest.map.y * (float)p.l_h);
	int middle_y = floor(middle.map.y * (float)p.l_h);
	int highest_y = floor(highest.map.y * (float)p.l_h);
	
	int lowest_x = floor(lowest.map.x * (float)p.l_w);
	int middle_x = floor(middle.map.x * (float)p.l_w);
	int highest_x = floor(highest.map.x * (float)p.l_w);
	
	// push
	if (middle_y == highest_y) {
		//middle_y++;
		//highest_y++;
	} else {
		//highest_y++;
	}
	
	
	
	
	// compute raster line counts
	int low_high_lines = highest_y - lowest_y;
	int low_mid_lines = middle_y - lowest_y;
	int mid_high_lines = highest_y - middle_y;
	
	// compute raster directions
	float low_high_dir = (float)(highest_x - lowest_x) / (float)low_high_lines;
	float low_mid_dir = (float)(middle_x - lowest_x) / (float)low_mid_lines;
	float mid_high_dir = (float)(highest_x - middle_x) / (float)mid_high_lines;
	
	float left_pos = lowest_x;
	float right_pos = lowest_x;
	
	// stupid bug fix
	if (!low_mid_lines) {
		
		left_pos = lowest_x;
		right_pos = middle_x;
		
		/*if (lowest_x > middle_x) {
			left_pos = lowest_x;
			right_pos = middle_x;
		} else {
			left_pos = middle_x;
			right_pos = lowest_x;
			
			std::swap(low_high_dir, mid_high_dir);
		}*/
	}
	
	
	
	for (int row = lowest_y; row < middle_y; row++) {
		left_pos += low_high_dir;
		right_pos += low_mid_dir;

		int from = left_pos;
		int to = right_pos;
		
		if (from > to) std::swap(from, to);
		
		for (int col = from; col < /*<=*/ /* push */ to; col++) {
			vec3 d = GetBarycentric(tri, (float)col / (float)p.l_w, (float)row / (float)p.l_h);
			
			const float thr = 1.0f/3.0f;
			
			vec3 pos = d.x * tri.v1.pos + d.y * tri.v2.pos + d.z * tri.v3.pos;
			vec3 nrm = d.x * tri.v1.nrm + d.y * tri.v2.nrm + d.z * tri.v3.nrm;
			vec3 mid = thr * tri.v1.pos + thr * tri.v2.pos + thr * tri.v3.pos;
			
			raster_f(col, row, pos, nrm, mid);
		}
	}
	
	for (int row = middle_y; row < highest_y; row++) {
		left_pos += low_high_dir;
		right_pos += mid_high_dir;
		
		int from = left_pos;
		int to = right_pos;
		
		if (from > to) std::swap(from, to);

		for (int col = from; col < /*<=*/ /* push */ to; col++) {
			vec3 d = GetBarycentric(tri, (float)col / (float)p.l_w, (float)row / (float)p.l_h);
			
			const float thr = 1.0f/3.0f;
			
			vec3 pos = d.x * tri.v1.pos + d.y * tri.v2.pos + d.z * tri.v3.pos;
			vec3 nrm = d.x * tri.v1.nrm + d.y * tri.v2.nrm + d.z * tri.v3.nrm;
			vec3 mid = thr * tri.v1.pos + thr * tri.v2.pos + thr * tri.v3.pos;
			
			raster_f(col, row, pos, nrm, mid);
		}
	}
}

int main(int argc, const char** argv) {
	stbi_flip_vertically_on_write(true);

	std::cout << "Tramway SDK -- Radiosity lightmapper" << std::endl;
	
	if (argc < 2) {
		std::cout << "Usage: trad worldcell";
		std::cout << "\n\tworldcell is the name of the worldcell, that contains the entities";
		std::cout << "\n\t\tthat will be lightmapped";
		return 0;
	}
	
	const char* worldcell = argv[1];
	
	bool paint_coords = false;
	bool paint_verts = false;
	
	bool force_fullbright = false;
	
	int padding = 1;
	
	std::vector<std::string> worldspawns;
	
	/*if (lightmap_width < 1 || lightmap_height < 1) {
		std::cout << "Lightmap size has to be at least something!!! NOT NEGATIVE!!!" << std::endl;
		return 0;
	}
	
	if ((lightmap_width & (lightmap_width - 1)) != 0 || (lightmap_height & (lightmap_height - 1)) != 0) {
		std::cout << "Lightmap size has to be a power of two." << std::endl;
		return 0;
	}*/
	
	// what we could do is we could add additional parameters, e.g.
	// - only entity to lightmap
	// - skip entity to lightmap
	// - override entity lightmap size
	
	for (int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-coords") == 0) {
			paint_coords = true;
		}
		
		if (strcmp(argv[i], "-verts") == 0) {
			paint_verts = true;
		}
		
		if (strcmp(argv[i], "-pad") == 0) {
			padding = atoi(argv[++i]);
		}
		
		if (strcmp(argv[i], "-fullbright") == 0) {
			force_fullbright = true;
		}
		
		if (strcmp(argv[i], "-worldspawn") == 0) {
			worldspawns.push_back(argv[++i]);
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
	
	File cell(worldcell_path.c_str(), File::READ | File::PAUSE_LINE);
	
	if (!cell.is_open()) {
		std::cout << "\nError opening file: " << worldcell_path << "!" <<std::endl;
		return 0;
	}
	
	name_t file_version = cell.read_name(); cell.skip_linebreak();
	
	if (file_version != "CELLv3") {
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
			entity.flags = cell.read_uint64();
			entity.pos = {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			entity.rot = vec3 {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			entity.model_name = cell.read_name();
			entity.lightmap_name = cell.read_name();
			
			entities.push_back(entity);
		}
		
		if (entity_type == "button") {
			Entity entity;
			
			entity.id = cell.read_name();
			entity.name = cell.read_name();
			entity.flags = cell.read_uint64();
			entity.pos = {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			entity.rot = vec3 {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			
			cell.read_int32();
			entity.model_name = cell.read_name();
			entity.lightmap_name = cell.read_name();
			
			entities.push_back(entity);
		}
		
		if (entity_type == "light") {
			Light light;
			
			cell.read_name();
			cell.read_name();
			cell.read_uint64();
			light.pos = {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			cell.read_float32(); cell.read_float32(); cell.read_float32();
			light.color = {cell.read_float32(), cell.read_float32(), cell.read_float32()};
			light.radius = cell.read_float32();
			
			lights.push_back(light);	
		}
		
		cell.skip_linebreak();
	}

	std::cout << "done!" << std::endl;
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                             MODEL LOADER                              +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	// the scene tree is an acceleration structure for raycast queries.
	// the scene triangles vector will contain all triangles that will be
	// lightmapped.
	AABBTree scene_tree;
	std::vector<SceneTriangle> scene_triangles;
	
	// load up all entity models
	for (auto& entity: entities) {
		
		// load up the model
		entity.model = LoadModel(entity.model_name);
		
		// check if it contains lightmap dimensions
		if (entity.model.lightmap_width == 0 || entity.model.lightmap_height == 0) {
			std::cout << "Model " << entity.model_name << " has invalid lightmap dimensions." << std::endl;
			std::cout << "Width " << entity.model.lightmap_width;
			std::cout << " and height " << entity.model.lightmap_height << std::endl;
			std::cout << "Run tmap to set the lightmap size in the model!" << std::endl;
			return 1;
		}
		
		std::cout << "Loaded " << entity.model_name << " for bake with";
		std::cout << " a size of " << entity.model.lightmap_width;
		std::cout << " by " << entity.model.lightmap_height << " texels." << std::endl;
		
	}
	
	// allocate lightmaps and insert them into the scene tree
	for (auto& entity: entities) {
		
		// allocate lightmap
		entity.lightmap = Lightmap(entity.model.lightmap_width, entity.model.lightmap_height);
		
		// compute local space -> world space matrix
		mat4 matrix = PositionRotationToMatrix(entity.pos, entity.rot);
		
		// insert triangles into scene tree
		for (const auto& tri : entity.model.triangles) {
			SceneTriangle new_tri;
			
			new_tri.triangle.v1.pos = matrix * vec4(tri.v1.pos, 1.0f);
			new_tri.triangle.v2.pos = matrix * vec4(tri.v2.pos, 1.0f);
			new_tri.triangle.v3.pos = matrix * vec4(tri.v3.pos, 1.0f);
			
			new_tri.triangle.v1.nrm = matrix * vec4(tri.v1.nrm, 0.0f);
			new_tri.triangle.v2.nrm = matrix * vec4(tri.v2.nrm, 0.0f);
			new_tri.triangle.v3.nrm = matrix * vec4(tri.v3.nrm, 0.0f);
			
			new_tri.triangle.v1.tex = tri.v1.tex;
			new_tri.triangle.v2.tex = tri.v2.tex;
			new_tri.triangle.v3.tex = tri.v3.tex;
			
			new_tri.triangle.v1.map = tri.v1.map;
			new_tri.triangle.v2.map = tri.v2.map;
			new_tri.triangle.v3.map = tri.v3.map;
			
			new_tri.triangle.mat = tri.mat;
			
			new_tri.entity = &entity;
			
			scene_tree.InsertLeaf(scene_triangles.size(),
								  TriangleAABBMin(new_tri.triangle),
								  TriangleAABBMax(new_tri.triangle));
			scene_triangles.push_back(new_tri);
		}
	}
	
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                        VISIBILITY CALCULATIONS                        +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	// TODO: implement
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                        RADIOSITY CALCULATIONS                         +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	if (lights.size() == 0) {
		std::cout << "Scene contains no lights!" << std::endl;
		force_fullbright = true;
	}
	
	
	// okay, so I think that we'll keep the first pass as-is.
	// as in, the following iteration through all of the scene_triangles is the
	// first radiosity pass.
	// after that we'll insert some additional, optional passes
	
	
	
	
	int last_dots = -1;
	
	// iterate through each triangle in the lightmap and rasterize it
	for (size_t i = 0; i < scene_triangles.size(); i++) {
		const auto& tri = scene_triangles[i];
		
		// draw a progress bar in the command line
		float progress = (float)i / (float)scene_triangles.size();
		int dots = ceil(progress * 62.0f);
		if (last_dots != dots) {
			std::cout << "\rComputing [";
			for (int k = 0; k < 62; k++) std::cout << (k < dots ? (k % 9 == 8 ? ':' : '.') : ' ');
			std::cout << "] " << ceil(progress*100.0f) << "%" << std::flush;
			last_dots = dots;
		}
		
		auto& l = tri.entity->lightmap;
		
		RasterParams image_params = {
			l.w,
			l.h,
			0, 0, 0, 0
		};
		
		if (paint_coords) {
			
			// rasterize triangle and set color to position
			RasterizeTriangle(image_params, tri.triangle, [&](int col, int row, vec3 pos, vec3 nrm, vec3 mid){
				l.Blit(col, row, {pos});
			});
			
		} else if (force_fullbright) {
			
			// rasterize triangle to fullbright
			RasterizeTriangle(image_params, tri.triangle, [&](int col, int row, vec3 pos, vec3 nrm, vec3 mid){
				l.Blit(col, row, {{1.0f, 1.0f, 1.0f}});
			});
			
		} else {
			
			// rasterize triangle and set color to light value
			RasterizeTriangle(image_params, tri.triangle, [&](int col, int row, vec3 pos, vec3 nrm, vec3 mid){
				vec3 color = FindTexelColorFromLights(scene_tree, scene_triangles, lights, pos, nrm, mid);
				l.Blit(col, row, {color});
			});
			
		}
		
		// paint in green dots in the triangle vertex positions
		if (paint_verts) {
			l.Blit(tri.triangle.v1.map.x * (float)l.w, tri.triangle.v1.map.y * (float)l.h, {{0.0f, 1.0f, 0.0f}});
			l.Blit(tri.triangle.v2.map.x * (float)l.w, tri.triangle.v2.map.y * (float)l.h, {{0.0f, 1.0f, 0.0f}});
			l.Blit(tri.triangle.v3.map.x * (float)l.w, tri.triangle.v3.map.y * (float)l.h, {{0.0f, 1.0f, 0.0f}});
		}
		
	}
	
	std::cout << "\rComputing... done!\t\t\t\t\t\t\t      " << std::endl;
	
	std::cout << "Filling gaps... " << std::flush;
	for (auto& entity: entities) entity.lightmap.Fill(padding);
	std::cout << "done!" << std::endl;
	
	std::cout << "Saving to disk... " << std::flush;
	
	for (auto& entity: entities) {
		
		if (!entity.lightmap_name || entity.lightmap_name == "fullbright") continue;
		
		auto& l = entity.lightmap;
		
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
		std::string output_path = "data/textures/";
		output_path += (const char*)entity.lightmap_name;
		output_path += ".png";
		
		if (!stbi_write_png(output_path.c_str(), l.w, l.h, 3, img, 0)) {
			std::cout << "failed! Couldn't write file to disk." << std::endl;
			return 0;
		}
	}
	
	std::cout << "done!" << std::endl;
	
	return 0;
}