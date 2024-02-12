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


int main(int argc, const char** argv) {
	SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);

	std::cout << "Tramway SDK -- Map converter" << std::endl;
	
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
	
	File output(model_path.c_str(), MODE_WRITE);
	
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
	}
	
	std::cout << "done!" << std::endl;
	
	return 0;
}