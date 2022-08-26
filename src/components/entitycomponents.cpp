// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ENTITYCOMPONENTS.CPP -- Random trash related to entity components. All file should be yeeted.

#include <core.h>
#include <components/rendercomponent.h>
#include <components/lightcomponent.h>
#include <components/playercomponent.h>
#include <components/controllercomponent.h>

namespace Core {
    // TOOD: move all of these pool inits into the where the respective components are implemented or something?
    template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool("render component pool", 250, false);
    template <> Pool<LightComponent> PoolProxy<LightComponent>::pool("light component pool", 250, false);

    template <> Pool<PlayerComponent> PoolProxy<PlayerComponent>::pool("player component pool", 5, false);
    template <> Pool<ControllerComponent> PoolProxy<ControllerComponent>::pool("controller component pool", 25, false);



    void Stats::Update(){
        //Stats::renderobjects = Render::renderList.GetSize();
        //Stats::animationcomponents = armatureCompPool.GetSize();
        //Stats::physicscomponents = physicsCompPool.GetSize();
    }






    
    
}
