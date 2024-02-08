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

int main(int argc, const char** argv) {
	std::cout << "HELLO WORLD" << std::endl;
	
	stbi_flip_vertically_on_write(true);
	
	File file("../../data/models/movs.stmdl", MODE_READ);
	//File file("../../data/models/movs2.stmdl", MODE_READ);
	//File file("../../data/models/tepik.stmdl", MODE_READ);
	
	if (!file.is_open()) {
		std::cout << "ERROR OPENING FILE" << std::endl;
		return 0;
	}
	
	int vrt_c = file.read_int32();
	int tri_c = file.read_int32();
	int mat_c = file.read_int32();
	
	std::cout << "Opened model; " << vrt_c << " verts, " << tri_c << " tris, " << mat_c << " materials." << std::endl;
	
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
	
	
	// this tree here will contain all of the triangles in the scene, at the
	// of the radiosity computation.
	// we will use it to determine if there is a clear path between two points
	AABBTree all_tree;
	std::vector<Triangle> all_tris;
	
	for (const auto& tri : triangles) {
		all_tree.InsertLeaf(all_tris.size(), TriangleAABBMin(tri), TriangleAABBMax(tri));
		all_tris.push_back(tri);
	}
	
	Lightmap l(1024, 1024);
	
	const float scanline = 1.0f/l.h;
	const float column = 1.0f/l.w;
	
	auto blit = [&](float x, float y, vec3 color) {
		if (x > 1.0f || y > 1.0f || x < 0.0f || y < 0.0f) return;
		int xi = x*1023.0f;
		int yi = y*1023.0f;
		l.t[yi*1024+xi].color = color;
	};
	
	for (const auto& tri : triangles) {
		
		//static int i = 0;
		//i++;
		
		//if (i != 2) continue;
		
		blit(tri.v1.map.x, tri.v1.map.y, {1.0f, 1.0f, 1.0f});
		blit(tri.v2.map.x, tri.v2.map.y, {1.0f, 1.0f, 1.0f});
		blit(tri.v3.map.x, tri.v3.map.y, {1.0f, 1.0f, 1.0f});
		
		Vertex lowest = tri.v1;
		if (tri.v1.map.y < lowest.map.y) lowest = tri.v1;
		if (tri.v2.map.y < lowest.map.y) lowest = tri.v2;
		if (tri.v3.map.y < lowest.map.y) lowest = tri.v3;
		
		Vertex highest = tri.v1;
		if (tri.v1.map.y > highest.map.y) highest = tri.v1;
		if (tri.v2.map.y > highest.map.y) highest = tri.v2;
		if (tri.v3.map.y > highest.map.y) highest = tri.v3;
		
		
		if (lowest == highest) std::cout << "what the fuck" << std::endl;
		
		Vertex middle;
		if ((tri.v2 == lowest || tri.v3 == lowest) && (tri.v2 == highest || tri.v3 == highest)) middle = tri.v1;
		if ((tri.v1 == lowest || tri.v3 == lowest) && (tri.v1 == highest || tri.v3 == highest)) middle = tri.v2;
		if ((tri.v1 == lowest || tri.v2 == lowest) && (tri.v1 == highest || tri.v2 == highest)) middle = tri.v3;
		
		//std::cout << (highest.map.y - lowest.map.y) / scanline << std::endl;
		
		int low_high_lines = ceil((highest.map.y - lowest.map.y) / scanline);
		int low_mid_lines = ceil((middle.map.y - lowest.map.y) / scanline);
		int mid_high_lines = ceil((highest.map.y - middle.map.y) / scanline);
		
		vec2 low_high_dir = (highest.map - lowest.map) / (float)low_high_lines;
		vec2 low_mid_dir = (middle.map - lowest.map) / (float)low_mid_lines;
		vec2 mid_high_dir = (highest.map - middle.map) / (float)mid_high_lines;
		
		vec3 low_high_dir_wrd = (highest.pos - lowest.pos) / (float)low_high_lines;
		vec3 low_mid_dir_wrd = (middle.pos - lowest.pos) / (float)low_mid_lines;
		vec3 mid_high_dir_wrd = (highest.pos - middle.pos) / (float)mid_high_lines;
		
		//std::cout << "hi " << highest.map.x << " " << highest.map.y << std::endl;
		//std::cout << "lo " << lowest.map.x << " " << lowest.map.y << std::endl;
		//std::cout << "to " << low_high_dir.x << " " << low_high_dir.y << std::endl;
		
		//std::cout << "hi " << highest.pos.x << " " << highest.pos.y << " " << highest.pos.z << std::endl;
		//std::cout << "lo " << lowest.pos.x << " " << lowest.pos.y << " " << lowest.pos.z << std::endl;
		//std::cout << "to " << low_high_dir_wrd.x << " " << low_high_dir_wrd.y << " " << low_high_dir_wrd.z << std::endl;
		
		
		vec2 left_pos = lowest.map;
		vec2 right_pos = lowest.map;
		
		vec3 left_pos_wrd = lowest.pos;
		vec3 right_pos_wrd = lowest.pos;
		
		
		int line = lowest.map.y * (float)l.h;//(float)l.h - 1.0f;
		
		for (int i = 0; i < low_mid_lines; i++) {
			left_pos += low_high_dir;
			right_pos += low_mid_dir;
			
			left_pos_wrd += low_high_dir_wrd;
			right_pos_wrd += low_mid_dir_wrd;
			
			int from = left_pos.x * (float)l.w;
			int to = right_pos.x * (float)l.w;
			
			vec3 mov_wrd = (right_pos_wrd - left_pos_wrd) / fabs((float)(from-to));
			vec3 pos_wrd = left_pos_wrd;
			
			if (from > to) {
				std::swap(from, to);
				mov_wrd = -mov_wrd;
			}
			
			for (int col = from; col < to; col++) {
				const float x = (float)col / (float)l.w;
				const float y = (float)line / (float)l.h;
				const vec2 v1 = tri.v2.map - tri.v1.map;
				const vec2 v2 = tri.v3.map - tri.v1.map;
				const vec2 v3 = vec2(x, y) - tri.v1.map;
				const float dominator = v1.x * v2.y - v2.x * v1.y;
				const float d2 = (v3.x * v2.y - v2.x * v3.y) / dominator;
				const float d3 = (v1.x * v3.y - v3.x * v1.y) / dominator;
				const float d1 = 1.0f - d2 - d3;
			
				
				//std::cout << d1 << " " << d2 << " " << d3 << " " << (d1+d2+d3) << " " << nm << " " << (d1+d2+d3)*nm << std::endl; 
				
				vec3 pos = d1*tri.v1.pos + d2*tri.v2.pos + d3*tri.v3.pos;
				
				//std::cout << glm::distance(pos, pos_wrd) << std::endl;
				
				l.Blit(col, line, {glm::max(0.0f, 5.5f - glm::length(pos)) * pos});
				//l.Blit(col, line, {glm::max(0.0f, 5.5f - glm::length(pos_wrd)) *pos_wrd});
				//l.Blit(col, line, {pos_wrd});
				//l.Blit(col, line, {pos});
				
				pos_wrd += mov_wrd;
			}

			line++;
		}
		
		right_pos = middle.map;
		right_pos_wrd = middle.pos;
		
		for (int i = low_mid_lines; i < low_high_lines; i++) {
			left_pos += low_high_dir;
			right_pos += mid_high_dir;
			
			left_pos_wrd += low_high_dir_wrd;
			right_pos_wrd += mid_high_dir_wrd;
			
			int from = left_pos.x * (float)l.w;
			int to = right_pos.x * (float)l.w;
			
			vec3 mov_wrd = (right_pos_wrd - left_pos_wrd) / fabs((float)(from-to));
			vec3 pos_wrd = left_pos_wrd;
			
			if (from > to) {
				std::swap(from, to);
				mov_wrd = -mov_wrd;
			}
			
			for (int col = from; col < to; col++) {
				const float x = (float)col / (float)l.w;
				const float y = (float)line / (float)l.h;
				const vec2 v1 = tri.v2.map - tri.v1.map;
				const vec2 v2 = tri.v3.map - tri.v1.map;
				const vec2 v3 = vec2(x, y) - tri.v1.map;
				const float dominator = v1.x * v2.y - v2.x * v1.y;
				const float d2 = (v3.x * v2.y - v2.x * v3.y) / dominator;
				const float d3 = (v1.x * v3.y - v3.x * v1.y) / dominator;
				const float d1 = 1.0f - d2 - d3;
			
				 
				//std::cout << d1 << " " << d2 << " " << d3 << " " << (d1+d2+d3) << " " << nm << " " << (d1+d2+d3)*nm << std::endl; 
				
				vec3 pos = d1*tri.v1.pos + d2*tri.v2.pos + d3*tri.v3.pos;
				
				//std::cout << glm::distance(pos, pos_wrd) << std::endl;
				
				l.Blit(col, line, {glm::max(0.0f, 5.5f - glm::length(pos)) * pos});
				//l.Blit(col, line, {glm::max(0.0f, 5.5f - glm::length(pos_wrd)) *pos_wrd});
				//l.Blit(col, line, {pos_wrd});
				//l.Blit(col, line, {pos});
				
				pos_wrd += mov_wrd;
				
				//return 0;
			}

			line++;
		}
		
		blit(lowest.map.x, lowest.map.y, {0.0f, 1.0f, 0.0f});
		blit(highest.map.x, highest.map.y, {1.0f, 0.0f, 0.0f});
		blit(middle.map.x, middle.map.y, {1.0f, 1.0f, 0.0f});
		
		//break;
	}
	
	
	
	
	
	
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
	stbi_write_png("output.png", l.w, l.h, 3, img, 0);
	
	//std::cout << triangles[0].v1.pos.x << " " << triangles[0].v2.pos.y << " " << triangles[0].v3.pos.z << " "  << triangles[0].mat << std::endl;
	
	return 0;
}