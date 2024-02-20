// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// Tramway SDK -- Map converter

// TODO:

// - mesh simplification(?) sometimes generates edges with NaN vertices
//		- this is not good and should be fixed.

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>

#include <framework/logging.h>
#include <framework/file.h>
#include <framework/math.h>

#include <stb_image.h>

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

struct Material {
	std::string name;
	int width, height;
};

struct Plane {
	float x_offset, y_offset;
	float x_scale, y_scale;
	float rotation;
	
	vec3 p1, p2, p3;
	
	std::string material;
};

struct Edge {
	vec3 p1, p2;
};

struct Polygon {
	std::vector<Edge> edges;
	Plane plane;
};

struct Brush {
	std::vector<Plane> planes;
	std::vector<Polygon> polys;
	
	std::vector<vec3> hull;
};

struct Entity {
	std::vector<Brush> brushes;
	std::string name;
	
	std::vector<Material> materials;
	std::vector<Vertex> vertices;
	std::vector<Triangle> indices;
};

vec4 PlaneToEquation(const Plane& plane) {
	vec3 dir1 = plane.p2 - plane.p1;
	vec3 dir2 = plane.p3 - plane.p1;
	vec3 cros = glm::normalize(glm::cross(dir1, dir2));
	
	float dist = glm::dot(cros, plane.p1);
	
	return {cros, -dist};
}

// returns 0 if needs clipped, -1 if thrown away, 1 if kept
int NeedsClipped(Polygon poly, vec4 eq, float bias = 0.0f) {
	int inside_vertices = 0;
	int outside_vertices = 0;

	for (auto& edge : poly.edges) {
		float dist1 = glm::dot(vec3(eq), edge.p1) + eq.w;
		float dist2 = glm::dot(vec3(eq), edge.p2) + eq.w; 

		if (dist1 < bias) outside_vertices++; else inside_vertices++;
		if (dist2 < bias) outside_vertices++; else inside_vertices++;
	}

	if (outside_vertices == 0) {
		return 1;
	}

	if (inside_vertices == 0) {
		return -1;
	}
	
	return 0;
}

std::pair<Polygon, Edge> Clip(Polygon poly, vec4 eq) {
	std::vector<Edge> new_edges;
	Edge new_edge = {{INFINITY, INFINITY, INFINITY}, {0, 0, 0}};
	for (auto& edge : poly.edges) {
		float dist1 = glm::dot(vec3(eq), edge.p1) + eq.w;
		float dist2 = glm::dot(vec3(eq), edge.p2) + eq.w;
		
		if (dist1 < 0.0f && dist2 < 0.0f) {
			continue;
		}
		
		if (dist1 < 0.0f) {
			vec3 l0 = edge.p1;
			vec3 l = glm::normalize(edge.p2 - edge.p1);
			vec3 n = vec3(eq);
			vec3 p0 = n * -eq.w;
			float d = glm::dot((p0-l0), n) / glm::dot(l, n);
			edge.p1 = l0 + l*d;
			
			if (new_edge.p1.x == INFINITY) {
				new_edge.p1 = edge.p1;
			} else {
				new_edge.p2 = edge.p1;
				new_edges.push_back(new_edge);
			}
		}
		
		if (dist2 < 0.0f) {
			vec3 l0 = edge.p1;
			vec3 l = glm::normalize(edge.p2 - edge.p1);
			vec3 n = vec3(eq);
			vec3 p0 = n * -eq.w;

			float d = glm::dot((p0-l0), n) / glm::dot(l, n);

			edge.p2 = l0 + l*d;
			
			if (new_edge.p1.x == INFINITY) {
				new_edge.p1 = edge.p2;
			} else {
				new_edge.p2 = edge.p2;
				new_edges.push_back(new_edge);
			}
		}
		
		new_edges.push_back(edge);
	}
	
	poly.edges = new_edges;
	
	return {poly, new_edge};
}

