#include <physics/bullet/bullet.h>

#include <btBulletDynamicsCommon.h>

namespace Core::Physics {
    btConvexShape* CollisionShapeToConvexShape (CollisionShape shape) {
        switch (shape.type) {
            case SHAPE_SPHERE:
                return new btSphereShape(shape.dimensions.x);
            case SHAPE_CYLINDER:
                return new btCylinderShape(btVector3(shape.dimensions.x, shape.dimensions.y, shape.dimensions.z));
            case SHAPE_CAPSULE:
                return new btCapsuleShape(shape.dimensions.x, shape.dimensions.y);
            case SHAPE_BOX:
                return new btBoxShape(btVector3(shape.dimensions.x, shape.dimensions.y, shape.dimensions.z));
            default:
                return nullptr;
        }
    }
}