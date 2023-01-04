// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// MOTIONSTATES.H -- Custom motion states for the bullet physics
//

#ifndef MOTIONSTATES_H
#define MOTIONSTATES_H

#include <btBulletDynamicsCommon.h>

#include <framework/core.h>
#include <framework/entity.h>

#include <components/physicscomponent.h>
#include <components/armaturecomponent.h>

namespace Core::Physics {
    
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

            entity->UpdateTransform(location, rotation);
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
    
}

#endif //MOTIONSTATES_H