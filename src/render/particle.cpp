// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/particle.h>

#include <framework/async.h>
#include <framework/logging.h>

#include <templates/hashmap.h>

using namespace tram;

template <> Pool<Render::Particle> PoolProxy<Render::Particle>::pool("particle pool", 500);

namespace tram::Render {

static Hashmap<Particle*> model_list("particle name list", 500);

Particle* Particle::Find(name_t name) {
    Particle* model = model_list.Find (name);
    
    if (!model) {
        model = PoolProxy<Particle>::New (name);
        model_list.Insert(UID(name), model);
    }
    
    return model;
}

Particle::LookupInfo Particle::FindValueKey(name_t name, System* system) {
    if (system) for (int i = 0; i < (int)system->vals.size(); i++) {
        if (system->vals[i].name != name) continue;
        return {system->vals[i].type, true, system->data_offsets[i]};
    }
    
    for (int i = 0; i < (int)base->vals.size(); i++) {
        if (base->vals[i].name != name) continue;
        return {base->vals[i].type, false, base->data_offsets[i]};
    }
    
    for (int i = 0; i < (int)controls.size(); i++) {
        if (controls[i].name != name) continue;
        return {controls[i].type, false, control_offsets[i]};
    }
    
    Log(Severity::CRITICAL_ERROR, tram::System::RENDER,
        "Key '{}' not found in particle system '{}'!", name, this->name);
    abort();
}

void Particle::FillKeys(Operation& op, System* system) {
    op.target_lookup = FindValueKey(op.target, system);
    if (op.param1.type == ParamType::DATA) op.param1.data_lookup = FindValueKey(op.param1.data, system);
    if (op.param2.type == ParamType::DATA) op.param2.data_lookup = FindValueKey(op.param2.data, system);
    if (op.param3.type == ParamType::DATA) op.param3.data_lookup = FindValueKey(op.param3.data, system);
    if (op.param4.type == ParamType::DATA) op.param4.data_lookup = FindValueKey(op.param4.data, system);
}

void Particle::FillKeys(Constraint& ct, System* system) {
    ct.property_lookup = FindValueKey(ct.property, system);
}

static int size_of_data_type(Particle::DataType type) {
    switch (type) {
        case Particle::DataType::SCALAR: return 1;
        case Particle::DataType::VECTOR: return 3;
    }
    assert(false);
    return -1;
}

static int size_of_data(const Particle::Data& data, int nums) {
    return size_of_data_type(data.type) * nums;
}

void Particle::LoadFromDisk() {
    if (!base && !systems.size()) {
        // TODO: load from disk here
    }
    
    // pack all data values
    for (auto& val : base->vals) {
        base->data_offsets.push_back(data_size);
        data_size += size_of_data(val, 1);
    }
    
    for (auto& ctrl : controls) {
        control_offsets.push_back(data_size);
        data_size += size_of_data(ctrl, 1);
    }
    
    for (auto sys: systems)
    for (auto& val : sys->vals) {
        if (val.name == "position") sys->position_offset = data_size;
        if (val.name == "rotation") sys->rotation_offset = data_size;
        if (val.name == "color") sys->color_offset = data_size;
        if (val.name == "width") sys->width_offset = data_size;
        if (val.name == "height") sys->height_offset = data_size;
        if (val.name == "frame") sys->frame_offset = data_size;
        sys->data_offsets.push_back(data_size);
        data_size += size_of_data(val, sys->particle_limit);
    }
    
    
    // fill in keys
    for (auto& op : base->ops) FillKeys(op, nullptr);
    for (auto& op : base->inits) FillKeys(op, nullptr);
    
    for (auto sys : systems) for (auto& op : sys->ops) FillKeys(op, sys);
    for (auto sys : systems) for (auto& op : sys->inits) FillKeys(op, sys);
    for (auto sys : systems) for (auto& ct : sys->constrs) FillKeys(ct, sys);
    
    for (auto sys : systems) {
        for (auto &emit : sys->emits) {
            emit.rate_lookup = FindValueKey(emit.rate, sys);
            emit.delay_lookup = FindValueKey(emit.delay, sys);
        }
    }
    
    
    for (auto system : systems) {
        if (auto sprite = system->GetSprite(); sprite) {
            sprite->AddReference();
            Async::LoadDependency(sprite);
        }
        
        if (auto wire = system->GetWire(); wire) {
            wire->AddReference();
            Async::LoadDependency(wire);
        }
        
        if (auto model = system->GetModel(); model) {
            model->AddReference();
            Async::LoadDependency(model);
        }
    }
    
    status = Resource::LOADED;
}

void Particle::LoadFromMemory() {
    status = Resource::READY;
}


void Particle::Unload() {
    for (auto system : systems) {
        if (auto sprite = system->GetSprite(); sprite) {
            sprite->RemoveReference();
        }
        
        if (auto wire = system->GetWire(); wire) {
            wire->RemoveReference();
        }
        
        if (auto model = system->GetModel(); model) {
            model->RemoveReference();
        }
    }
    
    status = Resource::UNLOADED;
}

void Particle::System::SetSprite(Sprite* sprite) {
    this->sprite = sprite;
}

void Particle::System::SetWire(Material* material) {
    this->wire = material;
}

void Particle::System::SetModel(Model* model) {
    this->model = model;
}

void Particle::System::AddValue(Data data) {
    vals.push_back(data);
}

void Particle::System::AddOperation(Operation operation) {
    ops.emplace_back(operation);
}

void Particle::System::AddInitializer(Operation operation) {
    inits.emplace_back(operation);
}

void Particle::System::AddConstraint(Constraint constraint) {
    constrs.push_back(constraint);
}

void Particle::System::AddEmitter(name_t rate_property, name_t delay_property) {
    emits.push_back({rate_property, delay_property});
}

Sprite* Particle::System::GetSprite() {
    return sprite;
}

Material* Particle::System::GetWire() {
    return wire;
}

Model* Particle::System::GetModel() {
    return model;
}

const Particle::Data* Particle::System::GetValue(int index) {
    if (index >= (int)vals.size()) return nullptr;
    return &vals[index];
}

const Particle::Operation* Particle::System::GetOperation(int index) {
    if (index >= (int)ops.size()) return nullptr;
    return &ops[index];
}

const Particle::Operation* Particle::System::GetInitializer(int index) {
    if (index >= (int)inits.size()) return nullptr;
    return &inits[index];
}

const Particle::Constraint* Particle::System::GetConstraint(int index) {
    if (index >= (int)constrs.size()) return nullptr;
    return &constrs[index];
}

const Particle::Emitter* Particle::System::GetEmitter(int index) {
    if (index >= (int)emits.size()) return nullptr;
    return &emits[index];
}

Particle::System* Particle::CreateSystem() {
     auto nusys = new Particle::System;
     systems.push_back(nusys);
     return nusys;
}

int Particle::GetSystems(Particle::System** array, int array_size) {
    int retrieve = array_size > (int)systems.size() ? (int)systems.size() : array_size;
    for (int i = 0; i < retrieve; i++) array[i] = systems[i];
    return retrieve;
}

Particle::System* Particle::GetBaseSystem() {
    if (!base) base = new Particle::System;
    return base;
}

void Particle::AddControl(name_t name, DataType type) {
    controls.push_back({name, type});
}

name_t Particle::GetControlName(int index) {
    if (index >= (int)controls.size()) return name_t();
    return controls[index].name;
}

Particle::DataType Particle::GetControlType(int index) {
    if (index >= (int)controls.size()) return Particle::DataType {};
    return controls[index].type;
}

int Particle::GetControlOffset(int index) {
    if (index >= (int)controls.size()) return 0;
    return control_offsets[index];
}


}