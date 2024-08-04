// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/bullet/debugdrawer.h>

#include <physics/api.h>

#include <components/trigger.h>

#include <btBulletDynamicsCommon.h>

namespace tram::Physics::API {

static btDiscreteDynamicsWorld* dynamics_world = nullptr;

enum UserIndex : int {
    USERINDEX_RIGIDBODY = 3,
    USERINDEX_TRIGGER = 4,
};

class CallbackMotionState : public btMotionState {
public:
    CallbackMotionState(vec3 position, quat rotation, get_trf_callback get_callback, set_trf_callback set_callback, void* data) {
        this->position = position;
        this->rotation = rotation;
        this->get_transform_callback = get_callback;
        this->set_transform_callback = set_callback;
        this->data_pointer = data;
    }
    virtual ~CallbackMotionState() {}

    virtual void getWorldTransform (btTransform& world_transform) const {
        vec3 position = this->position;
        quat rotation = this->rotation;
        
        if (get_transform_callback) {
            std::tie(position, rotation) = get_transform_callback(data_pointer);
        }

        btVector3 transform_translation;
        btQuaternion transform_rotation;
        
        transform_translation.setX(position.x);
        transform_translation.setY(position.y);
        transform_translation.setZ(position.z);
        
        transform_rotation.setX(rotation.x);
        transform_rotation.setY(rotation.y);
        transform_rotation.setZ(rotation.z);
        transform_rotation.setW(rotation.w);
        
        btTransform transform;
        transform.setIdentity();
        transform.setRotation(transform_rotation);
        transform.setOrigin(transform_translation);

        world_transform = transform;
    }

    virtual void setWorldTransform (const btTransform& world_transform) {
        btQuaternion transform_rotation = world_transform.getRotation();
        btVector3 transform_translation = world_transform.getOrigin();

        position.x = transform_translation.getX();
        position.y = transform_translation.getY();
        position.z = transform_translation.getZ();
        
        rotation.x = transform_rotation.getX();
        rotation.y = transform_rotation.getY();
        rotation.z = transform_rotation.getZ();
        rotation.w = transform_rotation.getW();

        if (set_transform_callback) {
            set_transform_callback(data_pointer, {position, rotation});
        }
    }

    void SetTransformCallback(std::pair<vec3, quat>(*get_transform_callback)(void*), void(*set_transform_callback)(void*, std::pair<vec3, quat>), void* data) {
        this->get_transform_callback = get_transform_callback;
        this->set_transform_callback = set_transform_callback;
        this->data_pointer = data;
    }

protected:
    std::pair<vec3, quat>(*get_transform_callback)(void*) = nullptr;
    void(*set_transform_callback)(void*, std::pair<vec3, quat>) = nullptr;
    
    vec3 position;
    quat rotation;
    
    void* data_pointer = nullptr;
};

enum MetadataType {
    METADATA_RIGIDBODY,
    METADATA_TRIGGER,
};

struct RigidbodyMetadata {
    uint32_t collision_mask = -1;
    uint32_t collision_group = -1;
    uint32_t collision_flags = 0;
    
    CallbackMotionState* motion_state = nullptr;
    
    MetadataType type;
    
