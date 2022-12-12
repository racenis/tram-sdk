// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// COLLISIONMODEL.H -- Collision model resource implementation
// Loaders and stuff for the collision model resource

#include <btBulletDynamicsCommon.h>
#include <physics/physics.h>
#include <fstream>

void Core::Physics::CollisionModel::LoadFromDisk(){
    std::ifstream file;
    std::string type;
    char path[100] = "data/models/";

    btCollisionShape* shape;
    btTransform transf;
    btQuaternion rotat;
    btCompoundShape* cshape = new btCompoundShape();

    strcat(path, name);
    strcat(path, ".collmdl");

    file.open(path);
    if(!file.is_open()){
        std::cout << "Can't find collisionmodel: " << path << std::endl;

        transf.setIdentity();
        shape = new btBoxShape(btVector3(0.125f, 0.125f, 0.125f));

        cshape->addChildShape(transf, shape);
    } else {
        std::cout << "Loading: " << path << std::endl;
        file >> type;
        while(file){
            if (type == "box"){
                float coords[3];
                float rotation[3];
                float dimensions[3];
                file >> coords[0];
                file >> coords[1];
                file >> coords[2];
                file >> rotation[0];
                file >> rotation[1];
                file >> rotation[2];
                file >> dimensions[0];
                file >> dimensions[1];
                file >> dimensions[2];

                transf.setIdentity();
                transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
                rotat.setEuler(rotation[1], rotation[0], rotation[2]);
                transf.setRotation(rotat);
                shape = new btBoxShape(btVector3(dimensions[0], dimensions[1], dimensions[2]));

                cshape->addChildShape(transf, shape);
            } else if (type == "sphere"){
                float coords[3];
                float rotation[3];
                float thickness;
                file >> coords[0];
                file >> coords[1];
                file >> coords[2];
                file >> rotation[0];
                file >> rotation[1];
                file >> rotation[2];
                file >> thickness;

                transf.setIdentity();
                transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
                rotat.setEuler(rotation[1], rotation[0], rotation[2]);
                transf.setRotation(rotat);
                shape = new btSphereShape(thickness);

                cshape->addChildShape(transf, shape);
            } else if (type == "cylinder"){
                float coords[3];
                float rotation[3];
                float dimensions[3];
                file >> coords[0];
                file >> coords[1];
                file >> coords[2];
                file >> rotation[0];
                file >> rotation[1];
                file >> rotation[2];
                file >> dimensions[0];
                file >> dimensions[1];
                file >> dimensions[2];

                transf.setIdentity();
                transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
                rotat.setEuler(rotation[1], rotation[0], rotation[2]);
                transf.setRotation(rotat);
                shape = new btCylinderShape(btVector3(dimensions[0], dimensions[1], dimensions[2]));

                cshape->addChildShape(transf, shape);
            } else if (type == "capsule"){
                float coords[3];
                float rotation[3];
                float thickness;
                float height;
                file >> coords[0];
                file >> coords[1];
                file >> coords[2];
                file >> rotation[0];
                file >> rotation[1];
                file >> rotation[2];
                file >> thickness;
                file >> height;

                transf.setIdentity();
                transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
                rotat.setEuler(rotation[1], rotation[0], rotation[2]);
                transf.setRotation(rotat);
                shape = new btCapsuleShape(thickness, height);

                cshape->addChildShape(transf, shape);
            } else if (type == "cone"){
                float coords[3];
                float rotation[3];
                float thickness;
                float height;
                file >> coords[0];
                file >> coords[1];
                file >> coords[2];
                file >> rotation[0];
                file >> rotation[1];
                file >> rotation[2];
                file >> thickness;
                file >> height;

                transf.setIdentity();
                transf.setOrigin(btVector3(coords[0], coords[1], coords[2]));
                rotat.setEuler(rotation[1], rotation[0], rotation[2]);
                transf.setRotation(rotat);
                shape = new btConeShape(thickness, height);

                cshape->addChildShape(transf, shape);
            } else if (type == "cloud"){
                float coords[3];
                size_t verts;
                file >> verts;

                btConvexHullShape* convexhull  = new btConvexHullShape();
                btVector3 v;

                for (size_t i = 0; i < verts; i++){
                    file >> coords[0];
                    file >> coords[1];
                    file >> coords[2];
                    v.setValue(coords[0], coords[1], coords[2]);
                    convexhull->addPoint(v);
                }

                transf.setIdentity();
                cshape->addChildShape(transf, convexhull);
            } else if (type == "mesh"){
                float coords[3];
                size_t verts;
                file >> verts;

                btTriangleMesh* trimesh = new btTriangleMesh();

                // TODO: allocate
                // allocate memory for the meshy mesh??? like here??
                // trimesh->preallocateVertices(verts);

                btVector3 v[3];

                for (size_t i = 0; i < verts; i++){
                    file >> coords[0];
                    file >> coords[1];
                    file >> coords[2];
                    v[0].setValue(coords[0], coords[1], coords[2]);
                    file >> coords[0];
                    file >> coords[1];
                    file >> coords[2];
                    v[1].setValue(coords[0], coords[1], coords[2]);
                    file >> coords[0];
                    file >> coords[1];
                    file >> coords[2];
                    v[2].setValue(coords[0], coords[1], coords[2]);

                    trimesh->addTriangle(v[0], v[1], v[2]);
                }

                btBvhTriangleMeshShape* mshape = new btBvhTriangleMeshShape(trimesh, true, true);

                transf.setIdentity();
                cshape->addChildShape(transf, mshape);
            }

            file >> type;
        }
    }

    model = cshape;
    status = READY;
}