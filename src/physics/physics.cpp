// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/physics.h>

#include <physics/actions.h>
#include <physics/motionstates.h>

#include <components/triggercomponent.h>

#include <framework/gui.h>
#include <framework/system.h>

namespace Core::Physics {
    std::unordered_map<uint64_t, CollisionModel*> CollisionModel::List;

    btBroadphaseInterface* broadphaseInterface;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* collisionDispatcher;
    btSequentialImpulseConstraintSolver* constraintSolver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    btIDebugDraw* debugDrawer;
    btVehicleRaycaster* vehicleRaycaster;
    
    bool DRAW_PHYSICS_DEBUG = false;
    
    enum UserIndex : int {
        USERINDEX_PHYSICSCOMPONENT = 1,
        USERINDEX_TRIGGERCOMPONENT = 2,
    };

    void StepPhysics(float time){
        dynamicsWorld->stepSimulation(time, 1);
        if (DRAW_PHYSICS_DEBUG) dynamicsWorld->debugDrawWorld();
        
        
        // process the triggers
        int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = contactManifold->getBody0();
            const btCollisionObject* obB = contactManifold->getBody1();
            
            if (obA->getUserIndex() == USERINDEX_TRIGGERCOMPONENT &&
                obB->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
                contactManifold->getNumContacts()) {
                assert(obA->getUserPointer());
                assert(obB->getUserPointer());
                ((TriggerComponent*)obA->getUserPointer())->Collision((PhysicsComponent*)obB->getUserPointer());
            }
            
            if (obA->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
                obB->getUserIndex() == USERINDEX_TRIGGERCOMPONENT &&
                contactManifold->getNumContacts()) {
                assert(obA->getUserPointer());
                assert(obB->getUserPointer());
                ((TriggerComponent*)obB->getUserPointer())->Collision((PhysicsComponent*)obA->getUserPointer());
            }
        }
        
        for (auto& trigger : PoolProxy<TriggerComponent>::GetPool()) trigger.ResetCollisions();
    }

    /// Debug drawer for the physics library.
    class PhysicsDebugDraw : public btIDebugDraw{
    public:

        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
            drawLine(PointOnB, normalOnB, color);
        };

        void reportErrorWarning(const char* warningString) {std::cout << warningString << std::endl;};

        void draw3dText(const btVector3& location, const char* textString) {
            Core::GUI::DebugText(textString, glm::vec3(location.getX(), location.getY(), location.getZ()), Render::COLOR_WHITE);
        };

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
        btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -9.0f);
        btDefaultMotionState* planeMotionState = new btDefaultMotionState(trans);
        btRigidBody::btRigidBodyConstructionInfo planeConstructionInfo(0.0f, planeMotionState, planeShape, btVector3(0.0f, 0.0f, 0.0f));
        btRigidBody* planeRigidBody = new btRigidBody(planeConstructionInfo);

        dynamicsWorld->addRigidBody(planeRigidBody);
        
        System::SetInitialized(System::SYSTEM_PHYSICS, true);
    }

    

    void PathAction::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep){
        //physcomp->UpdatePath();
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



    CollisionShape CollisionShape::Sphere (float radius) {
        return {SHAPE_SPHERE, vec3(radius, 0.0f, 0.0f)};
    }
    
    CollisionShape CollisionShape::Cylinder (float radius, float height) {
        return {SHAPE_CYLINDER, vec3(radius, height, radius)};
    }
    
    CollisionShape CollisionShape::Capsule (float radius, float height) {
        return {SHAPE_CAPSULE, vec3(radius, height, 0.0f)};
    }
    
    CollisionShape CollisionShape::Box (vec3 dimensions) {
        return {SHAPE_BOX, dimensions};
    }

}

namespace Core {
    using namespace Core::Physics;

    template <> Pool<PhysicsComponent> PoolProxy<PhysicsComponent>::pool("physics component pool", 250, false);
    template <> Pool<TriggerComponent> PoolProxy<TriggerComponent>::pool("trigger component pool", 50, false);

