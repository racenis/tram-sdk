// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PHYSICS.CPP

#include <btBulletDynamicsCommon.h>
//#include <glm/glm.hpp>
//#include <glm/gtx/quaternion.hpp>

#include <core.h>

#include <components/armaturecomponent.h>
#include <armature.h>

#include <physics.h>
#include <components/physicscomponent.h>

#include <fstream>

namespace Core::Physics {
std::unordered_map<uint64_t, CollisionModel*> CollisionModel::List;

btBroadphaseInterface* broadphaseInterface;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* collisionDispatcher;
btSequentialImpulseConstraintSolver* constraintSolver;
btDiscreteDynamicsWorld* dynamicsWorld;
btIDebugDraw* debugDrawer;
btVehicleRaycaster* vehicleRaycaster;

CollisionModel* CollisionModel::Find(uint64_t modelName){
    std::unordered_map<uint64_t, CollisionModel*>::iterator ff = List.find(modelName);
    CollisionModel* model;
    if(ff == List.end()){
        model = new CollisionModel(modelName);
        List[modelName] = model;
    } else {
        model = ff->second;
    }
    return model;
}

void StepPhysics(float time){
    Stats::Start(Stats::PHYSICS);
    dynamicsWorld->stepSimulation(time, 1);
    if (DRAW_PHYSICS_DEBUG) dynamicsWorld->debugDrawWorld();
    Stats::Stop(Stats::PHYSICS);
}

class PathAction : public btActionInterface {
public:

    PathAction(PhysicsComponent* physComponent){
        physcomp = physComponent;
    }

	virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep);

	virtual void debugDraw(btIDebugDraw* debugDrawer){}

private:
    PhysicsComponent* physcomp = nullptr;
};

/// Used to interface the physics library with the rest of the system.
class EntMotionState : public btMotionState {
public:
    EntMotionState(Entity* ent, glm::vec3& offset) {
        entity = ent;
        troffse = offset;
    }

    virtual ~EntMotionState() {
    }

    void SetEntity(Entity* ent) {
        entity = ent;
    }

    virtual void getWorldTransform(btTransform &worldTrans) const {
        glm::vec3 loc;
        glm::quat rot;

        if (entity == nullptr){
            loc = glm::vec3(0.0f, 0.0f, 0.0f);
            rot = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
        } else {
            entity->GetLocation(loc);
            entity->GetRotation(rot);
        }

        loc += troffse;

        btVector3 translation;
        translation.setX(loc.x);
        translation.setY(loc.y);
        translation.setZ(loc.z);

        btQuaternion rotation;
        rotation.setX(rot.x);
        rotation.setY(rot.y);
        rotation.setZ(rot.z);
        rotation.setW(rot.w);

        btTransform transf;
        transf.setRotation(rotation);
        transf.setOrigin(translation);

        worldTrans = transf;
    }

    virtual void setWorldTransform(const btTransform &worldTrans) {
        if (entity == nullptr) return;

        glm::vec3 location;
        glm::quat rotation;

        btQuaternion rot = worldTrans.getRotation();
        btVector3 loc = worldTrans.getOrigin();

        location.x = loc.getX();
        location.y = loc.getY();
        location.z = loc.getZ();

        location -= troffse;

        rotation.x = rot.getX();
        rotation.y = rot.getY();
        rotation.z = rot.getZ();
        rotation.w = rot.getW();

        entity->SetTransform(location, rotation);
    }

protected:
    Entity* entity = nullptr;
    glm::vec3 troffse;
};

/// Used to interface the physics library with the rest of the system.
class ArmMotionState : public btMotionState {
public:
    ArmMotionState(uint64_t boneName, ArmatureComponent* armature, glm::vec3 bindPos, Entity* entity, PhysicsComponent* physicsComp) {
        ent = entity;
        arm = armature;
        offset = bindPos;
        bone = boneName;
        physcomp = physicsComp;
    }

    virtual ~ArmMotionState() {
    }

    virtual void getWorldTransform(btTransform &worldTrans) const {
        glm::vec3 loc;
        glm::quat rot;

        ent->GetLocation(loc);
        ent->GetRotation(rot);

        loc += rot * offset;

        btVector3 translation;
        translation.setX(loc.x);
        translation.setY(loc.y);
        translation.setZ(loc.z);

        btQuaternion rotation;
        rotation.setX(rot.x);
        rotation.setY(rot.y);
        rotation.setZ(rot.z);
        rotation.setW(rot.w);

        btTransform transf;
        transf.setRotation(rotation);
        transf.setOrigin(translation);

        worldTrans = transf;
    }

