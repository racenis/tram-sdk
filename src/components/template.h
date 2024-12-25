// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_TEMPLATECOMPONENT_H
#define TRAM_SDK_COMPONENTS_TEMPLATECOMPONENT_H

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

#endif // TRAM_SDK_COMPONENTS_TEMPLATECOMPONENT_H