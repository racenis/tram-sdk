// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/light.h>

#include <render/material.h>

#include <framework/file.h>
#include <framework/logging.h>

#include <templates/hashmap.h>

#include <config.h>






#include <render/render.h>

/**
 * @class tram::Render::LightGraph render/light.h <render/light.h>
 * 
 * LightGraph resource.
 */

using namespace tram;
using namespace tram::Render;

static Hashmap<LightGraph*> lightgraph_list("LightGraph name list", RESOURCE_LIMIT_LIGHT_GRAPH);

template <> Pool<Render::LightGraph> PoolProxy<Render::LightGraph>::pool("LightGraph pool", RESOURCE_LIMIT_LIGHT_GRAPH);

// TODO: add an octree here with all of the nodes!!

bool LightGraph::ContainsEntity(id_t entity) {
    for (auto& graph : PoolProxy<Render::LightGraph>::GetPool()) {
        for (auto graph_entity : graph.entities) {
            if (graph_entity == entity) return true;
        }
    }
    return false;
}

std::vector<std::pair<uint32_t, vec3>> LightGraph::GetEnvironmentProbes() {
    std::vector<std::pair<uint32_t, vec3>> probes;
    for (uint32_t i = 0; i < nodes.size(); i++) if (nodes[i].has_reflection) probes.push_back({i, nodes[i].position});
    return probes;
}

Material* LightGraph::LookupEnvironmentMap(vec3 position) {
    Material* nearest = nullptr;
    float nearest_dist = INFINITY;
    for (auto& graph : PoolProxy<Render::LightGraph>::GetPool()) {
        for (auto& node : graph.nodes) {
            
            if (!node.environment_map) continue;
            
            if (float this_dist = glm::distance(position, node.position); this_dist < nearest_dist) {
                nearest = node.environment_map;
                nearest_dist = this_dist;
            } 
            
        }
    }
    
    return nearest;
}

SphericalHarmonic LightGraph::LookupHarmonic(vec3 position, uint32_t layers) {
    uint32_t nearest = 0;
    LightGraph* nearest_parent = nullptr;
    float nearest_dist = INFINITY;
    
    for (auto& graph : PoolProxy<Render::LightGraph>::GetPool()) {
        for (uint32_t node =0; node < graph.nodes.size(); node++) {
            if (!graph.nodes[node].has_light || !graph.nodes[node].constants.size()) continue;
            
            const float dist = glm::distance(position, graph.nodes[node].position);
            if (dist < nearest_dist) {
                nearest_dist = dist;
                nearest = node;
                nearest_parent = &graph;
            }
            
            //vec3 cco = graph.nodes[node].constants[0].l00;
            //vec3 cco = COLOR_WHITE;
            /*Render::AddText(graph.nodes[node].position, (std::to_string(graph.nodes[node].constants[0].l00.x) + "\n"
                                                       + std::to_string(graph.nodes[node].constants[0].l00.y) + "\n"
                                                       + std::to_string(graph.nodes[node].constants[0].l00.z)).c_str(), cco);*/
        }
    }
    
    SphericalHarmonic harmonic = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    std::vector<std::pair<Node*, float>> connected;
    
    if (!nearest_parent) return harmonic;
    
    connected.push_back({&nearest_parent->nodes[nearest], 0.0f});
    //Render::AddLineMarker(nearest_parent->nodes[nearest].position, Render::COLOR_CYAN);
    
    //return nearest_parent->nodes[nearest].constants[0];
    
    for (auto& edge : nearest_parent->edges) {
        if (edge.first == nearest) {
            connected.push_back({&nearest_parent->nodes[edge.second], 0.0f});
        } else if (edge.second == nearest) {
            connected.push_back({&nearest_parent->nodes[edge.first], 0.0f});
        }
    }
    
    float total_distance = 0.0f;
    
    for (auto& pair : connected) {
        const float dist = 1.0f / glm::distance(pair.first->position, position);
        pair.second = dist;
        total_distance += dist;
        
        //Render::AddLine(nearest_parent->nodes[nearest].position, pair.first->position, Render::COLOR_GREEN);
    }
    
    for (auto& pair : connected) {
        // TODO: implement layer interpolation!!
        
        
        
        SphericalHarmonic& this_harmonic = pair.first->constants[0];
        
        const float weight = pair.second / total_distance;
        
        //Render::AddText(pair.first->position + vec3(0, 1, 0), (std::to_string(weight).c_str()));
        
        harmonic.l00 += weight * this_harmonic.l00;
        
        harmonic.l1m1 += weight * this_harmonic.l1m1;
        harmonic.l10 += weight * this_harmonic.l10;
        harmonic.l11 += weight * this_harmonic.l11;
        
        harmonic.l2m2 += weight * this_harmonic.l2m2;
        harmonic.l2m1 += weight * this_harmonic.l2m1;
        harmonic.l20 += weight * this_harmonic.l20;
        harmonic.l21 += weight * this_harmonic.l21;
        harmonic.l22 += weight * this_harmonic.l22;
    }
    
    return harmonic;
}