    virtual void setWorldTransform(const btTransform &worldTrans) {
        glm::vec3 location;
        glm::quat rotation;

        btQuaternion rot = worldTrans.getRotation();
        btVector3 loc = worldTrans.getOrigin();

        location.x = loc.getX();
        location.y = loc.getY();
        location.z = loc.getZ();

        rotation.x = rot.getX();
        rotation.y = rot.getY();
        rotation.z = rot.getZ();
        rotation.w = rot.getW();

        glm::vec3 ent_loc;
        glm::quat ent_rot;

        ent->GetLocation(ent_loc);
        ent->GetRotation(ent_rot);

        location = location - ent_loc;
        location = glm::inverse(ent_rot) * location;
        location = location - offset;

        rotation = rotation * glm::inverse(ent_rot);

        Render::Keyframe kframe;
        kframe.rotation = rotation;
        kframe.location = location;

        arm->SetBoneKeyframe(bone, kframe);
    }

protected:
    PhysicsComponent* physcomp;
    ArmatureComponent* arm;
    Entity* ent;
    glm::vec3 offset;
    uint64_t bone;
};

/// Debug drawer for the physics library.
class PhysicsDebugDraw : public btIDebugDraw{
public:

    // TODO: implement this one
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {};

	void reportErrorWarning(const char* warningString) {std::cout << warningString << std::endl;};

	// TODO: implement this one
	void draw3dText(const btVector3& location, const char* textString) {};

	void setDebugMode(int debugMode) {};

    int getDebugMode() const {
        return DBG_MAX_DEBUG_DRAW_MODE;
    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color){
        glm::vec3 f;
        glm::vec3 t;
        glm::vec3 c;
        f.x = from.getX();
        f.y = from.getY();
        f.z = from.getZ();
        t.x = to.getX();
        t.y = to.getY();
        t.z = to.getZ();
        c.x = color.getX();
        c.y = color.getY();
        c.z = color.getZ();
        Core::Render::AddLine(f, t, c);
    }
};

void InitPhysics(){
    broadphaseInterface = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
    constraintSolver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphaseInterface, constraintSolver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    debugDrawer  = new PhysicsDebugDraw;
    dynamicsWorld->setDebugDrawer(debugDrawer);

    vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);

    btTransform trans;
    trans.setIdentity();
    btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -3.0f);
    btDefaultMotionState* planeMotionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo planeConstructionInfo(0.0f, planeMotionState, planeShape, btVector3(0.0f, 0.0f, 0.0f));
    btRigidBody* planeRigidBody = new btRigidBody(planeConstructionInfo);

    dynamicsWorld->addRigidBody(planeRigidBody);
}

void CollisionModel::LoadFromDisk(){
    std::ifstream file;
    std::string type;
    char path[100] = "data/models/";

    btCollisionShape* shape;
    btTransform transf;
    btQuaternion rotat;
    btCompoundShape* cshape = new btCompoundShape();

    strcat(path, ReverseUID(name));
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

void PathAction::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep){
	    physcomp->UpdatePath();
}

PhysicsComponent* Raycast(const glm::vec3& from, const glm::vec3& to){
    btVector3 bto, bfrom;

    bto.setValue(to.x, to.y, to.z);
    bfrom.setValue(from.x, from.y, from.z);

    btCollisionWorld::ClosestRayResultCallback callback(bfrom, bto);

    dynamicsWorld->rayTest(bfrom, bto, callback);

    if (callback.hasHit())
        return (PhysicsComponent*)callback.m_collisionObject->getUserPointer();
    else
        return nullptr;
}







}

namespace Core {
using namespace Core::Physics;

template <> Pool<PhysicsComponent> PoolProxy<PhysicsComponent>::pool("physics component pool", 250, false);