    col_callback collision_callback = nullptr;
    void* collision_data = nullptr;
};

Pool<RigidbodyMetadata> rigidbody_metadata_pool("rigibody emtadat pool", 200);
    
static btCollisionShape* MakeBulletShape(CollisionShape shape) {
    switch (shape.type) {
        case SHAPE_SPHERE:
            return new btSphereShape(shape.radius);
        case SHAPE_CYLINDER:
            return new btCylinderShape(btVector3(shape.radius_x, shape.height, shape.radius_x));
        case SHAPE_CAPSULE:
            return new btCapsuleShape(shape.radius, shape.height);
        case SHAPE_BOX:
            return new btBoxShape(btVector3(shape.extent_x, shape.extent_y, shape.extent_z));
        case SHAPE_HULL: {
            btConvexHullShape* hull  = new btConvexHullShape();
            for (size_t i = 0; i < shape.hull_size; i++) {
                hull->addPoint(btVector3(shape.hull_points[i].x, shape.hull_points[i].y, shape.hull_points[i].z));
            }
            return hull;
        } break;
        case SHAPE_MESH: {
            btTriangleMesh* mesh = new btTriangleMesh();
            
            for (size_t i = 0; i < shape.mesh_size; i++) {
                btVector3 p0(shape.mesh_triangles[i].p0.x,
                             shape.mesh_triangles[i].p0.y,
                             shape.mesh_triangles[i].p0.z);
                btVector3 p1(shape.mesh_triangles[i].p1.x,
                             shape.mesh_triangles[i].p1.y,
                             shape.mesh_triangles[i].p1.z);
                btVector3 p2(shape.mesh_triangles[i].p2.x,
                             shape.mesh_triangles[i].p2.y,
                             shape.mesh_triangles[i].p2.z);
                
                mesh->addTriangle(p0, p1, p2);
            }

            return new btBvhTriangleMeshShape(mesh, true, true);
        } break;
        default:
            return nullptr;
    }
}

collisionshape_t MakeCollisionShape(CollisionShapeTransform* shapes, size_t shape_count) {
    if (!shapes || shape_count < 1) {
        return {new btBoxShape(btVector3(0.125f, 0.125f, 0.125f))};
    }
    
    btCompoundShape* shape = new btCompoundShape(true);
    
    for (size_t i = 0; i < shape_count; i++) {
        btTransform transform;
        btQuaternion rotation;

        transform.setIdentity();
        transform.setOrigin(btVector3(shapes[i].position.x, shapes[i].position.y, shapes[i].position.z));
        rotation.setX(shapes[i].rotation.x);
        rotation.setY(shapes[i].rotation.y);
        rotation.setZ(shapes[i].rotation.z);
        rotation.setW(shapes[i].rotation.w);
        transform.setRotation(rotation);
        
        shape->addChildShape(transform, MakeBulletShape(shapes[i].shape));
    }
    
    return {shape};
}

collisionshape_t MakeCollisionShape(CollisionShape shape) {
    return {MakeBulletShape(shape)};
}

void YeetCollisionShape(collisionshape_t shape) {
    delete shape.bt_shape;
}


rigidbody_t MakeRigidbody(collisionshape_t shape, float mass, vec3 position, quat rotation, uint32_t mask, uint32_t group, get_trf_callback get_callback, set_trf_callback set_callback, void* data) {
    RigidbodyMetadata* metadata = rigidbody_metadata_pool.AddNew();
    
    if (mass == 0.0f) {
        metadata->collision_flags |= btCollisionObject::CF_STATIC_OBJECT;
    }
    
    metadata->type = METADATA_RIGIDBODY;
    metadata->collision_mask = mask;
    metadata->collision_group = group;
    metadata->motion_state = new CallbackMotionState(position, rotation, get_callback, set_callback, data);
    
    btScalar rigidbody_mass = mass;
    btVector3 rigidbody_inertia (0.0f, 0.0f, 0.0f);
    shape.bt_shape->calculateLocalInertia(rigidbody_mass, rigidbody_inertia);
    
    btRigidBody::btRigidBodyConstructionInfo bullet_construction_info(rigidbody_mass, metadata->motion_state, shape.bt_shape, rigidbody_inertia);
    rigidbody_t rigidbody = {new btRigidBody(bullet_construction_info), metadata};

    std::cout << "collisin mask " << metadata->collision_mask << " grop " << metadata->collision_group << std::endl;

    dynamics_world->addRigidBody(rigidbody.bt_rigidbody, metadata->collision_group, metadata->collision_mask);
    
    rigidbody.bt_rigidbody->setUserIndex(USERINDEX_RIGIDBODY);
    rigidbody.bt_rigidbody->setUserPointer(metadata);
    rigidbody.bt_rigidbody->setCollisionFlags(metadata->collision_flags);
    
    return rigidbody;
}

void YeetRigidbody(rigidbody_t rigidbody) {
    dynamics_world->removeRigidBody(rigidbody.bt_rigidbody);
    delete rigidbody.bt_rigidbody;
    delete rigidbody.bt_metadata->motion_state;
    rigidbody_metadata_pool.Remove(rigidbody.bt_metadata);
}

void SetRigidbodyTransformCallback(rigidbody_t rigidbody, std::pair<vec3, quat>(*get_transform_callback)(void*), void(*set_transform_callback)(void*, std::pair<vec3, quat>), void* data) {
    std::cout << "insignde callback setter" << std::endl;
    rigidbody.bt_metadata->motion_state->SetTransformCallback(get_transform_callback, set_transform_callback, data);
}

void SetRigidbodyCollisionCallback(rigidbody_t rigidbody, col_callback callback, void* data) {
    rigidbody.bt_metadata->collision_callback = callback;
    rigidbody.bt_metadata->collision_data = data;
}

void SetRigidbodyCollisionMask(rigidbody_t rigidbody, uint32_t mask) {
    rigidbody.bt_metadata->collision_mask = mask;
    dynamics_world->removeRigidBody(rigidbody.bt_rigidbody);
    std::cout << "SetRigidbodyCollisionMask mask " << rigidbody.bt_metadata->collision_mask << " grop " << rigidbody.bt_metadata->collision_group << std::endl;
    dynamics_world->addRigidBody(rigidbody.bt_rigidbody, rigidbody.bt_metadata->collision_group, rigidbody.bt_metadata->collision_mask);
}

void SetRigidbodyCollisionGroup(rigidbody_t rigidbody, uint32_t group) {
    rigidbody.bt_metadata->collision_group = group;
    dynamics_world->removeRigidBody(rigidbody.bt_rigidbody);
    std::cout << "SetRigidbodyCollisionGroup mask " << rigidbody.bt_metadata->collision_mask << " grop " << rigidbody.bt_metadata->collision_group << std::endl;
    dynamics_world->addRigidBody(rigidbody.bt_rigidbody, rigidbody.bt_metadata->collision_group, rigidbody.bt_metadata->collision_mask);
}

void SetRigidbodyLocation(rigidbody_t rigidbody, vec3 position) {
    btTransform trans = rigidbody.bt_rigidbody->getWorldTransform();
    trans.setOrigin(btVector3 (position.x, position.y, position.z));
    rigidbody.bt_rigidbody->setWorldTransform(trans);
}

void SetRigidbodyRotation(rigidbody_t rigidbody, quat rotation) {
    btTransform trans = rigidbody.bt_rigidbody->getWorldTransform();
    trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
    rigidbody.bt_rigidbody->setWorldTransform(trans);
}

void SetRigidbodyMass(rigidbody_t rigidbody, float mass) {
    
}

void PushRigidbody(rigidbody_t rigidbody, vec3 direction) {
    rigidbody.bt_rigidbody->activate(); // force awake, sleeping objects won't move
    rigidbody.bt_rigidbody->applyCentralImpulse(btVector3(direction.x, direction.y, direction.z));
}

void PushRigidbody(rigidbody_t rigidbody, vec3 direction, vec3 local) {
    rigidbody.bt_rigidbody->activate();
    rigidbody.bt_rigidbody->applyImpulse(btVector3(direction.x, direction.y, direction.z), btVector3(local.x, local.y, local.z));
}

void SpinRigidbody(rigidbody_t rigidbody, vec3 direction) {
    rigidbody.bt_rigidbody->activate();
    rigidbody.bt_rigidbody->applyTorqueImpulse(btVector3(direction.x, direction.y, direction.z));
}

void SetRigidbodyDebugDrawing(rigidbody_t rigidbody, bool drawing) {
    if (drawing) {
        rigidbody.bt_metadata->collision_flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    } else {
        rigidbody.bt_metadata->collision_flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    }
    
    rigidbody.bt_rigidbody->setCollisionFlags(rigidbody.bt_metadata->collision_flags);
}

void SetRigidbodyKinematic(rigidbody_t rigidbody, bool kinematic) {
    if (kinematic) {
        rigidbody.bt_metadata->collision_flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
    } else {
        rigidbody.bt_metadata->collision_flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
    }
    
    rigidbody.bt_rigidbody->setCollisionFlags(rigidbody.bt_metadata->collision_flags);
}

void SetRigidbodyAngularFactor(rigidbody_t rigidbody, vec3 factor) {
    rigidbody.bt_rigidbody->setAngularFactor({factor.x, factor.y, factor.z});
}

void SetRigidbodyLinearFactor(rigidbody_t rigidbody, vec3 factor) {
    rigidbody.bt_rigidbody->setLinearFactor({factor.x, factor.y, factor.z});
}

void SetRigidbodyVelocity(rigidbody_t rigidbody, vec3 velocity) {
    if (velocity.x != 0.0f &&
        velocity.y != 0.0f &&
        velocity.z != 0.0f
    ) {
        rigidbody.bt_rigidbody->activate();
    }
    
    rigidbody.bt_rigidbody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

vec3 GetRigidbodyVelocity(rigidbody_t rigidbody) {
    auto velocity = rigidbody.bt_rigidbody->getLinearVelocity();
    return {velocity.getX(), velocity.getY(), velocity.getZ()};
}

void AwakenRigidbody(rigidbody_t rigidbody) {
    rigidbody.bt_rigidbody->activate();
}

void SleepRigidbody(rigidbody_t rigidbody) {
    rigidbody.bt_rigidbody->setActivationState(0);
}

void DisableRigidbodyDeactivation(rigidbody_t rigidbody) {
    rigidbody.bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
}

trigger_t MakeTrigger(collisionshape_t shape, uint32_t mask, uint32_t group, vec3 position, quat rotation) {
    RigidbodyMetadata* metadata = rigidbody_metadata_pool.AddNew();
   
    metadata->type = METADATA_RIGIDBODY;
    metadata->collision_mask = mask;
    metadata->collision_group = group;
    metadata->collision_flags = 0;
    metadata->motion_state = nullptr;
 
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    
    btQuaternion rotation_quat;
    rotation_quat.setX(rotation.x);
    rotation_quat.setY(rotation.y);
    rotation_quat.setZ(rotation.z);
    rotation_quat.setW(rotation.w);
    transform.setRotation(rotation_quat);
    
    trigger_t trigger = {new btCollisionObject(), metadata};
    
    metadata->collision_flags = trigger.bt_collisionshape->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE;
    
    trigger.bt_collisionshape->setCollisionShape(shape.bt_shape);
    trigger.bt_collisionshape->setWorldTransform(transform);
    trigger.bt_collisionshape->setUserPointer(metadata);
    trigger.bt_collisionshape->setUserIndex(USERINDEX_TRIGGER);
    trigger.bt_collisionshape->setCollisionFlags(metadata->collision_flags);
    
    dynamics_world->addCollisionObject(trigger.bt_collisionshape, metadata->collision_group, metadata->collision_mask);
    
    return trigger;
}

void YeetTrigger(trigger_t trigger) {
    dynamics_world->removeCollisionObject(trigger.bt_collisionshape);
    delete trigger.bt_collisionshape;
}

void SetTriggerCollisionCallback(trigger_t trigger, col_callback callback, void* data) {
    trigger.bt_metadata->collision_callback = callback;
    trigger.bt_metadata->collision_data = data;
}

void SetTriggerCollisionMask(trigger_t trigger, uint32_t mask) {
    trigger.bt_metadata->collision_mask = mask;
    dynamics_world->removeCollisionObject(trigger.bt_collisionshape);
    dynamics_world->addCollisionObject(trigger.bt_collisionshape, trigger.bt_metadata->collision_group, trigger.bt_metadata->collision_mask);
}

void SetTriggerCollisionGroup(trigger_t trigger, uint32_t group) {
    trigger.bt_metadata->collision_group = group;
    dynamics_world->removeCollisionObject(trigger.bt_collisionshape);
    dynamics_world->addCollisionObject(trigger.bt_collisionshape, trigger.bt_metadata->collision_group, trigger.bt_metadata->collision_mask);
}

void SetTriggerLocation(trigger_t trigger, vec3 location) {
    btTransform trans = trigger.bt_collisionshape->getWorldTransform();
    trans.setOrigin(btVector3 (location.x, location.y, location.z));
    trigger.bt_collisionshape->setWorldTransform(trans);
}

void SetTriggerRotation(trigger_t trigger, quat rotation) {
    btTransform trans = trigger.bt_collisionshape->getWorldTransform();
    trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
    trigger.bt_collisionshape->setWorldTransform(trans);
}

std::pair<ObjectCollision, void*> Raycast(vec3 from, vec3 to, uint32_t collision_mask) {
    btVector3 bto, bfrom;

    bto.setValue(to.x, to.y, to.z);
    bfrom.setValue(from.x, from.y, from.z);

    btCollisionWorld::ClosestRayResultCallback callback (bfrom, bto);

    callback.m_collisionFilterMask = collision_mask;
    
    dynamics_world->rayTest(bfrom, bto, callback);

    if (callback.hasHit() && callback.m_collisionObject->getUserIndex() == USERINDEX_RIGIDBODY) {
        auto& point = callback.m_hitPointWorld;
        auto& normal = callback.m_hitNormalWorld;
        
        RigidbodyMetadata* metadata = (RigidbodyMetadata*)callback.m_collisionObject->getUserPointer();
        
        return {{vec3(point.getX(), point.getY(), point.getZ()),
                 vec3(normal.getX(), normal.getY(), normal.getZ()),
                 0.0f},
                 metadata->collision_data};
    } else {
        return {{vec3 (0.0f, 0.0f, 0.0f), vec3 (0.0f, 0.0f, 0.0f), 0.0f}, nullptr};
    }
}

struct ShapecastCallback : public btCollisionWorld::ConvexResultCallback {
    ShapecastCallback(std::vector<std::pair<ObjectCollision, void*>>& collisions, uint32_t collision_mask)
        : collisions(collisions), collision_mask(collision_mask) {}
    btScalar addSingleResult (btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace) {
        const btCollisionObject* ob = convexResult.m_hitCollisionObject;
        
        // ignore triggers
        if (ob->getUserIndex() != USERINDEX_RIGIDBODY) {
            return 1;
        }
        
        /*const*/ RigidbodyMetadata* metadata = (RigidbodyMetadata*)ob->getUserPointer();
        
        if (assert(metadata); metadata->collision_group & collision_mask) {
            auto& contact = convexResult.m_hitPointLocal;
            auto& normal = convexResult.m_hitNormalLocal;
            
            collisions.push_back({
                {{contact.getX(), contact.getY(), contact.getZ()},
                {-normal.getX(), normal.getY(), normal.getZ()},
                0.0f}, metadata->collision_data
            });
        }
        
        return 1; // tbh idk what this method is supposed to return
    }
    std::vector<std::pair<ObjectCollision, void*>>& collisions;
    uint32_t collision_mask;
};

std::vector<std::pair<ObjectCollision, void*>> Shapecast(CollisionShape shape, vec3 from, vec3 to, uint32_t collision_mask) {
    auto shape_ptr = API::MakeBulletShape(shape);
    btTransform bto, bfrom;
    std::vector<std::pair<ObjectCollision, void*>> collisions;
    
    ShapecastCallback callback(collisions, collision_mask);

    bto.setIdentity();
    bto.setOrigin({to.x, to.y, to.z});
    bfrom.setIdentity();
    bfrom.setOrigin({from.x, from.y, from.z});

    // that btConvexShape* cast will probably segfault if MakeBulletShape() 
    // didn't return a btConvexShape (of which there is only a mesh)
    dynamics_world->convexSweepTest((btConvexShape*)shape_ptr, bfrom, bto, callback);
    
    delete shape_ptr;
    
    return collisions;
}

void Init() {
    btBroadphaseInterface* broadphase_interface = new btDbvtBroadphase();
    btDefaultCollisionConfiguration* collision_configuration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* collision_dispatcher = new btCollisionDispatcher(collision_configuration);
    btSequentialImpulseConstraintSolver* constraint_solver = new btSequentialImpulseConstraintSolver();

    dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase_interface, constraint_solver, collision_configuration);

    dynamics_world->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    btIDebugDraw* debug_drawer  = new PhysicsDebugDraw;
    dynamics_world->setDebugDrawer(debug_drawer);

    btVehicleRaycaster* vehicle_raycaster = new btDefaultVehicleRaycaster(dynamics_world);
    (void)vehicle_raycaster;
    
    // this will make a plane so that stuff doesn't fall out of the world
    btTransform trans;
    trans.setIdentity();
    btCollisionShape* shape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -9.0f);
    btDefaultMotionState* motionstate = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo constructioninfo (0.0f, motionstate, shape, btVector3(0.0f, 0.0f, 0.0f));
    btRigidBody* rigidbody = new btRigidBody(constructioninfo);

