// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/particle.h>

#include <framework/async.h>
#include <framework/logging.h>
#include <framework/file.h>

#include <templates/hashmap.h>

using namespace tram;

template <> Pool<Render::Particle> PoolProxy<Render::Particle>::pool("particle pool", 500);

namespace tram::Render {

static Hashmap<Particle*> model_list("particle name list", 500);

Particle* Particle::Find(name_t name) {
    Particle* model = model_list.find(name);
    
    if (!model) {
        model = PoolProxy<Particle>::make(name);
        model_list.insert(name, model);
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
    return {};
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
    if (ct.param1.type == ParamType::DATA) ct.param1.data_lookup = FindValueKey(ct.param1.data, system);
    if (ct.param2.type == ParamType::DATA) ct.param2.data_lookup = FindValueKey(ct.param2.data, system);
    if (ct.param3.type == ParamType::DATA) ct.param3.data_lookup = FindValueKey(ct.param3.data, system);
    if (ct.param4.type == ParamType::DATA) ct.param4.data_lookup = FindValueKey(ct.param4.data, system);
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

static Particle::Data parse_data(File& file) {
    Particle::Data data;
    data.name = file.read_name();
    auto type = file.read_name();
    
    if (type == "scalar") {
        data.type = Particle::DataType::SCALAR;
    } else if (type == "vector") {
        data.type = Particle::DataType::VECTOR;
    } else {
        data.type = Particle::DataType::SCALAR;
        Log(Severity::WARNING, System::RENDER, "Unrecognized .prt DataType: {}", type);
    }
    
    return data;
}

static Particle::Parameter parse_param(File& file) {
    Particle::Parameter param;
    file.skip_linebreak();
    name_t type = file.read_name();
    
    if (type == "none") {
        param.type = Particle::ParamType::NONE;
    } else if (type == "data") {
        param.type = Particle::ParamType::DATA;
    } else if (type == "scalar") {
        param.type = Particle::ParamType::SCALAR;
    } else if (type == "vector") {
        param.type = Particle::ParamType::VECTOR;
    } else {
        param.type = Particle::ParamType::NONE;
        Log(Severity::WARNING, System::RENDER, "Unrecognized .prt ParamType: {}", type);
    }
    
    switch (param.type) {
        case Particle::ParamType::NONE:
            break;
        case Particle::ParamType::DATA:
            param.data = file.read_name();
            break;
        case Particle::ParamType::SCALAR:
            param.scalar = file.read_float32();
            break;
        case Particle::ParamType::VECTOR:
            param.vector = {file.read_float32(), file.read_float32(), file.read_float32()};
            break;
    }
    
    return param;
}

static void parse_system(File& file, Particle::System* sys) {
    while (file.is_continue()) {
        name_t record_type = file.read_name();

        if (record_type == "sprite") {
            sys->SetSprite(Sprite::Find(file.read_name())); file.skip_linebreak(); continue;
        }
        
        if (record_type == "wire") {
            sys->SetWire(Material::Find(file.read_name())); file.skip_linebreak(); continue;
        }
        
        if (record_type == "model") {
            sys->SetModel(Model::Find(file.read_name())); file.skip_linebreak(); continue;
        }
        
        if (record_type == "value") {
            sys->AddValue(parse_data(file)); file.skip_linebreak(); continue;
        }
        
        if (record_type == "operation" || record_type == "initializer") {
            Particle::Operation op;
            name_t optype = file.read_name();
            name_t mergetype = file.read_name();
            name_t mergedest = file.read_name();
            op.target = file.read_name();
            
            op.param1 = parse_param(file);
            op.param2 = parse_param(file);
            op.param3 = parse_param(file);
            op.param4 = parse_param(file);
            
            if (optype == "copy") {
                op.type = Particle::OperationType::COPY;
            } else if (optype == "oscillator") {
                op.type = Particle::OperationType::OSCILLATOR;
            } else if (optype == "noise") {
                op.type = Particle::OperationType::NOISE;
            } else if (optype == "clamp") {
                op.type = Particle::OperationType::CLAMP;
            } else if (optype == "normalize") {
                op.type = Particle::OperationType::NORMALIZE;
            } else {
                op.type = Particle::OperationType::COPY;
                Log(Severity::WARNING, System::RENDER, "Unrecognized .prt OperationType: {}", optype);
            }
            
            if (mergetype == "set") {
                op.merge = Particle::MergeType::SET;
            } else if (mergetype == "add") {
                op.merge = Particle::MergeType::ADD;
            } else if (mergetype == "sub") {
                op.merge = Particle::MergeType::SUBTRACT;
            } else if (mergetype == "mul") {
                op.merge = Particle::MergeType::MULTIPLY;
            } else if (mergetype == "div") {
                op.merge = Particle::MergeType::DIVIDE;
            } else {
                op.merge = Particle::MergeType::SET;
                Log(Severity::WARNING, System::RENDER, "Unrecognized .prt MergeType: {}", mergetype);
            }
            
            if (mergedest == "any") {
                op.dest = Particle::MergeDest::ANY;
            } else if (mergedest == "x") {
                op.dest = Particle::MergeDest::X;
            } else if (mergedest == "y") {
                op.dest = Particle::MergeDest::Y;
            } else if (mergedest == "z") {
                op.dest = Particle::MergeDest::Z;
            } else {
                op.dest = Particle::MergeDest::ANY;
                Log(Severity::WARNING, System::RENDER, "Unrecognized .prt MergeDest: {}", mergedest);
            }
            
            if (record_type == "operation") {
                sys->AddOperation(op);
            }
            
             if (record_type == "initializer") {
                sys->AddInitializer(op);
            }
            
            file.skip_linebreak(); continue;
        }
        
        if (record_type == "constraint") {
            Particle::Constraint ct;
            name_t type = file.read_name();
            name_t dest = file.read_name();
            ct.property = file.read_name();
            
            ct.param1 = parse_param(file);
            ct.param2 = parse_param(file);
            ct.param3 = parse_param(file);
            ct.param4 = parse_param(file);
            
            if (type == "gt") {
                ct.type = Particle::ConstraintType::GREATER_THAN;
            } else if (type == "lt") {
                ct.type = Particle::ConstraintType::LESSER_THAN;
            } else {
                ct.type = Particle::ConstraintType::GREATER_THAN;
                Log(Severity::WARNING, System::RENDER, "Unrecognized .prt ConstraintType: {}", type);
            }
            
            if (dest == "any") {
                ct.dest = Particle::MergeDest::ANY;
            } else if (dest == "x") {
                ct.dest = Particle::MergeDest::X;
            } else if (dest == "y") {
                ct.dest = Particle::MergeDest::Y;
            } else if (dest == "z") {
                ct.dest = Particle::MergeDest::Z;
            } else {
                ct.dest = Particle::MergeDest::ANY;
                Log(Severity::WARNING, System::RENDER, "Unrecognized .prt MergeDest: {}", dest);
            }
            
            sys->AddConstraint(ct);
            
            file.skip_linebreak(); continue;
        }
        
        if (record_type == "emitter") {
            Particle::Parameter rate = parse_param(file);
            Particle::Parameter delay = parse_param(file);
            sys->AddEmitter(rate, delay);
            
            file.skip_linebreak(); continue;
        }
        
        if (record_type == "end") {
            return;
        }
        
        Log(Severity::WARNING, System::RENDER, "Unrecognized .prt record: {}", record_type);
        
        file.skip_linebreak();
    }
}

void Particle::ActuallyLoadFromDisk() {
    std::string filename = std::string("data/particles/") + std::string(name) + ".prt";

    File file(filename.c_str(), File::READ | File::PAUSE_LINE);
    
    bool failed = !file.is_open();
    if (failed) {
        Log(Severity::NOTE, tram::System::RENDER, "Particle not found: {}", filename);
    } else {
        name_t header = file.read_name(); file.skip_linebreak();
        
        if (header != "PRTv1") {
            Log(Severity::WARNING, tram::System::RENDER, "Incorrect particle header \"{}\" in file \"{}\"", header, filename);
            failed = true;
        }
    }
    
    if (failed) {
        // TODO: generate some kind of a blank particle thing
        
        // make sure that base gets created
        (void)GetBaseSystem();
        
        status = LOADED;
        load_fail = true;
        
        return;
    }
    
    while (file.is_continue()) {
        name_t record_type = file.read_name(); file.skip_linebreak();
        
        if (record_type == "control") {
            auto control = parse_data(file);
            AddControl(control.name, control.type);
            
            file.skip_linebreak(); continue;
        }
        
        if (record_type == "base") {
            auto sys = GetBaseSystem();
            parse_system(file, sys);
            
            file.skip_linebreak(); continue;
        }
        
        if (record_type == "system") {
            auto sys = CreateSystem();
            parse_system(file, sys);
            
            file.skip_linebreak(); continue;
        }
        
        Log(Severity::WARNING, tram::System::RENDER, "Unrecognized .prt section: {}", record_type);
        file.skip_linebreak();
    }
}

void Particle::LoadFromDisk() {
    if (status != UNLOADED) {
        Log(Severity::WARNING, tram::System::RENDER, "Particle {} already loaded! Ignoring Particle::LoadFromDisk() call.", name);
        return;
    }
    
    if (!base && !systems.size()) {
        ActuallyLoadFromDisk();
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
            if (emit.rate.type == ParamType::DATA) {
                emit.rate.data_lookup = FindValueKey(emit.rate.data, sys);
            }
            if (emit.delay.type == ParamType::DATA) {
                emit.delay.data_lookup = FindValueKey(emit.delay.data, sys);
            }
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
    if (status != LOADED) {
        Log(Severity::WARNING, tram::System::RENDER, "Particle {} hasn't been loaded! Ignoring Particle::LoadFromMemory() call.", name);
        return;
    }
    
    status = Resource::READY;
}

void Particle::Unload() {
    if (status != READY) {
        Log(Severity::WARNING, tram::System::RENDER, "Particle {} hasn't been loaded! Ignoring Particle::Unload() call.", name);
        return;
    }

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
    
    if (base) {
        delete base;
        base = nullptr;
    }
    
    for (auto sys : systems) {
        delete sys;
    }
    systems.clear();
    
    controls.clear();
    control_offsets.clear();
    
    data_size = 0;
    
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

void Particle::System::AddEmitter(Parameter rate, Parameter delay) {
    emits.push_back({rate, delay});
}

Sprite* Particle::System::GetSprite() const {
    return sprite;
}

Material* Particle::System::GetWire() const {
    return wire;
}

Model* Particle::System::GetModel() const {
    return model;
}

const Particle::Data* Particle::System::GetValue(int index) const {
    if (index >= (int)vals.size()) return nullptr;
    return &vals[index];
}

const Particle::Operation* Particle::System::GetOperation(int index) const {
    if (index >= (int)ops.size()) return nullptr;
    return &ops[index];
}

const Particle::Operation* Particle::System::GetInitializer(int index) const {
    if (index >= (int)inits.size()) return nullptr;
    return &inits[index];
}

const Particle::Constraint* Particle::System::GetConstraint(int index) const {
    if (index >= (int)constrs.size()) return nullptr;
    return &constrs[index];
}

const Particle::Emitter* Particle::System::GetEmitter(int index) const {
    if (index >= (int)emits.size()) return nullptr;
    return &emits[index];
}

Particle::System* Particle::CreateSystem() {
     auto nusys = new Particle::System;
     systems.push_back(nusys);
     return nusys;
}

int Particle::GetSystems(Particle::System** array, int array_size) const {
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

name_t Particle::GetControlName(int index) const {
    if (index >= (int)controls.size()) return name_t();
    return controls[index].name;
}

Particle::DataType Particle::GetControlType(int index) const {
    if (index >= (int)controls.size()) return Particle::DataType {};
    return controls[index].type;
}

int Particle::GetControlOffset(int index) const {
    if (index >= (int)controls.size()) return 0;
    return control_offsets[index];
}

}