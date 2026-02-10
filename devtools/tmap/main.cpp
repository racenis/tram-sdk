// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// Tramway SDK -- Lightmap packer

#include <iostream>
#include <vector>
#include <map>

#include <cstdarg>
#include <cstring>

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

bool VertexCanMerge(const Vertex& a, const Vertex& b) {
	return
		a.pos == b.pos &&
		a.nrm == b.nrm &&
		a.tex == b.tex &&
		a.mat == b.mat;
}

int XAtlasPrint(const char* format, ...) {
	va_list arg;
	va_start(arg, format);
	printf("XAtlas: ");
	const int result = vprintf(format, arg);
	va_end(arg);
	return result;
}

int main(int argc, const char** argv) {
	std::cout << "Tramway SDK -- Lightmap packer" << std::endl;
	
	if (argc < 3) {
		std::cout << "Usage: tmap <model> <size> [options]";
		std::cout << "\n\nArguments:";
		std::cout << "\n  <model>\t\tName of the model, which will be packed";
		std::cout << "\n  <size> is the size of the lightmap, in pixels";
		
		std::cout << "\n\nOptions:";
		std::cout << "\n  -pad <pixels> Padding of lightmap segments, in pixels";
		std::cout << "\n  -density <integer> Number of texels per meter";
		return 0;
	}
	
	const char* model_name = argv[1];
	int lightmap_size = atoi(argv[2]);
	int lightmap_padding = 2;
	int lightmap_density = 4;
	
	if ((lightmap_size & (lightmap_size - 1)) != 0 || lightmap_size < 1) {
		std::cout << "Lightmap size has to be a power of two." << std::endl;
		return 0;
	}
	
	for (int i = 3; i < argc; i++) {
		if (strcmp(argv[i], "-pad") == 0) {
			lightmap_padding = atoi(argv[++i]);
		}
		
		if (strcmp(argv[i], "-density") == 0) {
			lightmap_density = atoi(argv[++i]);
		}
	}
		
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                             MODEL LOADER                              +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
		
	std::string model_path = "data/models/";
	model_path += (const char*)model_name;
	model_path += ".stmdl";
	
	File file(model_path.c_str(), File::READ);
	
	if (!file.is_open()) {
		std::cout << "Error opening model file " << model_path << std::endl;
		return 1;
	}
	
	name_t header = file.read_name();
	
	if (header != "STMDLv1") {
		std::cout << "Unrecognized header " << header << " in file " << model_path << std::endl;
		return 1;
	}
	
	int vrt_c = file.read_int32();
	int tri_c = file.read_int32();
	int mat_c = file.read_int32();
	
	int metadata_count = file.read_int32();
	
	vec3 origin = {0.0f, 0.0f, 0.0f};
	float near = 0.0f;
	float far = INFINITY;
	
	for (int i = 0; i < metadata_count; i++) {
		name_t field = file.read_name();

		if (field == "lightmap") {
			// I think that we just ignore this for now
			file.read_int32();
			file.read_int32();
		} else if (field == "near") {
			near = file.read_float32();
		} else if (field == "far") {
			far = file.read_float32();
		} else if (field == "origin") {
			origin = {file.read_float32(), file.read_float32(), file.read_float32()};
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
	
	
	
	std::vector<Vertex> merged_vertices;	// similar vertices merged
	std::map<int, int> merged_mapping;		// mapping from unmerged to merged
	
	for (int v = 0; v < vertices.size(); v++) {
		int merge_index = -1;
		
		for (int m = 0; m < merged_vertices.size(); m++) {
			if (VertexCanMerge(vertices[v], merged_vertices[m])) {
				merge_index = m;
				break;
			}
		}
		
		if (merge_index == -1) {
			merge_index = merged_vertices.size();
			merged_vertices.push_back(vertices[v]);
		}
		
		merged_mapping[v] = merge_index;
	}
	
	int vert_decrease = vertices.size() - merged_vertices.size();
	float vert_decrese_prop = (float)vert_decrease / (float)vertices.size();
	std::cout << "Merged " << vert_decrease << " vertices, " << vert_decrese_prop * 100.0f << "% decrease." << std::endl;
	
	for (auto& tri : triangles) {
		tri.v1 = merged_mapping[tri.v1];
		tri.v2 = merged_mapping[tri.v2];
		tri.v3 = merged_mapping[tri.v3];
	}
	
	std::swap(merged_vertices, vertices);
	
	
	
	
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
	
	meshDecl.vertexUvData = &vertices[0].map;
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

	xatlas::ChartOptions chart_options = xatlas::ChartOptions();
	//chart_options.useInputMeshUvs = true;
	//chart_options.maxCost = 100000.0f;
	//chart_options.maxIterations = 160;
	xatlas::ComputeCharts(atlas, chart_options);
	
	xatlas::PackOptions pack_options = xatlas::PackOptions();
	//pack_options.texelsPerUnit = 16.0f;
	pack_options.padding = lightmap_padding;
	pack_options.resolution = lightmap_size;
	pack_options.texelsPerUnit = lightmap_density;
	//pack_options.bilinear = true;
	//pack_options.blockAlign = true;
	//pack_options.bruteForce = true;
	//pack_options.rotateChartsToAxis = false;
	//pack_options.rotateCharts = false;
	
	xatlas::PackCharts(atlas, pack_options);
	//xatlas::Generate(atlas, chart_options, pack_options);

	
	
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                             MODEL WRITER                              +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	File output(model_path.c_str(), File::WRITE);
	
	if (!output.is_open()) {
		std::cout << "Error writing to model file " << model_path << std::endl;
		return 0;
	}

	std::cout << "Lightmap packed! Writing to disk..." << std::flush;
	
	const xatlas::Mesh& new_mesh = atlas->meshes[0];
	
	int metadatas = 1; // at least lightmap size
	
	if (near != 0.0f) metadatas++;
	if (far != INFINITY) metadatas++;
	if (origin != vec3(0.0f, 0.0f, 0.0f)) metadatas++;
	
	output.write_name("STMDLv1");
	
	output.write_uint32(new_mesh.vertexCount);
	output.write_uint32(new_mesh.indexCount / 3);
	output.write_uint32(mat_c);
	output.write_int32(metadatas);
	
	output.write_newline();
	
	output.write_name("lightmap");
	output.write_int32(lightmap_size);
	output.write_int32(lightmap_size);
	
	output.write_newline();
	
	if (near != 0.0f) {
		output.write_name("near");
		output.write_float32(near);
		output.write_newline();
	}
	
	if (far != INFINITY) {
		output.write_name("far");
		output.write_float32(far);
		output.write_newline();
	}
	
	if (origin != vec3(0.0f, 0.0f, 0.0f)) {
		output.write_name("origin");
		output.write_float32(origin.x);
		output.write_float32(origin.y);
		output.write_float32(origin.z);
		output.write_newline();
	}
	
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
		
		output.write_float32((new_mesh.vertexArray[i].uv[0]) / (float)atlas->width);
		output.write_float32((new_mesh.vertexArray[i].uv[1]) / (float)atlas->height);

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