    dynamics_world->addRigidBody(rigidbody);
}

void StepPhysics(){
    dynamics_world->stepSimulation(1.0f/60.0f, 0);
    
    // process the triggers
    // TODO: move this ?? to physics.cpp?
    for (auto& trigger : PoolProxy<TriggerComponent>::GetPool()) trigger.ResetCollisions();
    
    // BulletPhysics API is trash and I hate it
    int numManifolds = dynamics_world->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = dynamics_world->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obj_a = contactManifold->getBody0();
        const btCollisionObject* obj_b = contactManifold->getBody1();
        
        // skip if no contacts
        if (!contactManifold->getNumContacts()) continue;
        
        // might need to reverse normal
        bool swapped = false;
        
        // make sure that obj_a is triggercomponent
        if (obj_a->getUserIndex() == USERINDEX_RIGIDBODY &&
            obj_b->getUserIndex() == USERINDEX_TRIGGER) {
            std::swap(obj_a, obj_b);
            swapped = true;
        }
        
        // if not collision between physicscomponent and trigger, skip
        if (obj_a->getUserIndex() != USERINDEX_TRIGGER ||
            obj_b->getUserIndex() != USERINDEX_RIGIDBODY) {
            continue;
        }
        
        assert(obj_a->getUserPointer());
        assert(obj_b->getUserPointer());
        
        for (int i = 0; i < contactManifold->getNumContacts(); i++) {
            auto& contact = contactManifold->getContactPoint(i);
            auto& posA = contact.getPositionWorldOnA();
            auto& posB = contact.getPositionWorldOnB();
            vec3 point = {posA.getX(), posA.getY(), posA.getZ()};
            vec3 normal = -glm::normalize(point - vec3 {posB.getX(), posB.getY(), posB.getZ()});
            
            // reverse the normal
            if (swapped) normal = -normal;
            
            // this is to avoid accidental division by zero
            if (contact.getDistance() == 0.0f) normal = {0.0f, 1.0f, 0.0f};
            
            //std::cout << "collision! " << point.x << " " << point.z << std::endl;
            
            API::RigidbodyMetadata* metadata_a = (API::RigidbodyMetadata*)obj_a->getUserPointer();
            API::RigidbodyMetadata* metadata_b = (API::RigidbodyMetadata*)obj_b->getUserPointer();
            
            if (metadata_a->collision_callback) {
                metadata_a->collision_callback(metadata_a->collision_data,
                                               metadata_b->collision_data,
                                               {point, normal, contact.getDistance()});
            }
        }
    }
}

void DrawDebug() {
    dynamics_world->debugDrawWorld();
}

}