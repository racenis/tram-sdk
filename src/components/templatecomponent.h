// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_TEMPLATECOMPONENT_H
#define COMPONENTS_TEMPLATECOMPONENT_H

#include <framework/entitycomponent.h>

namespace tram {

class TemplateComponent : public EntityComponent {
public:
    TemplateComponent();
    ~TemplateComponent();
    void Init();
    void Start();
    void EventHandler(Event &event);
};

}

#endif // COMPONENTS_TEMPLATECOMPONENT_H