// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <btBulletDynamicsCommon.h>
#include <physics/physics.h>
#include <physics/collisionmodel.h>
#include <templates/hashmap.h>

#include <framework/file.h>

namespace tram::Physics {

static Hashmap<CollisionModel*> collision_models ("Collision MODEL HASHMAP", 500);

/// Finds a model.
/// Will either return a pointer to a model that already exists with that name,
/// or will create a new one.
/// @return Always will return a pointer to a CollisionModel.
CollisionModel* CollisionModel::Find (name_t model_name) {
    auto model = collision_models.Find (model_name);
    
    if (!model) {
        model = new CollisionModel (model_name);
        collision_models.Insert (model_name, model);
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

    model = new btCompoundShape();

    File file (path, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't find collisionmodel: " << path << std::endl;

        btCollisionShape* shape = new btBoxShape(btVector3(0.125f, 0.125f, 0.125f));

        btTransform transform;
        transform.setIdentity();
        
        model->addChildShape(transform, shape);
        
        load_fail = true;
        status = READY;
        
        return;
    }

    std::cout << "Loading: " << path << std::endl;
    
    while (file.is_continue()) {
        name_t type = file.read_name();
        if (type == "box") {
            float coords[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float rotation[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float dimensions[3] = {file.read_float32(), file.read_float32(), file.read_float32()};

            btTransform transf;
            btQuaternion rotat;

            transf.setIdentity();
            transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
            rotat.setEuler(rotation[1], rotation[0], rotation[2]);
            transf.setRotation(rotat);
            btCollisionShape* shape = new btBoxShape(btVector3(dimensions[0], dimensions[1], dimensions[2]));

            model->addChildShape(transf, shape);
        } else if (type == "sphere"){
            float coords[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float rotation[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float thickness= file.read_float32();

            btTransform transf;
            btQuaternion rotat;

            transf.setIdentity();
            transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
            rotat.setEuler(rotation[1], rotation[0], rotation[2]);
            transf.setRotation(rotat);
            btCollisionShape* shape = new btSphereShape(thickness);

            model->addChildShape(transf, shape);
        } else if (type == "cylinder"){
            float coords[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float rotation[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float dimensions[3] = {file.read_float32(), file.read_float32(), file.read_float32()};

            btTransform transf;
            btQuaternion rotat;

            transf.setIdentity();
            transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
            rotat.setEuler(rotation[1], rotation[0], rotation[2]);
            transf.setRotation(rotat);
            btCollisionShape* shape = new btCylinderShape(btVector3(dimensions[0], dimensions[1], dimensions[2]));

            model->addChildShape(transf, shape);
        } else if (type == "capsule"){
            float coords[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float rotation[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float thickness= file.read_float32();
            float height = file.read_float32();

            btTransform transf;
            btQuaternion rotat;

            transf.setIdentity();
            transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
            rotat.setEuler(rotation[1], rotation[0], rotation[2]);
            transf.setRotation(rotat);
            btCollisionShape* shape = new btCapsuleShape(thickness, height);

            model->addChildShape(transf, shape);
        } else if (type == "cone"){
            float coords[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float rotation[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
            float thickness= file.read_float32();
            float height = file.read_float32();

            btTransform transf;
            btQuaternion rotat;

            transf.setIdentity();
            transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
            rotat.setEuler(rotation[1], rotation[0], rotation[2]);
            transf.setRotation(rotat);
            btCollisionShape* shape = new btConeShape(thickness, height);

            model->addChildShape(transf, shape);
        } else if (type == "cloud") {
            size_t verts = file.read_uint32();
        
            btConvexHullShape* convexhull  = new btConvexHullShape();
            
            btTransform transf;
            btVector3 v;
            
            for (size_t i = 0; i < verts; i++){
                float coords[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
                v.setValue(coords[0], coords[1], coords[2]);
                convexhull->addPoint(v);
            }

            transf.setIdentity();
            model->addChildShape(transf, convexhull);
        } else if (type == "mesh") {
            size_t verts = file.read_uint32();

            btTriangleMesh* trimesh = new btTriangleMesh();

            // TODO: allocate
            // allocate memory for the meshy mesh??? like here??
            // trimesh->preallocateVertices(verts);

            btVector3 v[3];
            btTransform transf;

            for (size_t i = 0; i < verts; i++){
                float coords1[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
                float coords2[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
                float coords3[3] = {file.read_float32(), file.read_float32(), file.read_float32()};
                
                v[0].setValue(coords1[0], coords1[1], coords1[2]);
                v[1].setValue(coords2[0], coords2[1], coords2[2]);
                v[2].setValue(coords3[0], coords3[1], coords3[2]);
                
                trimesh->addTriangle(v[0], v[1], v[2]);
            }

            btBvhTriangleMeshShape* mshape = new btBvhTriangleMeshShape(trimesh, true, true);

            transf.setIdentity();
            model->addChildShape(transf, mshape);
        }
    }

    status = READY;
}

}
