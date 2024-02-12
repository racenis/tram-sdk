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
	
	bool operator== (const Plane& pp) {
		return p1 == pp.p1 && p2 == pp.p2 && p3 == pp.p3
			&& x_offset == pp.x_offset && y_offset == pp.y_offset 
			&& x_scale == pp.x_scale && y_scale == pp.y_scale 
			&& rotation == pp.rotation && material == pp.material;
	}
};

struct Brush {
	std::vector<int> planes;
};

struct Entity {
	std::vector<Plane> planes;
	std::vector<Brush> brushes;
	
	std::string name;
};

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
					
					int plane_index = -1;
					for (int i = 0; i < entity.planes.size(); i++) {
						if (entity.planes[i] == plane) {
							plane_index = i;
							break;
						}
					}
					
					if (plane_index == -1) {
						plane_index = entity.planes.size();
						entity.planes.push_back(plane);
					}
					
					brush.planes.push_back(plane_index);
					
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
	
	
	
	/*
	Entity& ent = entities[0];
	std::cout << "printing " << ent.name << std::endl;
	for (auto& brush: ent.brushes) {
		std::cout << "new" << std::endl;
		for (int plane : brush.planes) {
			Plane p = ent.planes[plane];
			std::cout << p.p1.x << " " << p.p1.y << " " << p.p1.z << " ";
			std::cout << p.p2.x << " " << p.p2.y << " " << p.p2.z << " ";
			std::cout << p.p3.x << " " << p.p3.y << " " << p.p3.z << " ";
			std::cout << p.material << std::endl;
		}
	}*/
	
	
	
	
	
	
	
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
	
	/*File output(model_path.c_str(), MODE_WRITE);
	
	if (!output.is_open()) {
		std::cout << "Error writing to model file " << model_path << std::endl;
		return 0;
	}

	std::cout << "Lightmap packed! Writing to disk..." << std::flush;
	
	const xatlas::Mesh& new_mesh = atlas->meshes[0];
	
	output.write_uint32(new_mesh.vertexCount);
	output.write_uint32(new_mesh.indexCount / 3);
	output.write_uint32(mat_c);
	
	output.write_newline();
	
	for (auto& mat : materials) {
		output.write_name(mat);
		output.write_newline();
	}
	
	for (uint32_t i = 0; i < new_mesh.vertexCount; i++) {
		uint32_t input = new_mesh.vertexArray[i].xref;
		
		output.write_float32(vertices[input].pos.x);
		output.write_float32(vertices[input].pos.y);
		output.write_float32(vertices[input].pos.z);
		
		output.write_float32(vertices[input].nrm.x);
		output.write_float32(vertices[input].nrm.y);
		output.write_float32(vertices[input].nrm.z);
		
		output.write_float32(vertices[input].tex.x);
		output.write_float32(vertices[input].tex.y);
		
		output.write_float32(new_mesh.vertexArray[i].uv[0] / atlas->width);
		output.write_float32(new_mesh.vertexArray[i].uv[1] / atlas->height);

		output.write_newline();
	}
	
	for (uint32_t i = 0; i < new_mesh.indexCount; i++) {
		output.write_uint32(new_mesh.indexArray[i]);
		
		if (i % 3 != 2) continue;
		
		uint32_t input = new_mesh.vertexArray[new_mesh.indexArray[i]].xref;
		
		output.write_uint32(vertices[input].mat);
		output.write_newline();
	}*/
	
	std::cout << "done!" << std::endl;
	
	return 0;
}