LightGraph* LightGraph::Find(name_t name) {
    auto graph = lightgraph_list.Find(name);
    
    if (!graph) {
        graph = PoolProxy<LightGraph>::New(name);
        lightgraph_list.Insert(name, graph);
    }

    return graph;
}

void LightGraph::LoadFromDisk() {
    std::string filename = std::string("data/worldcells/") + std::string(name) + ".light";
    
    File file (filename.c_str(), File::READ);
    
    if (!file.is_open()) {
        Log("Light graph not found: {}", filename);
        return;
    }
    
    name_t header = file.read_name();
    
    if (header != "LIGHTGRAPHv1") {
        Log("Light unrecognized header '{}' in file: {}", header, filename);
        return;
    }
    
    while (file.is_continue()) {
        name_t record_type = file.read_name();
        
        if (record_type == "node") {
            Node new_node;
            
            new_node.position = {file.read_float32(),
                                 file.read_float32(),
                                 file.read_float32()};
            
            new_node.has_light = file.read_uint32();
            new_node.has_reflection = file.read_uint32();
            
            nodes.push_back(new_node);
            
        } else if (record_type == "edge") {
            const uint32_t from_node_index = file.read_uint32();
            const uint32_t to_node_index = file.read_uint32();
            
            edges.push_back({from_node_index, to_node_index});
            
        } else if (record_type == "light") {
            uint32_t index = file.read_uint32();
            uint32_t layer = file.read_uint32();
            name_t channel = file.read_name();
            
            nodes[index].constants.resize(layer + 1);
            
            auto& c = nodes[index].constants[layer];
            
            if (channel == "r") {
                c.l00.x = file.read_float32();

                c.l1m1.x = file.read_float32();
                c.l10.x = file.read_float32();
                c.l11.x = file.read_float32();

                c.l2m2.x = file.read_float32();
                c.l2m1.x = file.read_float32();
                c.l20.x = file.read_float32();
                c.l21.x = file.read_float32();
                c.l22.x = file.read_float32();
            } else if (channel == "g") {
                c.l00.y = file.read_float32();

                c.l1m1.y = file.read_float32();
                c.l10.y = file.read_float32();
                c.l11.y = file.read_float32();

                c.l2m2.y = file.read_float32();
                c.l2m1.y = file.read_float32();
                c.l20.y = file.read_float32();
                c.l21.y = file.read_float32();
                c.l22.y = file.read_float32();
            } else if (channel == "b") {
                c.l00.z = file.read_float32();

                c.l1m1.z = file.read_float32();
                c.l10.z = file.read_float32();
                c.l11.z = file.read_float32();

                c.l2m2.z = file.read_float32();
                c.l2m1.z = file.read_float32();
                c.l20.z = file.read_float32();
                c.l21.z = file.read_float32();
                c.l22.z = file.read_float32();
            }
        } else if (record_type == "entity") {
            entities.push_back(file.read_uint32());
        } else {
            std::cout << "unknown light graph record: " << record_type << std::endl;
        }
    }
    
    for (uint32_t i = 0; i < nodes.size(); i++) {
        if (!nodes[i].has_reflection) continue;
        
        auto material_name = std::string("environment/") + (const char*)GetName() + "." + std::to_string(i);
        nodes[i].environment_map = Render::Material::Make(material_name, MATERIAL_ENVIRONMENTMAP);
        nodes[i].environment_map->AddReference();
        nodes[i].environment_map->Load();
    }
    
    status = READY;
}