int main(int argc, const char** argv) {
	SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);

	std::cout << "Tramway SDK -- Map converter" << std::endl;
	
	
	if (argc < 2) {
		std::cout << "Usage: tbsp map [flags]";
		std::cout << "\n\tmap is the name of the map";
		std::cout << "\n\tflags are flags";
		std::cout << "\n\t\t-nostmdl to skip outputting 3D model";
		std::cout << "\n\t\t-nocollmdl to skip outputting collsion model";
		std::cout << "\n\t\t-nohidden to skip hidden surface removal";
		std::cout << "\n\t\t-nonodraw to skip nodraw surface removal";
		std::cout << "\n\t\t-only entity to output only entity";
		std::cout << "\n\t\t-worldspawn name to override worldspawn name";
		return 0;
	}
	
	const char* map_name = argv[1];
	const char* entity_only = nullptr;
	const char* worldspawn_name = "worldspawn";
	
	bool skip_model = false;
	bool skip_collision = false;
	bool skip_hidden = false;
	bool skip_nodraw = false;
	
	for (int i = 3; i < argc; i++) {
		if (strcmp(argv[i], "-nostmdl") == 0)		skip_model = true;
		if (strcmp(argv[i], "-nocollmdl") == 0)		skip_collision = true;
		if (strcmp(argv[i], "-nohidden") == 0)		skip_hidden = true;
		if (strcmp(argv[i], "-nonodraw") == 0)		skip_nodraw = true;		
		if (strcmp(argv[i], "-only") == 0)			entity_only = argv[++i];
		if (strcmp(argv[i], "-worldspawn") == 0)	worldspawn_name = argv[++i];
	}
	
	
	std::vector<Entity> entities;
	
	// + --------------------------------------------------------------------- +
	// |                                                                       |
	// |                               MAP LOADER                              |
	// |                                                                       |
	// + --------------------------------------------------------------------- +
	
	// This part is pretty simple -- first we load the map file into memory,
	// then we parse it. The file format itself consists of text. 

	// This parser implementation is pretty bad and might not work with map 
	// files generated by all programs.
	
	char map_path[200] = "";
	
	// check if absolute path
	if (map_name[0] == '/' || map_name[1] == ':') {
		strcpy(map_path, map_name);
		strcat(map_path, ".map");
	} else {
		strcpy(map_path, "");
		strcat(map_path, map_name);
		strcat(map_path, ".map");
	}
	
	File file(map_path, MODE_READ | MODE_PAUSE_LINE);
	
	if (!file.is_open()) {
		std::cout << "Error opening map file " << map_path << std::endl;
		return 1;
	}
	
	std::cout << "Reading map file " << map_path << "... " << std::flush;
	
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
					entity.name = worldspawn_name;
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

			if (!entity_only || entity.name == entity_only) {
				entities.push_back(entity);
			}	
		}
		
		file.cursor++;
	}
	
	std::cout << " done!" << std::endl;
	
	// + --------------------------------------------------------------------- +
	// |                                                                       |
	// |                            POLYGON CONVERTER                          |
	// |                                                                       |
	// + --------------------------------------------------------------------- +
	
	// Now that the map has been parsed and loaded into memory, it is time to
	// convert the brushes (which are defined as planes), into polygons, which 
	// we can use later for rendering.
	
	// For each brush we will initially create a very large polygonal cube. Then
	// we will start clipping it with each brush plane, until we have clipped it
	// into the shape of the brush.
	
	const vec3 low_lft_bak = {-1000.0f, -1000.0f, -1000.0f};
	const vec3 low_rgt_bak = { 1000.0f, -1000.0f, -1000.0f};
	const vec3 low_rgt_frt = { 1000.0f, -1000.0f,  1000.0f};
	const vec3 low_lft_frt = {-1000.0f, -1000.0f,  1000.0f};
	const vec3 hgh_lft_bak = {-1000.0f,  1000.0f, -1000.0f};
	const vec3 hgh_rgt_bak = { 1000.0f,  1000.0f, -1000.0f};
	const vec3 hgh_rgt_frt = { 1000.0f,  1000.0f,  1000.0f};
	const vec3 hgh_lft_frt = {-1000.0f,  1000.0f,  1000.0f};
	
	const Plane blank = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		{0.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		"none"
	};
	
	const std::vector<Polygon> initial {
		{{{low_lft_bak, hgh_lft_bak}, {hgh_lft_bak, hgh_rgt_bak}, {hgh_rgt_bak, low_rgt_bak}, {low_rgt_bak, low_lft_bak}}, blank},
		{{{low_rgt_bak, hgh_rgt_bak}, {hgh_rgt_bak, hgh_rgt_frt}, {hgh_rgt_frt, low_rgt_frt}, {low_rgt_frt, low_rgt_bak}}, blank},
		{{{low_rgt_frt, hgh_rgt_frt}, {hgh_rgt_frt, hgh_lft_frt}, {hgh_lft_frt, low_lft_frt}, {low_lft_frt, low_rgt_frt}}, blank},
		{{{low_lft_frt, hgh_lft_frt}, {hgh_lft_frt, hgh_lft_bak}, {hgh_lft_bak, low_lft_bak}, {low_lft_bak, low_lft_frt}}, blank},
		{{{low_lft_bak, low_rgt_bak}, {low_rgt_bak, low_rgt_frt}, {low_rgt_frt, low_lft_frt}, {low_lft_frt, low_lft_bak}}, blank},
		{{{hgh_lft_bak, hgh_lft_frt}, {hgh_lft_frt, hgh_rgt_frt}, {hgh_rgt_frt, hgh_rgt_bak}, {hgh_rgt_bak, hgh_lft_bak}}, blank},
	};
	
	for (size_t i = 0; i < entities.size(); i++) {
		auto& entity = entities[i];
		
		std::cout << "[" << (i + 1) << "/" << entities.size() << "] Converting "<< entity.name <<" to polygon soup... " << std::flush;
		
		for (auto& brush : entity.brushes) {
			// make initial polygonal mega-cube
			brush.polys = initial;

			// clip it down with brush planes
			for (auto& plane : brush.planes) {
				std::vector<Polygon> clipped_polys;
				
				vec4 eq = PlaneToEquation(plane);
				
				Polygon new_polygon = {.plane = plane}; // may or may not be filled

				for (auto& poly : brush.polys) {
					int needs_clipped = NeedsClipped(poly, eq);
					if (needs_clipped == 1) {
						clipped_polys.push_back(poly);
						continue;
					}
					
					if (needs_clipped == -1) {
						continue;
					}
					
					auto[clipped_poly, new_edge] = Clip(poly, eq);
					
					clipped_polys.push_back(clipped_poly);
					new_polygon.edges.push_back(new_edge);
				}
				
				clipped_polys.push_back(new_polygon);
				
				brush.polys = clipped_polys;
			}
		}
		
		std::cout << "done!" << std::endl;
	}

	// + --------------------------------------------------------------------- +
	// |                                                                       |
	// |                         HIDDEN SURFACE REMOVAL                        |
	// |                                                                       |
	// + --------------------------------------------------------------------- +
	
	if (!skip_hidden) for (size_t i = 0; i < entities.size(); i++) {
		auto& entity = entities[i];
		
		std::cout << "[" << (i + 1) << "/" << entities.size() << "] Removing hidden surfaces for "<< entity.name <<"... " << std::flush;
		
		std::vector<Brush> new_brushes;
		
		// iterate through all brushes of an entity
		for (auto& brush : entity.brushes) {
			Brush new_brush = {.planes = brush.planes};
			
			// then iterate through all polygons of a brush
			for (auto& poly : brush.polys) {
				
				// compute the plane equation of the polygon
				vec4 eq = PlaneToEquation(poly.plane);
				
				// now find all of the brushes that are adjacent to the polygon
				std::vector<Brush*> adjacent;
				
				for (auto& brush_clip : entity.brushes) {
					if (&brush == &brush_clip) continue;
					
					bool shared_plane = false;
					
					for (auto& poly_clip : brush_clip.polys) {
						bool on_plane = true;
						
						for (auto& edge_clip : poly_clip.edges) {
							if (abs(glm::dot(vec3(eq), edge_clip.p1) + eq.w) > 0.1f ||
								abs(glm::dot(vec3(eq), edge_clip.p2) + eq.w) > 0.1f
							) {
								on_plane = false;
							}
						}
						
						if (on_plane) {
							vec4 poly_eq = PlaneToEquation(poly_clip.plane);
								
							vec3 eq1 = eq;
							vec3 eq2 = poly_eq;
							
							if (glm::dot(eq1, eq2) < 0.0f) {
								shared_plane = true;
							}
							
						}
					}
					
					if (shared_plane) {
						adjacent.push_back(&brush_clip);
					}
				}
				
				std::vector<Polygon> soup = {poly};
				
				for (Brush* brush_clip : adjacent) {
					std::vector<Polygon> new_soup;
					
					for (auto& soup_poly : soup) {
						std::vector<Polygon> speculative_soup;
						Polygon remainder = soup_poly;
						bool clipped = false;
						bool what_the_fuck = false;
						
						for (auto& plane : brush_clip->planes) {
							vec4 plane_eq = PlaneToEquation(plane);

							// this skips planes that have the same plane as polygon
							if (abs(glm::dot(vec3(eq), vec3(plane_eq))) >0.9f) continue;
							
							
							int needs_clipped = NeedsClipped(remainder, plane_eq, 0.1f);
							
							if (needs_clipped == -1) {
								what_the_fuck = true;
								continue;
							}
							
							if (needs_clipped == 1) {
								clipped = true;
								continue;
							}
							
							clipped = true;
							
							auto clipped_off = Clip(remainder, -plane_eq);
							auto new_remainder = Clip(remainder, plane_eq);
							
							speculative_soup.push_back(clipped_off.first);
							remainder = new_remainder.first;
							
						
						}
						
						if (what_the_fuck) {
							new_soup.push_back(soup_poly);
							continue;
						}
						
						if (!clipped) speculative_soup.push_back(remainder);
						
						for (auto& poly : speculative_soup) {
							new_soup.push_back(poly);
						}

					}
					
					soup = new_soup;
				}
				
				for (auto& soup_polygon : soup) {
					new_brush.polys.push_back(soup_polygon);
				}
			}
			
			new_brushes.push_back(new_brush);
		}
		
		entity.brushes = new_brushes;
		
		std::cout << "done!" << std::endl;
	}
	
	// + --------------------------------------------------------------------- +
	// |                                                                       |
	// |                     POLYGON TO TRIANGLE CONVERTER                     |
	// |                                                                       |
	// + --------------------------------------------------------------------- +
	
	// This part here will convert our polygons, which are defined as a set of
	// edges, into little indexed traingles, that can be saved to a disk file.
	
	// First it will find a list of unique materials, then it will load in the
	// dimensions of the textures of these materials, and finally it will 
	// triangulate our polygons, using the texture dimensions to help with
	// texture projection.
	
	for (size_t i = 0; i < entities.size(); i++) {
		auto& entity = entities[i];
		
		std::cout << "[" << (i + 1) << "/" << entities.size() << "] Triangulating polygon soup for "<< entity.name <<"... " << std::flush;
		
		// iterate through all planes and find all of the materials
		for (auto& brush : entity.brushes) {
			for (auto& plane : brush.planes) {
				bool already_in_list = false;
				for (int i = 0 ; i < entity.materials.size(); i++) {
					if (entity.materials[i].name == plane.material) {
						already_in_list = true;
						break;
					}
				}
				
				if (!already_in_list) entity.materials.push_back({.name=plane.material});
			}
		}
		
		// find the parameters of the materials
		for (auto& mat : entity.materials) {
			std::string path = "../../data/textures/";
			path += mat.name;
			path += ".png";
			
			int x, y, n;
			
			if (!stbi_info(path.c_str(), &x, &y, &n)) {
				std::cout << "\nFile " << path << " not found!" << std::endl;
				
				// this is a sane resolution for a texture
				x = 32;
				y = 32;
			}
			
			mat.width = x;
			mat.height = y;
		}
			
		
		auto make_vertex = [](Plane plane, Material mat, vec3 vert, vec3 normal) -> Vertex {
			vec3 pos = vert * (1.0f/32.0f);
			vec2 tex = vec2{vert.x, vert.y} * (1.0f/32.0f);
			
			if (abs(glm::dot(vec3(1.0f, 0.0f, 0.0f), normal))>0.5f) {
				tex = vec2{vert.y * (1.0f/(float)mat.width), vert.z * (1.0f/(float)mat.height)};
			}
			
			if (abs(glm::dot(vec3(0.0f, 1.0f, 0.0f), normal))>0.5f) {
				tex = vec2{vert.x * (1.0f/(float)mat.width), vert.z * (1.0f/(float)mat.height)};
			}
			
			if (abs(glm::dot(vec3(0.0f, 0.0f, 1.0f), normal))>0.5f) {
				tex = vec2{vert.x * (1.0f/(float)mat.width), vert.y * (1.0f/(float)mat.height)};
			}
			
			
			return {
				{pos.x, pos.z, -pos.y},
				{normal.x, normal.z, -normal.y},
				tex,
				tex
			};
		};
		
		for (auto& brush : entity.brushes) {
		for (auto& poly : brush.polys) {
			if (poly.edges.size() < 3) continue;

			uint32_t mat = 0;
			for (int i = 0 ; i < entity.materials.size(); i++) {
				if (entity.materials[i].name == poly.plane.material) mat = i;
			}
		
			vec3 eq = PlaneToEquation(poly.plane);
			vec3 pivot = poly.edges[0].p1;
			
			uint32_t p0 = entity.vertices.size();
			
			entity.vertices.push_back(make_vertex(poly.plane, entity.materials[mat], pivot, eq));
			
			for (auto& edge : poly.edges) {
				if (edge.p1 == pivot || edge.p2 == pivot) {
					continue;
				}
				
				
				uint32_t p1 = entity.vertices.size();
				uint32_t p2 = entity.vertices.size() + 1;
				
				entity.vertices.push_back(make_vertex(poly.plane, entity.materials[mat], edge.p1, eq));
				entity.vertices.push_back(make_vertex(poly.plane, entity.materials[mat], edge.p2, eq));
				
				if (glm::dot(glm::normalize(glm::cross(edge.p1-pivot, edge.p2-pivot)), vec3(eq)) < 0.0f) {
					entity.indices.push_back({p0, p1, p2, mat});
				} else {
					entity.indices.push_back({p0, p2, p1, mat});
				}
			}

			
		}
		}
		
		std::cout << "done!" << std::endl;
	}
	
	// + --------------------------------------------------------------------- +
	// |                                                                       |
	// |                    POLYGON TO CONVEX HULL CONVERTER                   |
	// |                                                                       |
	// + --------------------------------------------------------------------- +
	
	for (auto& entity : entities) {
		for (auto& brush : entity.brushes) {
			for (const auto& poly : brush.polys) {
			for (const auto& edge : poly.edges) {
				bool found_p1 = false;
				bool found_p2 = false;
				for (const auto& point : brush.hull) {
					if (point == edge.p1) found_p1 = true;
					if (point == edge.p2) found_p2 = true;
				}
				if (!found_p1 && !std::isnan(edge.p1.x) && !std::isinf(edge.p1.x)) brush.hull.push_back(edge.p1);
				if (!found_p2 && !std::isnan(edge.p2.x) && !std::isinf(edge.p2.x)) brush.hull.push_back(edge.p2);
			}}
			
			for (auto& point : brush.hull) {
				point = {point.x, point.z, -point.y};
				point *= 1.0f/32.0f;
			}
		}
	}
	
	// +-----------------------------------------------------------------------+
	// +                                                                       +
	// +                             MODEL WRITER                              +
	// +                                                                       +
	// +-----------------------------------------------------------------------+
	
	if (!skip_model) for (size_t i = 0; i < entities.size(); i++) {
		auto& entity = entities[i];
		std::string path = "../../data/models/";
		path += entity.name;
		path += ".stmdl";
		
		File output(path.c_str(), MODE_WRITE);
		
		if (!output.is_open()) {
			std::cout << "Error writing to model file " << path << std::endl;
			continue;
		}

		std::cout << "[" << (i + 1) << "/" << entities.size() << "] Writing " << entity.name << " 3D model to disk..." << std::flush;
		
		output.write_uint32(entity.vertices.size());
		output.write_uint32(entity.indices.size());
		output.write_uint32(entity.materials.size());
		
		output.write_newline();
		
		for (auto& mat : entity.materials) {
			output.write_name(mat.name);
			output.write_newline();
		}
		
		for (auto& vertex : entity.vertices) {
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
		
		for (auto& index : entity.indices) {
			output.write_uint32(index.v1);
			output.write_uint32(index.v2);
			output.write_uint32(index.v3);
			
			output.write_uint32(index.mat);
			
			output.write_newline();
		}
		
		std::cout << "\n";
	}
	
	std::cout << "EEEE" << std::endl;
	
	if (!skip_collision) for (size_t i = 0; i < entities.size(); i++) {
		auto& entity = entities[i];
		std::string path = "../../data/models/";
		path += entity.name;
		path += ".collmdl";
		
		File file(path.c_str(), MODE_WRITE);
		
		if (!file.is_open()) {
			std::cout << "Error writing to model file " << path << std::endl;
			continue;
		}
		
		std::cout << "[" << (i + 1) << "/" << entities.size() << "] Writing " << entity.name << " 3D model to disk..." << std::flush;
		
		for (const auto& brush : entity.brushes) {
			if (!brush.hull.size()) continue;
			
			file.write_name("cloud");
			file.write_uint64(brush.hull.size());
			
			file.write_newline();
			
			for (const auto& point : brush.hull) {
				file.write_float32(point.x);
				file.write_float32(point.y);
				file.write_float32(point.z);
				
				file.write_newline();
			}
		}

		std::cout << "done!" << std::endl;
	}
	
	
	std::cout << "Finished!" << std::endl;
	
	return 0;
}