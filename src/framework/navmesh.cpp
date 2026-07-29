// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/navmesh.h>

#include <framework/file.h>
#include <render/render.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <config.h>

#include <unordered_map>
#include <cstring>

/**
 * @class tram::Navmesh framework/navmesh.h <framework/navmesh.h>
 * 
 * Not fully implemented yet.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/navmesh.html
 */

namespace tram {

template <> Pool<Navmesh> PoolProxy<Navmesh>::pool("navmesh pool", 100);
template <> Pool<Navplan> PoolProxy<Navplan>::pool("navplan pool", 100);
static Hashmap<Navmesh*> navmesh_list("navmesh list", 200);

Navmesh* Navmesh::Find(name_t name) {
    Navmesh* navmesh = navmesh_list.find(name);
    
    if (!navmesh) {
        navmesh =  PoolProxy<Navmesh>::make(name);
    }
    
    return navmesh;
}


void Navmesh::LoadFromDisk() {
    char path[PATH_LIMIT];
    snprintf(path, PATH_LIMIT, "data/navmeshes/%s.nav", (const char*)name);
    
    File file(path, File::READ | File::PAUSE_LINE);
    
    if (!file.is_open()) {
        Log(Severity::NOTE, System::CORE, "Can't find navmesh file: {}", path);
        return;
    }
    
    if (file.read_name() != "NAVMESHv2") {
        Log(Severity::WARNING, System::CORE, "Unrecognized navmesh format in: {}", path);
        return;
    }
    
    file.skip_linebreak();
    
    while (file.is_continue()) {
        name_t entry_type = file.read_name();
        
        if (entry_type == "node") {
            Node node = {
                .position = {file.read_float32(),
                             file.read_float32(),
                             file.read_float32()}
            };
            
            this->nodes.push_back(node);
        } else if (entry_type == "edge") {
            uint32_t edge_index = this->edges.size();
            
            Edge edge = {file.read_uint32(), file.read_uint32()};
            
            this->nodes[edge.from].edges.push_back(edge_index);
            
            this->edges.push_back(edge);
        }
        
        file.skip_linebreak();
    }
}

void Navmesh::Draw() {
    for (const auto& edge : edges) {
        Render::AddLine(nodes[edge.from].position, nodes[edge.to].position, Render::COLOR_CYAN);
    }
    
    for (const auto& node : nodes) {
        Render::AddLineAABB({-0.1f, -0.1f, -0.1f}, {0.1f, 0.1f, 0.1f}, node.position, {1.0f, 0.0f, 0.0f, 0.0f}, Render::COLOR_CYAN);
    }
}


void Navplan::Reset() {
    uint32_t first = 0;
    uint32_t last = 0;
    
    for (uint32_t i = 0; i < mesh->GetNodes().size(); i++) {
        if (glm::distance(mesh->GetNodes()[first].position, from) > glm::distance(mesh->GetNodes()[i].position, from)) {
            first = i;
        }
        if (glm::distance(mesh->GetNodes()[last].position, to) > glm::distance(mesh->GetNodes()[i].position, to)) {
            last = i;
        }
    }
    
    std::vector<int32_t> backings;
    for (int i = 0; i < (int)mesh->GetNodes().size(); i++) backings.push_back(-1);
    
    std::vector<int32_t> inthis;
    std::vector<int32_t> innext;
    inthis.push_back(first);
    
    while (inthis.size()) {
        for (int32_t node : inthis) {
            for (int32_t edge : mesh->GetNodes()[node].edges) {
                int32_t next = mesh->GetEdges()[edge].to;
                if (backings[next] != -1) continue;
                innext.push_back(next);
                backings[next] = node;
                if (next == (int32_t)last) goto weredonehere;
            }
        }
        
        std::swap(inthis, innext);
        innext.clear();
    }
weredonehere:

    if (backings[last] == -1) {
        points.push_back(mesh->GetNodes()[first].position);
        points.push_back(mesh->GetNodes()[last].position);
        
        return;
    }
    
    for (int32_t egg = last; egg != (int32_t)first; egg = backings[egg]) {
        points.push_back(mesh->GetNodes()[egg].position);
    }
    
    
}

vec3 Navplan::GetNextPoint() {
    if (!points.size()) return {0.0f, 0.0f, 0.0f};
    return points.back();
}

int32_t Navplan::GetRemaining() {
    return points.size();
}

void Navplan::Advance() {
    points.pop_back();
}

void Navplan::Draw() {
    for (int32_t i = 0; i < (int32_t)points.size() - 1; i++) {
        Render::AddLine(points[i] + DIRECTION_UP * 0.05f, points[i+1] + DIRECTION_UP * 0.05f, Render::COLOR_PINK);
    }
    
    for (const auto& p : points) {
        Render::AddLineAABB(vec3(0.2f, 0.2f, 0.2f), -vec3(0.2f, 0.2f, 0.2f), p, vec3(0.0f, 0.0f, 0.0f), Render::COLOR_PINK);
    }
    
    if (points.size()) {
        Render::AddLine(points.back() + DIRECTION_UP * 0.05f, from, Render::COLOR_PINK);
        Render::AddLine(points.front() + DIRECTION_UP * 0.05f, to, Render::COLOR_PINK);
    }
}

void Navplan::Yeet() {
    this->~Navplan();
    PoolProxy<Navplan>::GetPool().deallocate(this);
}

Navplan* Navplan::Make(Navmesh* mesh, vec3 from, vec3 to) {
    Navplan* ptr = PoolProxy<Navplan>::GetPool().allocate();
    new(ptr) Navplan(mesh, from, to);
    return ptr;
}

}