    void PhysicsComponent::Start(){
        if (model.get())
            shape = (btCollisionShape*)model->GetPtr();


        //if(bone == 0)
        motionState = new EntMotionState(parent, off_set);
        //else
        //    motionState = new ArmMotionState(bone, armature, off_set, parent, this);

        btScalar bmass = mass;
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        shape->calculateLocalInertia(bmass, inertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(bmass, motionState, shape, inertia);

        rigidBody = new btRigidBody(rigidBodyConstructionInfo);

        dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        rigidBody->setUserPointer(this);
        rigidBody->setUserIndex(USERINDEX_PHYSICSCOMPONENT);

        if(should_asleep) rigidBody->setActivationState(0);

        is_ready = true;

        //return true;
    }
    PhysicsComponent::~PhysicsComponent(){
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
    
    void PhysicsComponent::SetMass(float nMass){mass = nMass;}
    
    void PhysicsComponent::Push (const glm::vec3& direction){
        if(rigidBody == nullptr) return;
        rigidBody->activate();
        rigidBody->applyCentralImpulse(btVector3(direction.x, direction.y, direction.z));
    }
    
    void PhysicsComponent::PushLocal (const glm::vec3& direction){
        if(rigidBody == nullptr) return;

        btTransform trans = rigidBody->getWorldTransform();
        btQuaternion rot_bt = trans.getRotation();
        glm::quat rot(rot_bt.getX(), rot_bt.getY(), rot_bt.getZ(), rot_bt.getW());

        glm::vec3 p_dir = rot * direction;

        rigidBody->activate();
        rigidBody->applyCentralImpulse(btVector3(p_dir.x, p_dir.y, p_dir.z));
    }
    
    void PhysicsComponent::SetCollisionMask(uint32_t flags){
        collisionMask = flags;
        if(is_ready){
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }
    }
    
    void PhysicsComponent::SetCollisionGroup(uint32_t flags){
        collisionGroup = flags;
        if(is_ready){
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }
    }
    

    void PhysicsComponent::SetSleep(bool sleep) {
        if (sleep) {
            if (rigidBody) rigidBody->setActivationState(0);
            should_asleep = true;
        } else {
            if (rigidBody) rigidBody->activate();
            should_asleep = false;
        }
    }
    
    
    void PhysicsComponent::DisableDebugDrawing(){
        rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
  
    void PhysicsComponent::SetLocation (const glm::vec3& location) {
        assert(rigidBody);
        btTransform trans = rigidBody->getWorldTransform();
        trans.setOrigin(btVector3 (location.x, location.y, location.z));
        rigidBody->setWorldTransform(trans);
    }
    
    void PhysicsComponent::SetRotation (const glm::quat& rotation) {
        assert(rigidBody);
        btTransform trans = rigidBody->getWorldTransform();
        trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
        rigidBody->setWorldTransform(trans);
    }
        
    
    void PhysicsComponent::SetLocalRotation(const glm::quat& rotation){
        glm::quat parent_rotation;
        parent->GetRotation(parent_rotation);

        glm::quat final_rotation = parent_rotation * rotation;

        btTransform trans = rigidBody->getWorldTransform();
        trans.setRotation(btQuaternion (final_rotation.x, final_rotation.y, final_rotation.z, final_rotation.w));
        rigidBody->setWorldTransform(trans);
    }    
    
    void PhysicsComponent::EnableRotation () {
        rigidBody->setAngularFactor(1.0f);
    }
    
    void PhysicsComponent::DisableRotation () {
        rigidBody->setAngularFactor(0.0f);
    }
    
    void PhysicsComponent::EnableMovement () {
        rigidBody->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    }
    
    void PhysicsComponent::DisableMovement () {
        rigidBody->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    }

    
    void PhysicsComponent::SetActivation (bool activation) {
        if (activation) {
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        } else {
            rigidBody->setActivationState(WANTS_DEACTIVATION);
        }
    }
        
    void PhysicsComponent::SetVelocity (const vec3& velocity){
        if (velocity.x != 0.0f &&
            velocity.y != 0.0f &&
            velocity.z != 0.0f
        ) {
            rigidBody->activate();
        }
        
        rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }

    vec3 PhysicsComponent::GetVelocity () {
        auto velocity = rigidBody->getLinearVelocity();
        return vec3 (velocity.getX(), velocity.getY(), velocity.getZ());
    }

    
    void TriggerComponent::Start(){
        if (model.get())
            shape = (btCollisionShape*)model->GetPtr();

        assert(shape);

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(location.x, location.y, location.z));
        
        btQuaternion rotation_quat;
        rotation_quat.setX(rotation.x);
        rotation_quat.setY(rotation.y);
        rotation_quat.setZ(rotation.z);
        rotation_quat.setW(rotation.w);
        transform.setRotation(rotation_quat);
        
        trigger = new btCollisionObject();
        //trigger->setCollisionShape(new btBoxShape(btVector3(btScalar(0.99f),btScalar(0.99f),btScalar(0.99f))));
        trigger->setCollisionShape(shape);
        trigger->setWorldTransform(transform);
        trigger->setUserPointer(this);
        trigger->setUserIndex(USERINDEX_TRIGGERCOMPONENT);
        trigger->setCollisionFlags(trigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        
        dynamicsWorld->addCollisionObject(trigger);
        
        
        //dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	
        /*
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
        */
        

        is_ready = true;

        //return true;
    }
    
    TriggerComponent::~TriggerComponent(){
        dynamicsWorld->removeCollisionObject(trigger);
        delete trigger;
    };

    void TriggerComponent::SetCollisionMask(uint32_t flags){
        collisionMask = flags;
        /*if (is_ready) {
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }*/
    }
    
    void TriggerComponent::SetCollisionGroup(uint32_t flags){
        collisionGroup = flags;
        /*if (is_ready) {
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }*/
    }
    
    void TriggerComponent::Collision (PhysicsComponent* other) {
        if (!filter_callback || filter_callback(this, other)) {
            if (!is_collided && !was_collided && activation_callback) activation_callback(this);

            is_collided = true;
        }
    }
    
    void TriggerComponent::ResetCollisions() {
        was_collided = is_collided;
        is_collided = false;
    }
    
    bool TriggerComponent::IsTriggered() {
        return true;
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

















