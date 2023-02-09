// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_BULLET_DEBUGDRAWER_H
#define PHYSICS_BULLET_DEBUGDRAWER_H

#include <physics/bullet/bullet.h>
#include <framework/gui.h>

namespace Core::Physics {
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
}

#endif // PHYSICS_BULLET_DEBUGDRAWER_H