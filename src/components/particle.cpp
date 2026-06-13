// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/api.h>
#include <render/vertices.h>
#include <components/particle.h>
#include <render/material.h>

#include <config.h>

#include <cstring>

/**
 * @class tram::ParticleComponent components/particle.h <components/particle.h>
 * 
 * Provides amusing graphical effects.
 * @see https://racenis.github.io/tram-sdk/documentation/components/particle.html
 */

namespace tram {

using namespace tram::Render;
using namespace tram::Render::API;

template <> Pool<ParticleComponent> PoolProxy<ParticleComponent>::pool("ParticleComponent pool", COMPONENT_LIMIT_PARTICLE);
template <> void Component<ParticleComponent>::init() { ptr = ParticleComponent::Make(); }
template <> void Component<ParticleComponent>::yeet() { ParticleComponent::Yeet(ptr); }

ParticleComponent::ParticleComponent() : particle(this) {
    
}

ParticleComponent::~ParticleComponent() {
    is_ready = true;

    RemoveDrawListEntry(draw_list_entry);
    
    delete[] data;
    
    data = nullptr;
};


void ParticleComponent::PerformOperation(const Render::Particle::Operation* op, int offset, int count) {
    switch (op->type) {
        case Render::Particle::OperationType::COPY:
            if (op->param1.type == Render::Particle::ParamType::SCALAR 
                || (op->param1.type == Render::Particle::ParamType::DATA
                && op->param1.data_lookup.type == Render::Particle::DataType::SCALAR)) {
                for (int i = 0; i < count; i++) {
                    float val = AsScalar(op->param1, i + offset);
                    MergeIn(op->target_lookup, i + offset, op->merge, op->dest, val);
                }
            }
            
            if (op->param1.type == Render::Particle::ParamType::VECTOR 
                || (op->param1.type == Render::Particle::ParamType::DATA
                && op->param1.data_lookup.type == Render::Particle::DataType::VECTOR)) {
                for (int i = 0; i < count; i++) {
                    vec3 val = AsVector(op->param1, i + offset);
                    MergeIn(op->target_lookup, i + offset, op->merge, op->dest, val);
                }
            }
            break;
        case Render::Particle::OperationType::OSCILLATOR: 
            for (int i = 0; i < count; i++) {
                float val = AsScalar(op->param1, i + offset) * cosf(1.0f / AsScalar(op->param2, i + offset) * 2.0f * 3.14f * GetFrameTime() + AsScalar(op->param3, i + offset));
                if (std::isnan(val) || std::isinf(val)) val = 0.0f;
                MergeIn(op->target_lookup, i + offset, op->merge, op->dest, val);
            }
            break;
        case Render::Particle::OperationType::NOISE:
            for (int i = 0; i < count; i++) {
                float val = AsScalar(op->param1, i + offset) + AsScalar(op->param2, i + offset) * ((float)rand() / (float)RAND_MAX);
                MergeIn(op->target_lookup, i + offset, op->merge, op->dest, val);
            }
            break;
        case Render::Particle::OperationType::CLAMP:
            if (op->target_lookup.type == Render::Particle::DataType::SCALAR) {
                for (int i = 0; i < count; i++) {
                    float& val = AsScalar(op->target_lookup, i + offset);
                    val = glm::clamp(val, AsScalar(op->param1, i + offset), AsScalar(op->param2, i + offset));
                }
            }
            
            if (op->target_lookup.type == Render::Particle::DataType::VECTOR) {
                for (int i = 0; i < count; i++) {
                    vec3& val = AsVector(op->target_lookup, i + offset);
                    val = glm::clamp(val, AsScalar(op->param1, i + offset), AsScalar(op->param2, i + offset));
                }
            }
            break;
        case Render::Particle::OperationType::NORMALIZE:
            if (op->target_lookup.type == Render::Particle::DataType::VECTOR) {
                for (int i = 0; i < count; i++) {
                    vec3& val = AsVector(op->target_lookup, i + offset);
                    val = glm::normalize(val);
                }
            }
            break;
    }
}

void ParticleComponent::PerformConstraint(const Render::Particle::Constraint* ct, int system) {
    for (int i = 0; i < (int)systems[system].slots.size(); i++) {
    if (!systems[system].slots[i]) continue;
    switch (ct->type) {
        case Render::Particle::ConstraintType::GREATER_THAN:
            if (AsScalar(ct->property_lookup, i, ct->dest) > AsScalar(ct->param1, i)) {
                systems[system].slots[i] = false;
            }
            break;
        case Render::Particle::ConstraintType::LESSER_THAN:
            if (AsScalar(ct->property_lookup, i, ct->dest) < AsScalar(ct->param1, i)) {
                systems[system].slots[i] = false;
            }
            break;
    }
    }
}

void ParticleComponent::PerformEmit(const Render::Particle::Emitter* em, int system) {
    if (AsScalar(em->delay, system) > systems[system].since_last_emit) {
        return;
    }
    
    int emits = AsScalar(em->rate, system);
    
    if (!emits) return;
    
    for (int i = 0; i < (int)systems[system].slots.size() && emits; i++) {
        if (systems[system].slots[i]) continue;
        systems[system].slots[i] = true;
        emits--;
        
        for (int op = 0; systems[system].system->GetInitializer(op); op++) {
            PerformOperation(systems[system].system->GetInitializer(op), i, 1);
        }
    }
    
    systems[system].since_last_emit = 0;
}

float& ParticleComponent::AsScalar(Render::Particle::LookupInfo info, int index, Render::Particle::MergeDest dest) {
    if (!info.array) index = 0;
    
    if (info.type == Render::Particle::DataType::VECTOR) {
        switch (dest) {
            case Render::Particle::MergeDest::ANY:
            case Render::Particle::MergeDest::X:    return data[info.offset + index * 3 + 0];
            case Render::Particle::MergeDest::Y:    return data[info.offset + index * 3 + 1];
            case Render::Particle::MergeDest::Z:    return data[info.offset + index * 3 + 2];  
        }
    }
    
    return data[info.offset + index];
}

vec3& ParticleComponent::AsVector(Render::Particle::LookupInfo info, int index, Render::Particle::MergeDest dest) {
    if (!info.array) index = 0;
    
    // this shouldn't happen, but we'll indulge nevertheless
    if (info.type == Render::Particle::DataType::SCALAR) {
        static vec3 vector;
        float fval = data[info.offset + index];
        vector = {fval, fval, fval};
        return vector;
    }
    
    return *(vec3*)&data[info.offset + index * 3];
}

float ParticleComponent::AsScalar(const Render::Particle::Parameter& param, int index) {
    switch (param.type) {
        default:
        case Render::Particle::ParamType::NONE:     return 0.0f;
        case Render::Particle::ParamType::DATA:     return AsScalar(param.data_lookup, index);
        case Render::Particle::ParamType::SCALAR:   return param.scalar;
        case Render::Particle::ParamType::VECTOR:   return param.vector.x;
    }
}

vec3 ParticleComponent::AsVector(const Render::Particle::Parameter& param, int index) {
    switch (param.type) {
        default:
        case Render::Particle::ParamType::NONE:     return {0.0, 0.0, 0.0f};
        case Render::Particle::ParamType::DATA:     return AsVector(param.data_lookup, index);
        case Render::Particle::ParamType::SCALAR:   return {param.scalar, param.scalar, param.scalar};
        case Render::Particle::ParamType::VECTOR:   return param.vector;
    }
}

void ParticleComponent::MergeIn(Render::Particle::LookupInfo info, int index, Render::Particle::MergeType type, Render::Particle::MergeDest dest, vec3 value) {
    if (info.type == Render::Particle::DataType::SCALAR) {
        float extracted;
        switch (dest) {
            case Render::Particle::MergeDest::ANY:
            case Render::Particle::MergeDest::X: extracted = value.x; break;
            case Render::Particle::MergeDest::Y: extracted = value.y; break;
            case Render::Particle::MergeDest::Z: extracted = value.z; break;
            break;
        }
        
        switch (type) {
            case Render::Particle::MergeType::SET:
                AsScalar(info, index) = extracted;
                return;
            case Render::Particle::MergeType::ADD:
                AsScalar(info, index) += extracted;
                return;
            case Render::Particle::MergeType::SUBTRACT:
                AsScalar(info, index) -= extracted;
                return;
            case Render::Particle::MergeType::MULTIPLY:
                AsScalar(info, index) *= extracted;
                return;
            case Render::Particle::MergeType::DIVIDE:
                AsScalar(info, index) /= extracted;
                return;
        }
    }
    
    if (info.type == Render::Particle::DataType::VECTOR) {
        switch (type) {
            case Render::Particle::MergeType::SET:
                AsVector(info, index) = value;
                return;
            case Render::Particle::MergeType::ADD:
                AsVector(info, index) += value;
                return;
            case Render::Particle::MergeType::SUBTRACT:
                AsVector(info, index) -= value;
                return;
            case Render::Particle::MergeType::MULTIPLY:
                AsVector(info, index) *= value;
                return;
            case Render::Particle::MergeType::DIVIDE:
                AsVector(info, index) /= value;
                return;
        }
    }
}

void ParticleComponent::MergeIn(Render::Particle::LookupInfo info, int index, Render::Particle::MergeType type, Render::Particle::MergeDest dest, float value) {
    if (info.type == Render::Particle::DataType::SCALAR) {
        switch (type) {
            case Render::Particle::MergeType::SET:
                AsScalar(info, index) = value;
                return;
            case Render::Particle::MergeType::ADD:
                AsScalar(info, index) += value;
                return;
            case Render::Particle::MergeType::SUBTRACT:
                AsScalar(info, index) -= value;
                return;
            case Render::Particle::MergeType::MULTIPLY:
                AsScalar(info, index) *= value;
                return;
            case Render::Particle::MergeType::DIVIDE:
                AsScalar(info, index) /= value;
                return;
        }
    }
    
    if (info.type == Render::Particle::DataType::VECTOR) {
        if (dest != Render::Particle::MergeDest::ANY) {
            float extracted;
            switch (dest) {
                case Render::Particle::MergeDest::ANY:
                case Render::Particle::MergeDest::X: extracted = AsVector(info, index).x; break;
                case Render::Particle::MergeDest::Y: extracted = AsVector(info, index).y; break;
                case Render::Particle::MergeDest::Z: extracted = AsVector(info, index).z; break;
                break;
            }
            
            switch (type) {
                case Render::Particle::MergeType::SET:      extracted = value; break;
                case Render::Particle::MergeType::ADD:      extracted += value; break;
                case Render::Particle::MergeType::SUBTRACT: extracted -= value; break;
                case Render::Particle::MergeType::MULTIPLY: extracted *= value; break;
                case Render::Particle::MergeType::DIVIDE:   extracted /= value; break;
            }
            
            switch (dest) {
                case Render::Particle::MergeDest::ANY:
                case Render::Particle::MergeDest::X: AsVector(info, index).x = extracted; break;
                case Render::Particle::MergeDest::Y: AsVector(info, index).y = extracted; break;
                case Render::Particle::MergeDest::Z: AsVector(info, index).z = extracted; break;
                break;
            }
            
            return;
        }
        
        switch (type) {
            case Render::Particle::MergeType::SET:
                AsVector(info, index) = {value, value, value};
                return;
            case Render::Particle::MergeType::ADD:
                AsVector(info, index) += value;
                return;
            case Render::Particle::MergeType::SUBTRACT:
                AsVector(info, index) -= value;
                return;
            case Render::Particle::MergeType::MULTIPLY:
                AsVector(info, index) *= value;
                return;
            case Render::Particle::MergeType::DIVIDE:
                AsVector(info, index) /= value;
                return;
        }
    }
}

void ParticleComponent::Start() {
    if (!particle) {
        Log(Severity::CRITICAL_ERROR, tram::System::RENDER, "Particle component doesn't have its particle set!");
    }
    
    data = new float[particle->GetDataSize()];
    memset(data, 0, particle->GetDataSize() * sizeof(float));
    
    Particle::System* systems[16];
    for (int i = 0; i < 16; i++) systems[i] = nullptr;
    int sysc = particle->GetSystems(systems, 16);
    
    for (int i = 0; i < sysc; i++) {
        System new_sys;
        new_sys.slots.assign(systems[i]->GetParticleLimit(), false);
        new_sys.system = systems[i];
        this->systems.push_back(new_sys);
    }
    
    for (int i = 0; particle->GetBaseSystem()->GetInitializer(i); i++) {
        PerformOperation(particle->GetBaseSystem()->GetInitializer(i), 0, 1);
    }

    sprite_array = CreateSpriteArray();
    
    Render::material_t textures[16];
    for (int i = 0; i < sysc; i++) {
        textures[i] = systems[i]->GetSprite()->GetMaterial()->GetMaterial();
    }

    draw_list_entry = InsertDrawListEntry();
    SetDrawListSpriteArray(draw_list_entry, sprite_array);
    SetDrawListIndexRange(draw_list_entry, 0, 0);
    SetFlags(draw_list_entry, FLAG_RENDER);
    SetDrawListMaterials(draw_list_entry, sysc, textures);
    SetDrawListShader(draw_list_entry, VERTEX_SPRITE, MATERIAL_TEXTURE_ALPHA);

    is_ready = true;
    UpdateRenderListObject();
}

void ParticleComponent::Update() {
    if (!is_ready) return;
    
    Particle::System* systems[16];
    int sysc = particle->GetSystems(systems, 16);
    
    // recalculate base
    for (int i = 0; particle->GetBaseSystem()->GetOperation(i); i++) {
        PerformOperation(particle->GetBaseSystem()->GetOperation(i), 0, 1);
    }
    
    // move particles
    for (int s = 0; s < sysc; s++) {
    for (int i = 0; systems[s]->GetOperation(i); i++) {
        PerformOperation(systems[s]->GetOperation(i), 0, systems[s]->GetParticleLimit());
    }}
    
    // constrain particles
    for (int s = 0; s < sysc; s++) {
    for (int i = 0; systems[s]->GetConstraint(i); i++) {
        PerformConstraint(systems[s]->GetConstraint(i), s);
    }}
    
    // emit new particles
    for (int s = 0; s < sysc; s++) {
        for (int i = 0; systems[s]->GetEmitter(i); i++) {
            PerformEmit(systems[s]->GetEmitter(i), s);
        }
        this->systems[s].since_last_emit += GetDeltaTime();
    }
    
    UpdateRenderListObject();
}

void ParticleComponent::UpdateRenderListObject() {
    if (!is_ready) return;
    
    std::vector<Render::SpritePoint> points;

    for (int s = 0; s < (int)systems.size(); s++)
        for (int i = 0; i < (int)systems[s].slots.size(); i++) {
            if (!systems[s].slots[i]) continue;
            
            vec3 position = {0.0f, 0.0f, 0.0f};
            float rotation = 0.0f;
            vec3 color = {1.0f, 1.0f, 1.0f};
            float width = 1.0f;
            float height = 1.0f;
            int frame = 0;
            
            const auto system = systems[s].system;
            
            if (system->GetPositionOffset() >= 0) {
                position.x = data[system->GetPositionOffset() + i * 3 + 0];
                position.y = data[system->GetPositionOffset() + i * 3 + 1];
                position.z = data[system->GetPositionOffset() + i * 3 + 2];
            }
            
            if (system->GetRotationOffset() >= 0) {
                rotation = data[system->GetRotationOffset() + i];
            }
            
            if (system->GetColorOffset() >= 0) {
                color.x = data[system->GetColorOffset() + i * 3 + 0];
                color.y = data[system->GetColorOffset() + i * 3 + 1];
                color.z = data[system->GetColorOffset() + i * 3 + 2];
            }
            
            if (system->GetWidthOffset() >= 0) {
                width = data[system->GetWidthOffset() + i];
            }
            
            if (system->GetHeightOffset() >= 0) {
                height = data[system->GetHeightOffset() + i];
            }
            
            if (system->GetFrameOffset() >= 0) {
                frame = data[system->GetFrameOffset() + i];
            }
            
            const auto sprite = system->GetSprite();
            
            float tex_width = (float)sprite->GetFrames()[frame].width / (float)sprite->GetMaterial()->GetWidth();
            float tex_height = (float)sprite->GetFrames()[frame].height / (float)sprite->GetMaterial()->GetHeight();
            float tex_w_off = (float)sprite->GetFrames()[frame].offset_x / (float)sprite->GetMaterial()->GetWidth();
            float tex_h_off = (float)sprite->GetFrames()[frame].offset_y / (float)sprite->GetMaterial()->GetHeight();
            
            float tex_x_mid = (float)sprite->GetFrames()[frame].midpoint_x / (float)sprite->GetMaterial()->GetWidth();
            float tex_y_mid = (float)sprite->GetFrames()[frame].midpoint_y / (float)sprite->GetMaterial()->GetHeight();

            tex_h_off = 1.0f - tex_h_off - tex_height;

            Render::SpritePoint point;            
            point.position = position;
            point.color = color;
            point.rotation = rotation;
            point.dimensions = {tex_width * width * 10.0f, tex_height * height * 10.0f}; 
            point.midpoint = {tex_x_mid * 10.0f, tex_y_mid * 10.0f};
            point.texture_offset = {tex_w_off, tex_h_off};
            point.texture_size = {tex_width, tex_height};
            point.texture = s;

            AddLineMarker(position, COLOR_GREEN);
            
            points.push_back(point);
    }

    UpdateSpriteArray(sprite_array, points.size(), points.data());
    SetDrawListIndexRange(draw_list_entry, 0, points.size() * 6);
    
    Render::API::SetMatrix(draw_list_entry, PositionRotationToMatrix(location, quat(1.0f, 0.0f, 0.0f, 0.0f)));
    
    AddLineMarker(location, COLOR_WHITE);
}

/// Creates a new ParticleComponent.
ParticleComponent* ParticleComponent::Make() {
    ParticleComponent* ptr = PoolProxy<ParticleComponent>::GetPool().allocate();
    new(ptr) ParticleComponent();
    return ptr;
}

/// Deletes an ParticleComponent.
void ParticleComponent::Yeet(ParticleComponent* component) {
    component->~ParticleComponent();
    PoolProxy<ParticleComponent>::GetPool().deallocate(component);
}

}