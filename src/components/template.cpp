// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/template.h>

#include <templates/pool.h>

/**
 * @class tram::TemplateComponent components/template.h <components/template.h>
 * 
 * Template component that can be copied to create new components.
 */

namespace tram {

template <> Pool<TemplateComponent> PoolProxy<TemplateComponent>::pool("TemplateComponent pool", 200);
template <> void Component<TemplateComponent>::init() { ptr = TemplateComponent::Make(); }
template <> void Component<TemplateComponent>::yeet() { TemplateComponent::Yeet(ptr); }

TemplateComponent::TemplateComponent() {
    Log("TemplateComponent created!");
}

TemplateComponent::~TemplateComponent() {
    Log("TemplateComponent yeeted!");
}

void TemplateComponent::Start() {
    Log("TemplateComponent started!");
}

void TemplateComponent::EventHandler(Event &event) {
    Log("TemplateComponent received an event!");
}

/// Creates a new TemplateComponent.
TemplateComponent* TemplateComponent::Make() {
    TemplateComponent* ptr = PoolProxy<TemplateComponent>::GetPool().allocate();
    new(ptr) TemplateComponent();
    return ptr;
}

/// Deletes an TemplateComponent.
void TemplateComponent::Yeet(TemplateComponent* component) {
    component->~TemplateComponent();
    PoolProxy<TemplateComponent>::GetPool().deallocate(component);
}

}