// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <physics/physics.h>
#include <physics/api.h>
#include <physics/collisionmodel.h>
#include <templates/hashmap.h>

#include <framework/file.h>

#include <config.h>

#include <cstring>

/**
 * @class tram::Physics::CollisionModel physics/collisionmodel.h <physics/collisionmodel.h>
 * 
 * Physics CollisionShape model.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/physics/collisionmodel.html
 */

namespace tram::Physics {

static Hashmap<CollisionModel*> collision_models ("CollisionModel list", RESOURCE_LIMIT_COLLISION_MODEL);

/// Finds a model.
/// Will either return a pointer to a model that already exists with that name,
/// or will create a new one.
/// @return Always will return a pointer to a CollisionModel.
CollisionModel* CollisionModel::Find(name_t model_name) {
    auto model = collision_models.Find(model_name);
    
    if (!model) {
        model = new CollisionModel(model_name);
        collision_models.Insert(model_name, model);
    }
    
    return model;
}

/// Loads the collision model from disk.
/// If the collision model file can't be found, the model will be replaced with
/// a 25cm wide cube.
void CollisionModel::LoadFromDisk () {
    char path[200] = "data/models/";
    
    strcat(path, name);
    strcat(path, ".collmdl");

    File file (path, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't find collisionmodel: " << path << std::endl;

        shape = Physics::API::MakeCollisionShape(nullptr, 0);
        load_fail = true;
        status = READY;
        
        return;
    }

    std::vector<API::CollisionShapeTransform> shapes;

    std::cout << "Loading: " << path << std::endl;
    
    while (file.is_continue()) {
        name_t type = file.read_name();
        if (type == "box") {
            vec3 position = {file.read_float32(), file.read_float32(), file.read_float32()};
            vec3 rotation = {file.read_float32(), file.read_float32(), file.read_float32()};
            vec3 dimensions = {file.read_float32(), file.read_float32(), file.read_float32()};
            
            shapes.push_back({CollisionShape::Box(dimensions), position, rotation});

        } else if (type == "sphere") {
            vec3 position = {file.read_float32(), file.read_float32(), file.read_float32()};
            vec3 rotation = {file.read_float32(), file.read_float32(), file.read_float32()};
            float radius = file.read_float32();

            shapes.push_back({CollisionShape::Sphere(radius), position, rotation});
            
        } else if (type == "cylinder") {
            vec3 position = {file.read_float32(), file.read_float32(), file.read_float32()};
            vec3 rotation = {file.read_float32(), file.read_float32(), file.read_float32()};
            vec3 dimensions = {file.read_float32(), file.read_float32(), file.read_float32()};

            shapes.push_back({CollisionShape::Cylinder(dimensions.x, dimensions.y), position, rotation});
            
        } else if (type == "capsule") {
            vec3 position = {file.read_float32(), file.read_float32(), file.read_float32()};
            vec3 rotation = {file.read_float32(), file.read_float32(), file.read_float32()};
            float radius = file.read_float32();
            float height = file.read_float32();

            shapes.push_back({CollisionShape::Capsule(radius, height), position, rotation});
            
        } else if (type == "cone") {
            vec3 position = {file.read_float32(), file.read_float32(), file.read_float32()};
            vec3 rotation = {file.read_float32(), file.read_float32(), file.read_float32()};
            float radius = file.read_float32();
            float height = file.read_float32();

            shapes.push_back({CollisionShape::Cone(radius, height), position, rotation});
            
        } else if (type == "cloud") {
            size_t point_count = file.read_uint32();
        
            vec3* points = new vec3[point_count];
        
            for (size_t i = 0; i < point_count; i++) {
                points[i] = {file.read_float32(), file.read_float32(), file.read_float32()};
            }
        
            shapes.push_back({CollisionShape::Hull(points, point_count), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f)});
        } else if (type == "mesh") {
            size_t triangle_count = file.read_uint32();

            CollisionTriangle* triangles = new CollisionTriangle[triangle_count];
            
            for (size_t i = 0; i < triangle_count; i++){
                vec3 p0 = {file.read_float32(), file.read_float32(), file.read_float32()};
                vec3 p1 = {file.read_float32(), file.read_float32(), file.read_float32()};
                vec3 p2 = {file.read_float32(), file.read_float32(), file.read_float32()};
                
                triangles[i] = {p0, p1, p2};
            }

            shapes.push_back({CollisionShape::Mesh(triangles, triangle_count), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f)});

        }
    }

    shape = Physics::API::MakeCollisionShape(&shapes[0], shapes.size());
    status = READY;
}

}
