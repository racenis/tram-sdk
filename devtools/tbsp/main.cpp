// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// Tramway SDK -- Map converter

#include <iostream>
#include <vector>

#include <framework/logging.h>
#include <framework/file.h>
#include <framework/math.h>

using namespace tram;

struct Vertex {
	vec3 pos;
	vec3 nrm;
	vec2 tex;
	vec2 map;
};

struct Triangle {
	uint32_t v1, v2, v3;
	uint32_t mat;
};




struct Plane {
	float x_offset, y_offset;
	float x_scale, y_scale;
	float rotation;
	
	vec3 p1, p2, p3;
	
	std::string material;
};

struct Brush {
	std::vector<Plane> planes;
};

struct Entity {
	std::vector<Brush> brushes;
	std::string name;
};


struct Edge {
	vec3 p1, p2;
};

struct Polygon {
	std::vector<Edge> edges;
	int plane;
};

vec4 PlaneToEquation(const Plane& plane) {
	vec3 dir1 = glm::normalize(plane.p2 - plane.p1);
	vec3 dir2 = glm::normalize(plane.p3 - plane.p1);
	vec3 cros = glm::cross(dir1, dir2);
	
	//float dist = cros.x * plane.p1.x + cros.y * plane.p1.y + cros.z * plane.p1.z;
	float dist = glm::dot(cros, plane.p1);
	
	return {cros, -dist};
}

