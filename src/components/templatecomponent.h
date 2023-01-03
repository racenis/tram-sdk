// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// TEMPLATECOMPONENT.H -- A template
// This file is a template for other source files

#ifndef TEMPLATECOMPONENT_H
#define TEMPLATECOMPONENT_H

#include <framework/core.h>

namespace Core {
    
    class TemplateComponent : public EntityComponent {
    public:
        TemplateComponent();
        ~TemplateComponent();
        void Init();
        void Start();
        void EventHandler(Event &event);
    };
    
}

#endif //TEMPLATECOMPONENT_H