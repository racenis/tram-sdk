// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// Tramway SDK -- Lightmap packer

#include <iostream>
#include <vector>

#include <cstdarg>

#include <framework/logging.h>
#include <framework/file.h>
#include <framework/math.h>

#include <xatlas.h>

using namespace tram;

struct Vertex {
	vec3 pos;
	vec3 nrm;
	vec2 tex;
	vec2 map;
	
	uint32_t mat;
};

struct Triangle {
	uint32_t v1, v2, v3;
};

int XAtlasPrint(const char* format, ...) {
	va_list arg;
	va_start(arg, format);
	printf("XAtlas: ");
	const int result = vprintf(format, arg);
	va_end(arg);
	return result;
}

int main(int argc, const char** argv) {
	SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);

	std::cout << "Tramway SDK -- Lightmap packer" << std::endl;
	
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
	}
	
	/*for (int i = 5; i < argc; i++) {
		if (strcmp(argv[i], "-pack") == 0) {
			packing = true;
		}
	}*/
		
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
		
		t.v1 = file.read_int32();
		t.v2 = file.read_int32();
		t.v3 = file.read_int32();
		
		uint32_t mat = file.read_int32();
		
		vertices[t.v1].mat = mat;
		vertices[t.v2].mat = mat;
		vertices[t.v3].mat = mat;
		
		triangles.push_back(t);
	}
	
	std::cout << "Loaded model " << model_name << ", it has " << vrt_c << " verts, " << tri_c << " tris, " << mat_c << " materials." << std::endl;
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                                PACKER                                 +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	// create atlas
	xatlas::SetPrint(XAtlasPrint, true);
	xatlas::Atlas* atlas = xatlas::Create();

	// add in vertices and indices
	xatlas::MeshDecl meshDecl;
	meshDecl.vertexCount = vertices.size();
	meshDecl.vertexPositionData = &vertices[0].pos;
	meshDecl.vertexPositionStride = sizeof(Vertex);
	
	meshDecl.vertexNormalData = &vertices[0].nrm;
	meshDecl.vertexNormalStride = sizeof(Vertex);
	
	meshDecl.vertexUvData = &vertices[0].tex;
	meshDecl.vertexUvStride = sizeof(Vertex);

	meshDecl.indexCount = triangles.size() * 3;
	meshDecl.indexData = &triangles[0];
	meshDecl.indexFormat = xatlas::IndexFormat::UInt32;
	
	xatlas::AddMeshError error = xatlas::AddMesh(atlas, meshDecl, 0); // idk what 0 hint
	if (error != xatlas::AddMeshError::Success) {
		xatlas::Destroy(atlas);
		std::cout << "Error packing: " << xatlas::StringForEnum(error) << std::endl;
		return 0;
	}

	xatlas::AddMeshJoin(atlas);
	
	xatlas::PackOptions pack_options = xatlas::PackOptions();
	//pack_options.texelsPerUnit = 16.0f;
	pack_options.padding = lightmap_padding;
	pack_options.resolution = lightmap_size;
	
	xatlas::Generate(atlas, xatlas::ChartOptions(), pack_options);

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