int main(int argc, const char** argv) {
	SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);

	std::cout << "Tramway SDK -- Map converter" << std::endl;
	
	std::vector<Entity> entities;
	
	File file("paliktnis.map", MODE_READ | MODE_PAUSE_LINE);
	
	if (!file.is_open()) {
		std::cout << "Error OPENING to model file " << "w" << std::endl;
		return 0;
	}
	
	//int i = 0;
	while (file.cursor < file.cursor_end) {
		
		// comment. skip until end of line
		if (*file.cursor == '/') {
			while (*file.cursor != '\n'/* || *file.cursor != '\0'*/) {
				file.cursor++;
			}
			file.cursor++;
			
			continue;
		}
		
		// start of an entity
		if (*file.cursor == '{') {
			Entity entity;
			
			*file.cursor++; // skip opening bracket
			
			// read in parameters of the entity
			for (;;) {
				
				// find parameter name opening bracket
				while (*file.cursor != '"' && *file.cursor != '{') file.cursor++;
				
				// we reached the part where the brush planes are defined
				if (*file.cursor == '{') break;
				
				file.cursor++; // skip opening quote
				
				char parameter[256];
				char value[256];
				
				// copy in parameter
				for (char* p = parameter; *file.cursor != '"'; p++ && file.cursor++) {
					p[0] = *file.cursor;
					p[1] = '\0';
				}
				
				file.cursor++; 								// skip closing quote
				while (*file.cursor != '"') file.cursor++; 	// find next quote
				file.cursor++;								// skip opening quote
				
				// copy in value
				for (char* v = value; *file.cursor != '"'; v++ && file.cursor++) {
					v[0] = *file.cursor;
					v[1] = '\0';
				}
				
				file.cursor++;  // skip closing quote
				
				if (std::string(parameter) == "classname" && std::string(value) == "worldspawn") {
					entity.name = "worldspawn";
				}
				
				if (std::string(parameter) == "_tb_name") {
					entity.name = value;
				}
			}
			
			// reading in brushes
			for (;;) {
				while(*file.cursor != '{' && *file.cursor != '}') file.cursor++;
				
				if (*file.cursor == '}') break;
				
				Brush brush;
				
				// read in all the planes
				for (;;) {
					vec3 points[3];
					
					// read in planes
					for (int i = 0; i < 3; i++) {
						while (*file.cursor != '(') file.cursor++;
						file.cursor++;
						
						points[i].x = atof(file.cursor);
						while (isspace(*file.cursor)) file.cursor++;
						while (!isspace(*file.cursor)) file.cursor++;
						points[i].y = atof(file.cursor);
						while (isspace(*file.cursor)) file.cursor++;
						while (!isspace(*file.cursor)) file.cursor++;
						points[i].z = atof(file.cursor);
					}
					
					Plane plane;
					
					plane.p1 = points[0];
					plane.p2 = points[1];
					plane.p3 = points[2];
					
					char material[256];
					
					while (*file.cursor != ')') file.cursor++;
					file.cursor++;
					while (isspace(*file.cursor)) file.cursor++;
					
					// copy in material name
					for (char* m = material; !isspace(*file.cursor); m++ && file.cursor++) {
						m[0] = *file.cursor;
						m[1] = '\0';
					}
					
					plane.material = material;
					
					plane.x_offset = atof(file.cursor);
					while (isspace(*file.cursor)) file.cursor++;
					while (!isspace(*file.cursor)) file.cursor++;
					plane.y_offset = atof(file.cursor);
					while (isspace(*file.cursor)) file.cursor++;
					while (!isspace(*file.cursor)) file.cursor++;
					plane.rotation = atof(file.cursor);
					while (isspace(*file.cursor)) file.cursor++;
					while (!isspace(*file.cursor)) file.cursor++;
					plane.x_scale = atof(file.cursor);
					while (isspace(*file.cursor)) file.cursor++;
					while (!isspace(*file.cursor)) file.cursor++;
					plane.y_scale = atof(file.cursor);
					while (isspace(*file.cursor)) file.cursor++;
					while (!isspace(*file.cursor)) file.cursor++;
					
					brush.planes.push_back(plane);
					
					while (isspace(*file.cursor)) file.cursor++;
					
					if (*file.cursor == '}') {
						*file.cursor++;
						break;
					}
				}
				
				entity.brushes.push_back(brush);
			}

			entities.push_back(entity);
		}
		
		file.cursor++;
	}
	
	//const vec3 low_lft_bak = {-1.0f, -1.0f, -1.0f};
	//const vec3 low_rgt_bak = { 1.0f, -1.0f, -1.0f};
	//const vec3 low_rgt_frt = { 1.0f, -1.0f,  1.0f};
	//const vec3 low_lft_frt = {-1.0f, -1.0f,  1.0f};
	//const vec3 hgh_lft_bak = {-1.0f,  1.0f, -1.0f};
	//const vec3 hgh_rgt_bak = { 1.0f,  1.0f, -1.0f};
	//const vec3 hgh_rgt_frt = { 1.0f,  1.0f,  1.0f};
	//const vec3 hgh_lft_frt = {-1.0f,  1.0f,  1.0f};
	
	const vec3 low_lft_bak = {-1000.0f, -1000.0f, -1000.0f};
	const vec3 low_rgt_bak = { 1000.0f, -1000.0f, -1000.0f};
	const vec3 low_rgt_frt = { 1000.0f, -1000.0f,  1000.0f};
	const vec3 low_lft_frt = {-1000.0f, -1000.0f,  1000.0f};
	const vec3 hgh_lft_bak = {-1000.0f,  1000.0f, -1000.0f};
	const vec3 hgh_rgt_bak = { 1000.0f,  1000.0f, -1000.0f};
	const vec3 hgh_rgt_frt = { 1000.0f,  1000.0f,  1000.0f};
	const vec3 hgh_lft_frt = {-1000.0f,  1000.0f,  1000.0f};
	
	const std::vector<Polygon> initial {
		{{{low_lft_bak, hgh_lft_bak}, {hgh_lft_bak, hgh_rgt_bak}, {hgh_rgt_bak, low_rgt_bak}, {low_rgt_bak, low_lft_bak}}, -1},
		{{{low_rgt_bak, hgh_rgt_bak}, {hgh_rgt_bak, hgh_rgt_frt}, {hgh_rgt_frt, low_rgt_frt}, {low_rgt_frt, low_rgt_bak}}, -1},
		{{{low_rgt_frt, hgh_rgt_frt}, {hgh_rgt_frt, hgh_lft_frt}, {hgh_lft_frt, low_lft_frt}, {low_lft_frt, low_rgt_frt}}, -1},
		{{{low_lft_frt, hgh_lft_frt}, {hgh_lft_frt, hgh_lft_bak}, {hgh_lft_bak, low_lft_bak}, {low_lft_bak, low_lft_frt}}, -1},
		{{{low_lft_bak, low_rgt_bak}, {low_rgt_bak, low_rgt_frt}, {low_rgt_frt, low_lft_frt}, {low_lft_frt, low_lft_bak}}, -1},
		{{{hgh_lft_bak, hgh_lft_frt}, {hgh_lft_frt, hgh_rgt_frt}, {hgh_rgt_frt, hgh_rgt_bak}, {hgh_rgt_bak, hgh_lft_bak}}, -1},
	};
	
	

	Entity& ent = entities[0];
	std::cout << "printing " << ent.name << std::endl;
	std::vector<Polygon> mesh;
	for (auto& brush : ent.brushes) {
		// make initial brush
		std::vector<Polygon> brush_polys = initial;
		
		// do clipping
		for (auto& plane : brush.planes) {
			//std::vector<Polygon> clipped_polys;
			
			vec4 eq = PlaneToEquation(plane);
			
			//std::cout << eq.x << " " << eq.y << " " << eq.z << " " << eq.w << std::endl;
			
			for (auto& poly : brush_polys) {
				for (auto& edge : poly.edges) {
					float dist1 = glm::dot(vec3(eq), edge.p1) + eq.w;
					float dist2 = glm::dot(vec3(eq), edge.p2) + eq.w;
					
					if (dist1 < 0.0f) edge.p1 -= vec3(eq) * dist1;
					if (dist1 < 0.0f) edge.p2 -= vec3(eq) * dist2;
					
					std::cout << dist1 << " ";
				}
			}
			
			std::cout << std::endl;
		}
		
		
		
		for (auto& poly : brush_polys) {
			mesh.push_back(poly);
		}
		
		//break;
		
		/*std::cout << "new" << std::endl;
		for (auto& p : brush.planes) {
			std::cout << p.p1.x << " " << p.p1.y << " " << p.p1.z << " ";
			std::cout << p.p2.x << " " << p.p2.y << " " << p.p2.z << " ";
			std::cout << p.p3.x << " " << p.p3.y << " " << p.p3.z << " ";
			std::cout << p.material << std::endl;
		}*/
	}

	
	
	std::vector<name_t> materials = {"dev/wall32x32"};
	std::vector<Vertex> vertices;
	std::vector<Triangle> indices;
	
	for (auto& poly : mesh) {
		vec3 dir1 = glm::normalize(poly.edges[1].p1 - poly.edges[0].p1);
		vec3 dir2 = glm::normalize(poly.edges[2].p1 - poly.edges[0].p1);
		vec3 normal = glm::normalize(glm::cross(dir1, dir2));
		
		std::cout << "outputting polygon" << std::endl;
		
		uint32_t v_index = vertices.size();
		
		for (auto& edge : poly.edges) {
			vec3 pos = edge.p1 * (1.0f/32.0f);
			vec2 tex = vec2{edge.p1.x, edge.p1.y} * (1.0f/32.0f);
			
			vertices.push_back({
				{pos.x, pos.z, -pos.y},
				{normal.x, normal.z, -normal.y},
				tex,
				tex
			});
		}
		
		for (int i = 1; i < poly.edges.size() - 1; i++) {
			indices.push_back({v_index, v_index+i, v_index+i+1, 0});
		}
		
		
	}
	
	/*
	if (argc < 3) {
		std::cout << "Usage: tmap model size [padding]";
		std::cout << "\n\tmodel is the name of the model, which will be packed";
		std::cout << "\n\tsize is the size of the lightmap, in pixels";
		std::cout << "\n\tpadding is the padding of lightmap segemnts, in pixels";
		return 0;
	}
	
	const char* model_name = argv[1];
	int lightmap_size = atoi(argv[2]);
	int lightmap_padding = argc >= 4 ? atoi(argv[3]) : 2;
	
	if ((lightmap_size & (lightmap_size - 1)) != 0 || lightmap_size < 1) {
		std::cout << "Lightmap size has to be a power of two." << std::endl;
		return 0;
	}*/
	
	/*for (int i = 5; i < argc; i++) {
		if (strcmp(argv[i], "-pack") == 0) {
			packing = true;
		}
	}*/
		
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                             MODEL WRITER                              +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	File output("../../data/models/paliktnis.stmdl", MODE_WRITE);
	
	if (!output.is_open()) {
		std::cout << "Error writing to model file " << "../../data/models/paliktnis.stmdl" << std::endl;
		return 0;
	}

	std::cout << "MODEL packed! Writing to disk..." << std::flush;
	
	output.write_uint32(vertices.size());
	output.write_uint32(indices.size());
	output.write_uint32(materials.size());
	
	output.write_newline();
	
	for (auto& mat : materials) {
		output.write_name(mat);
		output.write_newline();
	}
	
	for (auto& vertex : vertices) {
		output.write_float32(vertex.pos.x);
		output.write_float32(vertex.pos.y);
		output.write_float32(vertex.pos.z);
		
		output.write_float32(vertex.nrm.x);
		output.write_float32(vertex.nrm.y);
		output.write_float32(vertex.nrm.z);
		
		output.write_float32(vertex.tex.x);
		output.write_float32(vertex.tex.y);
		
		output.write_float32(vertex.map.x);
		output.write_float32(vertex.map.x);

		output.write_newline();
	}
	
	for (auto& index : indices) {
		output.write_uint32(index.v1);
		output.write_uint32(index.v2);
		output.write_uint32(index.v3);
		
		output.write_uint32(index.mat);
		
		output.write_newline();
	}
	
	std::cout << "done!" << std::endl;
	
	return 0;
}