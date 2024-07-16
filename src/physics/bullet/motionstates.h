// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_BULLET_MOTIONSTATES_H
#define PHYSICS_BULLET_MOTIONSTATES_H

#include <btBulletDynamicsCommon.h>

#include <framework/core.h>
#include <framework/entity.h>

#include <components/physics.h>
#include <components/animation.h>

namespace tram::Physics {

    
class EntMotionState : public btMotionState {
public:
    EntMotionState (Entity* ent, vec3& offset) {
        entity = ent;
        troffse = offset;
    }

    virtual ~EntMotionState() {
    }

    void SetEntity(Entity* ent) {
        entity = ent;
    }

    virtual void getWorldTransform (btTransform &worldTrans) const {
        vec3 loc;
        quat rot;

        if (entity == nullptr){
            loc = vec3(0.0f, 0.0f, 0.0f);
            rot = quat(vec3(0.0f, 0.0f, 0.0f));
        } else {
            loc = entity->GetLocation();
            rot = entity->GetRotation();
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

    virtual void setWorldTransform (const btTransform &worldTrans) {
        if (entity == nullptr) return;

        vec3 location;
        quat rotation;

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

        entity->UpdateTransform(location, rotation);
    }

protected:
    Entity* entity = nullptr;
    vec3 troffse;
};

class ArmMotionState : public btMotionState {
public:
    ArmMotionState (name_t boneName, AnimationComponent* armature, vec3 bindPos, Entity* entity, PhysicsComponent* physicsComp) {
        ent = entity;
        arm = armature;
        offset = bindPos;
        bone = boneName;
        physcomp = physicsComp;
    }

    virtual ~ArmMotionState() {}

    virtual void getWorldTransform (btTransform &worldTrans) const {
        vec3 loc = ent->GetLocation();
        quat rot = ent->GetRotation();

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

    virtual void setWorldTransform (const btTransform &worldTrans) {
        vec3 location;
        quat rotation;

        btQuaternion rot = worldTrans.getRotation();
        btVector3 loc = worldTrans.getOrigin();

        location.x = loc.getX();
        location.y = loc.getY();
        location.z = loc.getZ();

        rotation.x = rot.getX();
        rotation.y = rot.getY();
        rotation.z = rot.getZ();
        rotation.w = rot.getW();

        vec3 ent_loc = ent->GetLocation();
        quat ent_rot = ent->GetRotation();


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
    AnimationComponent* arm;
    Entity* ent;
    vec3 offset;
    name_t bone;
};

}

#endif // PHYSICS_BULLET_MOTIONSTATES_H