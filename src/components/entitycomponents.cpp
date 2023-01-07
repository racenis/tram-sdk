// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <components/rendercomponent.h>
#include <components/lightcomponent.h>
#include <components/playercomponent.h>
#include <components/controllercomponent.h>
#include <components/spritecomponent.h>
#include <components/particlecomponent.h>

namespace Core {
    // TOOD: move all of these pool inits into the where the respective components are implemented or something?
    template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool("render component pool", 250, false);
    template <> Pool<LightComponent> PoolProxy<LightComponent>::pool("light component pool", 250, false);

    template <> Pool<PlayerComponent> PoolProxy<PlayerComponent>::pool("player component pool", 5, false);
    template <> Pool<ControllerComponent> PoolProxy<ControllerComponent>::pool("controller component pool", 25, false);

    template <> Pool<SpriteComponent> PoolProxy<SpriteComponent>::pool("sprite component pool", 100, false);
    template <> Pool<ParticleComponent> PoolProxy<ParticleComponent>::pool("particle component pool", 100, false);
}