    void PhysicsComponent::Init(){
        if(resources_waiting == 0) Start();
        //if (!shape) collModel->AddRef();

        //if(!SetUpRigidBody()){
            //AddToReminderQueue(this, (bool (EntityComponent::*)())SetUpRigidBody);
        //}
    };
    void PhysicsComponent::Start(){
        if (model.GetResource())
            shape = (btCollisionShape*)model.GetResource()->GetPtr();


        if(bone == 0)
            motionState = new EntMotionState(parent, off_set);
            else
                motionState = new ArmMotionState(bone, armature, off_set, parent, this);

        btScalar bmass = mass;
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        shape->calculateLocalInertia(bmass, inertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(bmass, motionState, shape, inertia);

        rigidBody = new btRigidBody(rigidBodyConstructionInfo);

        dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        rigidBody->setUserPointer(this);

        if(should_asleep) rigidBody->setActivationState(0);

        is_ready = true;

        //return true;
    }
    void PhysicsComponent::Uninit(){
        if (pathConstraint) dynamicsWorld->removeConstraint(pathConstraint);
        if (glueConstraint) dynamicsWorld->removeConstraint(glueConstraint);
        if (hingeConstraint) dynamicsWorld->removeConstraint(hingeConstraint);

        dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
        delete motionState;
    };


void PhysicsComponent::SetShapeCapsule(float thickness, float length){
        shape = new btCapsuleShape(thickness, length); // this will leak the memory, but it's ok
    };
    void PhysicsComponent::GetAABB(glm::vec3& aabb_min, glm::vec3& aabb_max){
        btVector3 b_min, b_max;
        rigidBody->getAabb(b_min, b_max);

        aabb_min.x = b_min.getX();
        aabb_min.y = b_min.getY();
        aabb_min.z = b_min.getZ();

        aabb_max.x = b_max.getX();
        aabb_max.y = b_max.getY();
        aabb_max.z = b_max.getZ();
    }
    void PhysicsComponent::SetMass(float nMass){mass = nMass;};
    void PhysicsComponent::Push (const glm::vec3& direction){
        if(rigidBody == nullptr) return;
        rigidBody->activate();
        rigidBody->applyCentralImpulse(btVector3(direction.x, direction.y, direction.z));
    };
    void PhysicsComponent::PushLocal (const glm::vec3& direction){
        if(rigidBody == nullptr) return;

        btTransform trans = rigidBody->getWorldTransform();
        btQuaternion rot_bt = trans.getRotation();
        glm::quat rot(rot_bt.getX(), rot_bt.getY(), rot_bt.getZ(), rot_bt.getW());

        glm::vec3 p_dir = rot * direction;

        rigidBody->activate();
        rigidBody->applyCentralImpulse(btVector3(p_dir.x, p_dir.y, p_dir.z));
    };
    void PhysicsComponent::PushRotate(const glm::vec3& direction){
        rigidBody->applyTorqueImpulse(btVector3(direction.x, direction.y, direction.z));
    }
    void PhysicsComponent::SetCollisionMask(uint32_t flags){
        collisionMask = flags;
        if(is_ready){
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }
    };
    void PhysicsComponent::SetCollisionGroup(uint32_t flags){
        collisionGroup = flags;
        if(is_ready){
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }
    };
    void PhysicsComponent::PutToSleep(){
        rigidBody->setActivationState(0);
    }
    void PhysicsComponent::SetStartAsleep(){
        should_asleep = true;
    }
    void PhysicsComponent::DisableDebugDrawing(){
        rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
    void PhysicsComponent::SetHingeConstraint(PhysicsComponent* other_component, glm::vec3 axis, glm::vec3 point_this, glm::vec3 point_other){
        btVector3 b_axis(axis.x, axis.y, axis.z);
        btVector3 b_this(point_this.x, point_this.y, point_this.z);
        btVector3 b_other(point_other.x, point_other.y, point_other.z);

        hingeConstraint = new btHingeConstraint(*other_component->rigidBody, *rigidBody, b_other, b_this, b_axis, b_axis);
        dynamicsWorld->addConstraint(hingeConstraint);
    };
    void PhysicsComponent::UnsetHingeConstraint(){
        dynamicsWorld->removeConstraint(hingeConstraint);
        delete hingeConstraint;
        hingeConstraint = nullptr;
    }
    void PhysicsComponent::SetHingeAxis(const glm::vec3& axis){
        btVector3 aaa(axis.x, axis.y, axis.z);
        hingeConstraint->setAxis(aaa);
    }
    void PhysicsComponent::SetLocalRotation(const glm::quat& rotation){
        glm::quat parent_rotation;
        parent->GetRotation(parent_rotation);

        glm::quat final_rotation = parent_rotation * rotation;

        btTransform trans = rigidBody->getWorldTransform();
        trans.setRotation(btQuaternion (final_rotation.x, final_rotation.y, final_rotation.z, final_rotation.w));
        rigidBody->setWorldTransform(trans);
    }
    void PhysicsComponent::SetOffset(const glm::vec3& offset){
        off_set = offset;
    };
    void PhysicsComponent::EnableRotation(){
        rigidBody->setAngularFactor(1.0f);
    };
    void PhysicsComponent::DisableRotation(){
        rigidBody->setAngularFactor(0.0f);
    };
    void PhysicsComponent::EnableMovement(){
        rigidBody->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    };
    void PhysicsComponent::DisableMovement(){
        rigidBody->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    };
    void PhysicsComponent::SetFriction(float f){
        if(rigidBody != nullptr) rigidBody->setFriction(f);
    };
    void PhysicsComponent::SetDamping(float f){
        if(rigidBody != nullptr) rigidBody->setDamping(f, 0.0f);
    };
    void PhysicsComponent::SetRestitution(float f){
        if(rigidBody != nullptr) rigidBody->setRestitution(f);
    };
    void PhysicsComponent::SetVelocity(glm::vec3& v){
        if(rigidBody == nullptr) return;
        rigidBody->activate();
        rigidBody->setLinearVelocity(btVector3(v.x, v.y, v.z));
    }
    void PhysicsComponent::SetLinearFactor(float f){
        rigidBody->setLinearFactor(btVector3(f, f, f));
    }
    void PhysicsComponent::DisableActivation(){
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
    }
    void PhysicsComponent::EnableActivation(){
        rigidBody->setActivationState(WANTS_DEACTIVATION);
    }
    float PhysicsComponent::GetVelocity(){
        if(rigidBody == nullptr) return 0.0f;
        return rigidBody->getLinearVelocity().length();
    }
    void PhysicsComponent::SetPath(){
        /*
        btTransform trans = rigidBody->getWorldTransform();
        btVector3 location = trans.getOrigin();


        // TODO: change this to a better search

        glm::vec3 current_loc = glm::vec3(location.getX(), location.getY(), location.getZ());
        next_path = pathNodePool.begin();

        for (PathNode* i = pathNodePool.begin(); i < pathNodePool.end(); i++){
            if(glm::distance(i->coords, current_loc) < glm::distance(next_path->coords,  current_loc))
                next_path = i;
        }

        prev_path = next_path->next[0];


        glm::vec3 loc = current_loc;
        ProjectLine(loc, next_path->coords, prev_path->coords);

        loc.y = loc.y + 0.5f;

        UpdatePathRotate(loc);



        action = new PathAction(this);
        dynamicsWorld->addAction(action);
*/

    }
    void PhysicsComponent::UpdatePath(){
        /*
        if(prev_path != nullptr && next_path != nullptr){
            btTransform trans = rigidBody->getWorldTransform();
            btVector3 location = trans.getOrigin();
            glm::vec3 loc = glm::vec3(location.getX(), location.getY(), location.getZ());
            glm::vec3 colorvec = Render::COLOR_CYAN;

            bool re_rot = false;

            if(glm::dot(loc - prev_path->coords, next_path->coords - prev_path->coords) < 0){
                PathNode* path_candidate = nullptr;

                for (size_t i = 0; i < 3; i++){
                    if(!prev_path->next[i] || prev_path->next[i] == next_path) continue;
                    path_candidate = prev_path->next[i];
                    if(prev_path->next[i]->type == path_pref) break;
                }

                if(path_candidate){
                    next_path = prev_path;
                    prev_path = path_candidate;
                    loc = next_path->coords + glm::vec3(0.0f, 0.5f, 0.0f);
                    re_rot = true;
                }


            } else if (glm::dot(loc - next_path->coords, prev_path->coords - next_path->coords) < 0){
                PathNode* path_candidate = nullptr;

                for (size_t i = 0; i < 3; i++){
                    if(!next_path->next[i] || next_path->next[i] == prev_path) continue;
                    path_candidate = next_path->next[i];
                    if(next_path->next[i]->type == path_pref) break;
                }

                if(path_candidate){
                    prev_path = next_path;
                    next_path = path_candidate;
                    loc = prev_path->coords + glm::vec3(0.0f, 0.5f, 0.0f);
                    re_rot = true;
                }
            }

            if(re_rot){
                dynamicsWorld->removeConstraint(pathConstraint);
                delete pathConstraint;

                UpdatePathRotate(loc);
            }


            if(Render::DRAW_PATH_DEBUG){
                Core::Render::AddLine(glm::vec3(location.getX(), location.getY() + 0.25f, location.getZ()), glm::vec3(location.getX(), location.getY(), location.getZ()), colorvec);
                Core::Render::AddLine(glm::vec3(location.getX(), location.getY() + 0.25f, location.getZ()), prev_path->coords, colorvec);
                Core::Render::AddLine(glm::vec3(location.getX(), location.getY() + 0.25f, location.getZ()), next_path->coords, colorvec);

                Core::Render::AddLine(p_start, p_found, Render::COLOR_WHITE);
                Core::Render::AddLineMarker(p_start, Render::COLOR_RED);
                Core::Render::AddLineMarker(p_found, Render::COLOR_GREEN);
            }
        }*/
    }

    void PhysicsComponent::UpdatePathRotate(const glm::vec3& loc){
        /*
        btTransform trans;
        trans = rigidBody->getWorldTransform();

        glm::quat rot;
        glm::quat new_rot;
        QuatLookAt(rot, glm::vec3(0.0f, 0.0f, 1.0f), next_path->coords - prev_path->coords);
        QuatLookAt(new_rot, rot * glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        new_rot = new_rot * rot;

        trans.setRotation(btQuaternion(new_rot.x, new_rot.y, new_rot.z, new_rot.w));
        trans.setOrigin(btVector3(loc.x, loc.y, loc.z));

        rigidBody->setWorldTransform(trans);


        btTransform frameInB;
        frameInB.setIdentity();
        frameInB.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
        frameInB.setRotation(btQuaternion(0.0f, 0.0f, 0.0f));
        pathConstraint = new btGeneric6DofConstraint(*rigidBody, frameInB, false);
        pathConstraint->setAngularLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
        pathConstraint->setAngularUpperLimit(btVector3(0.0f, 0.0f, 0.0f));
        pathConstraint->setLinearLowerLimit(btVector3(0.0f, 0.0f, 1.0f));
        pathConstraint->setLinearUpperLimit(btVector3(0.0f, 0.0f, 0.0f));
        dynamicsWorld->addConstraint(pathConstraint);
         * */
    }

    void PhysicsComponent::Glue(PhysicsComponent* comp){
        btTransform trans1 = comp->rigidBody->getWorldTransform();
        btTransform trans2 = rigidBody->getWorldTransform();
        btVector3 origin1 = (trans1.inverse() * trans2).getOrigin();
        btVector3 origin2 = btVector3(0.0f, 0.0f, 0.0f);

        glueConstraint = new btPoint2PointConstraint(*(comp->rigidBody), *rigidBody, origin1, origin2);

        dynamicsWorld->addConstraint(glueConstraint);
    }

    void PhysicsComponent::Unglue(){
        dynamicsWorld->removeConstraint(glueConstraint);

        delete glueConstraint;
        glueConstraint = nullptr;
    }








    // TODO: OKEY, the next thing will be needed to be brapped out of here to buscomponent.cpp

    #ifdef BENIGONIS
    void BusComponent::EventHandler(Event &event){
        if(event.type == Event::KEYDOWN){
            if(event.subtype & UI::KEY_FORWARD){
                vehicle->applyEngineForce(5000.0f, 2);
                vehicle->applyEngineForce(5000.0f, 3);
            } else if(event.subtype & UI::KEY_BACKWARD){
                vehicle->applyEngineForce(-3000.0f, 2);
                vehicle->applyEngineForce(-3000.0f, 3);
            }

            if(event.subtype & UI::KEY_LEFT){
                vehicle->setSteeringValue(0.5f, 0);
                //vehicle->setSteeringValue(-0.75f, 1);
            } else if(event.subtype & UI::KEY_RIGHT){
                //vehicle->setSteeringValue(0.75f, 0);
                vehicle->setSteeringValue(-0.5f, 1);
            }

            if(event.subtype & UI::KEY_JUMP){
                vehicle->setBrake(1000.0f, 2);
                vehicle->setBrake(1000.0f, 3);
            }
        }


        if(event.type == Event::KEYUP){
            if(event.subtype & UI::KEY_FORWARD || event.subtype & UI::KEY_BACKWARD){
                vehicle->applyEngineForce(0.0f, 2);
                vehicle->applyEngineForce(0.0f, 3);

                vehicle->setBrake(10.0f, 2);
                vehicle->setBrake(10.0f, 3);
            }

            if(event.subtype & UI::KEY_LEFT || event.subtype & UI::KEY_RIGHT){
                vehicle->setSteeringValue(0.0f, 0);
                vehicle->setSteeringValue(0.0f, 1);
            }

            if(event.subtype & UI::KEY_JUMP){
                vehicle->setBrake(10.0f, 2);
                vehicle->setBrake(10.0f, 3);
            }
        }


    };
    bool BusComponent::Start(){
        if(true/*model->GetStatus() == Resource::READY && body_physcomp->IsReady()*/){
            Render::Bone* bone = model->GetArmatureFirstPtr();
            for(size_t i = 0; i < model->GetArmatureSize(); i++){
                if(bone->name == UID("WheelFL")){
                    offset_wheel_fl = bone->head;
                } else if(bone->name == UID("WheelFR")){
                    offset_wheel_fr = bone->head;
                } else if(bone->name == UID("WheelBL")){
                    offset_wheel_bl = bone->head;
                } else if(bone->name == UID("WheelBR")){
                    offset_wheel_br = bone->head;
                }

                bone++;
            }

            //TODO: check if this is safe
            btRaycastVehicle::btVehicleTuning vtuna;

            vehicle = new btRaycastVehicle(vtuna, body_physcomp->rigidBody, vehicleRaycaster);

            dynamicsWorld->addVehicle(vehicle);

            btVector3 wheel_dir(0.0f, -1.0f, 0.0f);
            btVector3 axle(1.0f, 0.0f, 0.0f);

            float v_off = 1.0f; // stupid hack

            float suspension = 0.4f;

            float wheel_radius = 0.3f;


            vehicle->addWheel(btVector3(offset_wheel_fl.x, offset_wheel_fl.y + v_off, offset_wheel_fl.z), wheel_dir, axle, suspension, wheel_radius, vtuna, true);

            vehicle->addWheel(btVector3(offset_wheel_fr.x, offset_wheel_fr.y + v_off, offset_wheel_fr.z), wheel_dir, axle, suspension, wheel_radius, vtuna, true);

            vehicle->addWheel(btVector3(offset_wheel_bl.x, offset_wheel_bl.y + v_off, offset_wheel_bl.z), wheel_dir, axle, suspension, wheel_radius, vtuna, false);

            vehicle->addWheel(btVector3(offset_wheel_br.x, offset_wheel_br.y + v_off, offset_wheel_br.z), wheel_dir, axle, suspension, wheel_radius, vtuna, false);


            body_physcomp->SetLinearFactor(1.0f);

            //std::cout << "action: " << body_physcomp->action << std::endl;

            return true;
        } else {
            return false;
        }
    }
    void BusComponent::Init(){
        model->AddRef();

        if(!FindOffsets())
            AddToReminderQueue(this, (bool (EntityComponent::*)())FindOffsets);

        if(resources_waiting == 0) Start();
    }
    void BusComponent::Uninit(){
        // TODO: better cleanup
        if (vehicle) dynamicsWorld->removeVehicle(vehicle);
    }

void BusComponent::Activate(){
        std::cout << "ACTIVATED!!" << std::endl;

        if(is_controlled){
            Message msg;
            msg.senderID = parent->GetID();
            msg.receiverID = PLAYER_ID;
            msg.type = MESSAGE_GET_OUT;
            SendDIRST(msg);

            body_physcomp->EnableActivation();

            RemoveEventListener(EVENT_KEYPRESS, keypress);
            keypress = nullptr;
            RemoveEventListener(EVENT_KEYDOWN, keydown);
            keydown = nullptr;
            RemoveEventListener(EVENT_KEYUP, keyup);
            keyup = nullptr;




            is_controlled = false;
        } else {
            Message msg;
            msg.senderID = parent->GetID();
            msg.receiverID = PLAYER_ID;
            msg.type = MESSAGE_GET_IN;
            msg.data = body_physcomp;
            SendDIRST(msg);

            body_physcomp->DisableActivation();

            keypress = AddEventListener(EVENT_KEYPRESS);
            keypress->ent = this;
            keydown = AddEventListener(EVENT_KEYDOWN);
            keydown->ent = this;
            keyup = AddEventListener(EVENT_KEYUP);
            keyup->ent = this;


            is_controlled = true;
        }


    }
    #endif // BENIGONIS

}

















