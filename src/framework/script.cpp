// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/script.h>

#include <cstring>

#include <iostream>

#include <framework/core.h>
#include <framework/entity.h>
#include <framework/event.h>
#include <framework/ui.h>
#include <framework/worldcell.h>
#include <audio/audio.h>
#include <entities/script.h>
#include <physics/physics.h>
#include <physics/collisionshape.h>
#include <physics/collisionmodel.h>
#include <render/render.h>
#include <render/material.h>
#include <render/model.h>
#include <render/sprite.h>
#include <render/api.h>
#include <physics/physics.h>
#include <components/render.h>
#include <components/animation.h>
#include <components/light.h>

using namespace tram::Render;

namespace tram::Script {

static Language language = {
    name_t(),
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};
    
void SetGlobal(name_t name, value_t value) {
    language.set_global(name, value);
}

value_t GetGlobal(name_t name) {
    return language.get_global(name);
}

void SetFunction(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t)) {
    language.set_function(name, parameters, function);
}

value_t CallFunction(name_t name, std::vector<Value> parameters) {
    return language.call_function(name, parameters);
}

void SetLanguage(Language lang) {
    language = lang;
}

void LoadScript(const char* path) {
    language.load_script(path);
}

void Init() {
    if (!language.name) {
        std::cout << "Failed script initialization! Language not set!" << std::endl;
        abort();
    }
    
    // FRAMEWORK/CORE.H
    
    SetFunction("__tram_impl_core_get_tick", {}, [](valuearray_t) -> value_t {
        return GetTick();
    });
    
    SetFunction("__tram_impl_core_get_tick_time", {}, [](valuearray_t) -> value_t {
        return (float)GetTickTime();
    });
    
    SetFunction("__tram_impl_core_get_delta_time", {}, [](valuearray_t) -> value_t {
        return GetDeltaTime();
    });
    
    
    // FRAMEWORK/MATH.H
    
    SetFunction("__tram_impl_math_quat_from_euler", {TYPE_VEC3}, [](valuearray_t array) -> value_t {
        return quat((vec3)array[0]);
    });
    
    SetFunction("__tram_impl_math_quat_multiply", {TYPE_QUAT, TYPE_QUAT}, [](valuearray_t array) -> value_t {
        return (quat)array[0] * (quat)array[1];
    });
    
    SetFunction("__tram_impl_math_quat_vec3_multiply", {TYPE_QUAT, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        return (quat)array[0] * (vec3)array[1];
    });
    
    
    
    
    // FRAMEWORK/EVENT.H
    
    SetFunction("__tram_impl_event_register", {TYPE_STRING}, [](valuearray_t array) -> value_t {
        assert(array.size());
        const char* name = array[0];
        assert(name);
        char* copy = (char*)malloc(strlen(name) + 1);
        strcpy(copy, name);
        
        return Event::Register(copy);
    });
    
    SetFunction("__tram_impl_event_get_type", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        return Event::GetType(array[0]);
    });
    
    SetFunction("__tram_impl_event_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return Event::GetName((uint32_t)array[0]);
    });
    
    SetFunction("__tram_impl_event_get_last", {}, [](valuearray_t) -> value_t {
        return Event::GetLast();
    });
    
    SetFunction("__tram_impl_event_post", {TYPE_UINT32, TYPE_UINT32, TYPE_UINT32, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        Event event;
        
        event.type = (uint32_t)array[0];
        event.subtype = (uint32_t)array[1];
        event.poster_id = array[2];
        
        if (array[3].GetType() == TYPE_UNDEFINED) {
            event.data = nullptr;
        } else {
            event.data_value = Event::AllocateData<Value>(array[3]);
        }
    
        Event::Post(event);
        
        return name_t();
    });
    
    SetFunction("__tram_impl_event_add_listener", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        size_t data_int = (uint32_t)array[1];
        void* data_ptr = (void*)data_int;
        
        return Event::AddListener((uint32_t)array[0], data_ptr, [](Event& event, void* data) {
            size_t data_ptr = (size_t)data;
            uint32_t data_int = (uint32_t)data_ptr;
            
            CallFunction("__tram_impl_event_event_callback", {event.type,
                                                              event.subtype,
                                                              event.poster_id,
                                                              (uint32_t)event.data_int,
                                                              data_int});
        });
    });
    
    SetFunction("__tram_impl_event_remove_listener", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Event::RemoveListener((listener_t)array[0]);
        return name_t();
    });
    
    
    
    
    // FRAMEWORK/MESSAGE.H
    
    SetFunction("__tram_impl_message_register", {TYPE_STRING}, [](valuearray_t array) -> value_t {
        assert(array.size());
        const char* name = array[0];
        assert(name);
        char* copy = (char*)malloc(strlen(name) + 1);
        strcpy(copy, name);
        
        return Message::Register(copy);
    });
    
    SetFunction("__tram_impl_message_get_type", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        return Message::GetType(array[0]);
    });
    
    SetFunction("__tram_impl_message_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return Message::GetName(array[0]);
    });
    
    SetFunction("__tram_impl_message_get_last", {}, [](valuearray_t) -> value_t {
        return Message::GetLast();
    });
    
    SetFunction("__tram_impl_message_send", {TYPE_UINT32, TYPE_UINT32, TYPE_UINT32, TYPE_UNDEFINED, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        Message message;
        
        message.type = array[0];
        message.sender = array[1];
        message.receiver = array[2];
        
        if (array[3].GetType() == TYPE_UNDEFINED) {
            message.data = nullptr;
        } else {
            message.data_value = Message::AllocateData<Value>(array[3]);
            
            if (array[3].GetType() == TYPE_STRING) {
                *message.data_value = name_t((const char*)array[3]);
            }
        }
        
        if (array[4].GetType() == TYPE_UNDEFINED || array[4].GetFloat() == 0.0f) {
            Message::Send(message);
        } else {
            Message::Send(message, array[4].GetFloat());
        }
        
        return name_t();
    });
    
    
    
    
    // FRAMEWORK/ENTITY.H
    
    SetFunction("__tram_impl_entity_find_by_name", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        //std::cout << "findning !! " << (name_t)array[0] << std::endl;
        Entity* entity = Entity::Find((name_t)array[0]);
        //std::cout << "found: " << entity << " | " << entity->GetID() << std::endl;
        return entity ? entity->GetID() : 0;
        //return entity->GetID();
    });
    
    SetFunction("__tram_impl_entity_find_by_id", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetID() : 0;
    });
    
    SetFunction("__tram_impl_entity_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetName() : name_t();
    });
    
    
    SetFunction("__tram_impl_entity_load", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        if (entity) entity->Load();
        return true;
    });
    
    SetFunction("__tram_impl_entity_unload", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        if (entity) entity->Unload();
        return true;
    });
    
    
    SetFunction("__tram_impl_entity_get_location", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetLocation() : vec3();
    });
    
    SetFunction("__tram_impl_entity_set_location", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        if (entity) entity->SetLocation(array[1]);
        return name_t();
    });
    
    SetFunction("__tram_impl_entity_get_rotation", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetRotation() : quat(1.0f, 0.0f, 0.0f, 0.0f);
    });
    
    SetFunction("__tram_impl_entity_set_rotation", {TYPE_UINT32, TYPE_QUAT}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        if (entity) entity->SetRotation(array[1]);
        return name_t();
    });
    
    
    // ENTITIES/SCRIPT.H
    
    SetFunction("__tram_impl_entity_add_listener", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[1]);
        
        if (!entity) return -1;
        
        return Event::AddListener((uint32_t)array[0], entity);
    });
    
    static std::vector<std::pair<Value, Value>> key_values;
    SetFunction("__tram_impl_clear_key_value", {}, [](valuearray_t array) -> value_t {
        key_values.clear();
        return true;
    });
    
    SetFunction("__tram_impl_push_key_value", {TYPE_UNDEFINED, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        key_values.push_back({array[0], array[1]});
        return true;
    });
    
    static auto pack_key_values = [](SharedEntityData& shared_data, std::vector<Value>& properties) {
        for (auto& [key, value] : key_values) {
            if (key.GetType() == TYPE_STRING) {
                const name_t key_name = (const char*)key;
                
                if (key_name == "id") {
                    shared_data.id = value.GetInt();;
                } else if (key_name == "name") {
                    shared_data.name = (const char*)value;
                } else if (key_name == "flags") {
                    shared_data.flags = value.GetInt();
                } else if (key_name == "location") {
                    shared_data.position = value;
                } else if (key_name == "rotation") {
                    shared_data.rotation = value;
                } else {
                    std::cout << "__tram_impl_entity_make: key-value array has unknown key value: " << key_name << std::endl;
                }
                
            } else if (key.IsInt()) {
                assert(key.GetInt() >= 0);
                assert(key.GetInt() < 100);
                
                if (properties.size() <= (size_t)key.GetInt()) {
                    properties.resize(key.GetInt() + 1);
                }
                
                properties[key.GetInt()] = value;
            } else {
                std::cout << "__tram_impl_entity_make: key-value array has unknown key type " << key.GetType() << std::endl;
            }
        }
    };
    
    
    SetFunction("__tram_impl_entity_make", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        name_t type = array[0];
        
        SharedEntityData shared_data;
        std::vector<Value> properties;
        
        pack_key_values(shared_data, properties);
        
        ValueArray field_array(properties.data(), properties.size());
        
        Entity* new_entity = Entity::Make(type, shared_data, field_array);
        
        return new_entity->GetID();
    });
    
    SetFunction("__tram_impl_entity_scriptable_make", {TYPE_NAME, TYPE_NAME}, [](valuearray_t array) -> value_t {
        name_t base_type = array[0];
        name_t new_type = array[1];
        
        SharedEntityData shared_data;
        std::vector<Value> properties;
        
        pack_key_values(shared_data, properties);
        
        ValueArray field_array(properties.data(), properties.size());
        
        Entity* new_entity = ScriptableType::Make(base_type, new_type, shared_data, field_array);
        
        return new_entity->GetID();
    });
    
    
    // FRAMEWORK/UI.H
    
    SetFunction("__tram_impl_ui_get_screen_width", {}, [](valuearray_t) -> value_t {
        return UI::GetScreenWidth();
    });
    
    SetFunction("__tram_impl_ui_get_screen_height", {}, [](valuearray_t) -> value_t {
        return UI::GetScreenHeight();
    });
    
    SetFunction("__tram_impl_ui_set_window_title", {TYPE_STRING}, [](valuearray_t array) -> value_t {
        UI::SetWindowTitle(array[0]);
        return true;
    });
    
    SetFunction("__tram_impl_ui_set_window_size", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        UI::SetWindowSize((uint32_t)array[0], (uint32_t)array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_ui_set_cursor", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        UI::SetCursor((UI::CursorType)((uint32_t)array[0]));
        return true;
    });
    
    SetFunction("__tram_impl_ui_bind_keyboard_action", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        UI::BindKeyboardKey((UI::KeyboardKey)((uint32_t)array[0]), (uint32_t)array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_ui_bind_keyboard_callback", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        UI::BindKeyboardKey((UI::KeyboardKey)((uint32_t)array[0]), [](UI::KeyboardKey key) {
            CallFunction("__tram_impl_ui_keyboard_callback", {key});
        });
        return true;
    });
    
    SetFunction("__tram_impl_ui_poll_keyboard_key", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return UI::PollKeyboardKey((UI::KeyboardKey)((uint32_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_poll_keyboard_axis", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return UI::PollKeyboardAxis((UI::KeyboardAxis)((uint32_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_poll_keyboard_axis_delta", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return UI::PollKeyboardAxis((UI::KeyboardAxis)((uint32_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_set_input_state", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        UI::SetInputState((UI::InputState)((uint32_t)array[0]));
        return true;
    });
    
    SetFunction("__tram_impl_ui_get_input_state", {}, [](valuearray_t) -> value_t {
        return UI::GetInputState();
    });
    
    SetFunction("__tram_impl_ui_get_axis_sensitivity", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return UI::GetAxisSensitivity((UI::KeyboardAxis)((uint32_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_set_axis_sensitivity", {TYPE_UINT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        UI::SetAxisSensitivity((UI::KeyboardAxis)((uint32_t)array[0]), array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_ui_register_keyboard_action", {TYPE_STRING}, [](valuearray_t array) -> value_t {
        return UI::RegisterKeyboardAction(array[0]);
    });
    
    SetFunction("__tram_impl_ui_get_keyboard_action", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        return UI::GetKeyboardAction(array[0]);
    });
    
    SetFunction("__tram_impl_ui_get_keyboard_action_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return UI::GetKeyboardActionName((uint32_t)array[0]);
    });
    
    
    
    
    // FRAMEWORK/SCRIPT.H
    
    SetFunction("__tram_impl_worldcell_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        WorldCell* cell = WorldCell::Find((name_t)array[0]);
        
        if (cell) {
            return (uint32_t)PoolProxy<WorldCell>::GetPool().index(cell);
        } else {
            return -1;
        }
    });
    
    SetFunction("__tram_impl_worldcell_make", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        WorldCell* cell = WorldCell::Make((name_t)array[0]);
        
        if (cell) {
            return (uint32_t)PoolProxy<WorldCell>::GetPool().index(cell);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_worldcell_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<WorldCell>::GetPool()[(uint32_t)array[0]].GetName();
    });
    
    SetFunction("__tram_impl_worldcell_loadfromdisk", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<WorldCell>::GetPool()[(uint32_t)array[0]].LoadFromDisk();
        return true;
    });
    
    SetFunction("__tram_impl_worldcell_load", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<WorldCell>::GetPool()[(uint32_t)array[0]].Load();
        return true;
    });
    
    SetFunction("__tram_impl_worldcell_unload", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<WorldCell>::GetPool()[(uint32_t)array[0]].Unload();
        return true;
    });
    
    
    
    
    // AUDIO/AUDIO.H
    SetFunction("__tram_impl_audio_set_volume", {TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        Audio::SetVolume(array[0]);
        return true;
    });
    
    SetFunction("__tram_impl_audio_get_volume", {}, [](valuearray_t) -> value_t {
        return Audio::GetVolume();
    });
    
    SetFunction("__tram_impl_audio_set_listener_position", {TYPE_VEC3}, [](valuearray_t array) -> value_t {
        Audio::SetListenerPosition(array[0]);
        return true;
    });
    
    SetFunction("__tram_impl_audio_set_listener_orientation", {TYPE_QUAT}, [](valuearray_t array) -> value_t {
        Audio::SetListenerOrientation(array[0]);
        return true;
    });


    // PHYSICS/PHYSICS.H
    
    // TODO: add raycasting
    
    
    
    
    // PHYSICS/COLLISIONSHAPE.H
    static vec3* hull_points = new vec3[100];
    static Physics::CollisionTriangle* mesh_tris = new Physics::CollisionTriangle[100];
    static size_t hull_point_count = 0;
    static size_t mesh_tri_count = 0;
    static Physics::CollisionShape coll_shape;
    
    SetFunction("__tram_impl_physics_collision_set_sphere", {TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        coll_shape = Physics::CollisionShape::Sphere(array[0]);
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_set_cylinder", {TYPE_FLOAT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        coll_shape = Physics::CollisionShape::Cylinder(array[0], array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_set_capsule", {TYPE_FLOAT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        coll_shape = Physics::CollisionShape::Cylinder(array[0], array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_set_cone", {TYPE_FLOAT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        coll_shape = Physics::CollisionShape::Cylinder(array[0], array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_set_box", {TYPE_VEC3}, [](valuearray_t array) -> value_t {
        coll_shape = Physics::CollisionShape::Box(array[0]);
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_set_hull", {}, [](valuearray_t) -> value_t {
        coll_shape = Physics::CollisionShape::Hull(hull_points, 0);
        hull_point_count = 0;
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_set_mesh", {}, [](valuearray_t) -> value_t {
        coll_shape = Physics::CollisionShape::Mesh(mesh_tris, 0);
        mesh_tri_count = 0;
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_append_point", {TYPE_VEC3}, [](valuearray_t array) -> value_t {
        hull_points[hull_point_count++] = array[0];
        coll_shape.hull_size = hull_point_count;
        return true;
    });
    
    SetFunction("__tram_impl_physics_collision_append_triangle", {TYPE_VEC3, TYPE_VEC3, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        mesh_tris[mesh_tri_count++] = {array[0], array[1], array[2]};
        coll_shape.mesh_size = mesh_tri_count;
        return true;
    });
    
    
    
    
    
    // PHYSICS/COLLISIONMODEL.H
    SetFunction("__tram_impl_physics_collisionmodel_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        Model* model = Model::Find(array[0]);
        
        if (model) {
            return (uint32_t)PoolProxy<Model>::GetPool().index(model);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_physics_collisionmodel_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Model>::GetPool()[(uint32_t)array[0]].GetName();
    });
    
    
    
    
    // RENDER/RENDER.H
    SetFunction("__tram_impl_render_set_sun_direction", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].GetInt() ? array[1].GetInt() : 0;
        Render::SetSunDirection(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_sun_color", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].GetInt() ? array[1].GetInt() : 0;
        Render::SetSunColor(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_ambient_color", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].GetInt() ? array[1].GetInt() : 0;
        Render::SetAmbientColor(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_screen_clear_color", {TYPE_VEC3}, [](valuearray_t array) -> value_t {
        Render::API::SetScreenClear(array[0], true);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_fov", {TYPE_FLOAT32, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].GetInt() ? array[1].GetInt() : 0;
        Render::SetViewFov(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_distance", {TYPE_FLOAT32, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].GetInt() ? array[1].GetInt() : 0;
        Render::SetViewDistance(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_position", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].GetInt() ? array[1].GetInt() : 0;
        Render::SetViewPosition(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_rotation", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].GetInt() ? array[1].GetInt() : 0;
        Render::SetViewRotation(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_get_view_position", {TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[0].GetInt() ? array[0].GetInt() : 0;
        return Render::GetViewPosition(layer);
    });
    
    SetFunction("__tram_impl_render_add_line", {TYPE_VEC3, TYPE_VEC3, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        Render::AddLine(array[0], array[1], array[2]);
        return true;
    });
    
    SetFunction("__tram_impl_render_add_line_marker", {TYPE_VEC3, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        Render::AddLineMarker(array[0], array[1]);
        return true;
    });


    // RENDER/ANIMATION.H
    SetFunction("__tram_impl_render_animation_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        Animation* animation = Animation::Find(array[0]);
        
        if (animation) {
            return (uint32_t)PoolProxy<Animation>::GetPool().index(animation);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_render_animation_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Animation>::GetPool()[(uint32_t)array[0]].GetName();
    });
    
    SetFunction("__tram_impl_render_animation_load", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<Animation>::GetPool()[(uint32_t)array[0]].Load();
        return true;
    });




    // RENDER/MATERIAL.H
    SetFunction("__tram_impl_render_material_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        Material* material = Material::Find(array[0]);
        
        if (material) {
            return (uint32_t)PoolProxy<Material>::GetPool().index(material);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_render_material_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Material>::GetPool()[(uint32_t)array[0]].GetName();
    });



    // RENDER/MODEL.H
    SetFunction("__tram_impl_render_model_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        Model* model = Model::Find(array[0]);
        
        if (model) {
            return (uint32_t)PoolProxy<Model>::GetPool().index(model);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_render_model_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Model>::GetPool()[(uint32_t)array[0]].GetName();
    });

    SetFunction("__tram_impl_render_model_add_reference", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<Model>::GetPool()[(uint32_t)array[0]].AddReference();
        return true;
    });
    
    SetFunction("__tram_impl_render_model_remove_reference", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<Model>::GetPool()[(uint32_t)array[0]].RemoveReference();
        return true;
    });
    
    SetFunction("__tram_impl_render_model_load", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<Model>::GetPool()[(uint32_t)array[0]].Load();
        return true;
    });
    
    SetFunction("__tram_impl_render_model_get_aabb_min", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Model>::GetPool()[(uint32_t)array[0]].GetAABBMin();
    });
    
    SetFunction("__tram_impl_render_model_get_aabb_max", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Model>::GetPool()[(uint32_t)array[0]].GetAABBMax();
    });
    
    SetFunction("__tram_impl_render_model_get_near_distance", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Model>::GetPool()[(uint32_t)array[0]].GetNearDistance();
    });
    
    SetFunction("__tram_impl_render_model_get_far_distance", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Model>::GetPool()[(uint32_t)array[0]].GetFarDistance();
    });
    
    SetFunction("__tram_impl_render_model_set_near_distance", {TYPE_UINT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<Model>::GetPool()[(uint32_t)array[0]].SetNearDistance(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_render_model_set_far_distance", {TYPE_UINT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<Model>::GetPool()[(uint32_t)array[0]].SetFarDistance(array[1]);
        return true;
    });



    // RENDER/SPRITE.H
    SetFunction("__tram_impl_render_sprite_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        Sprite* sprite = Sprite::Find(array[0]);
        
        if (sprite) {
            return (uint32_t)PoolProxy<Sprite>::GetPool().index(sprite);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_render_sprite_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Sprite>::GetPool()[(uint32_t)array[0]].GetName();
    });

    







    // CMOMOAWMDPA WMDP MPAWDM PAW DMAPWD MAWD P

    SetFunction("__tram_impl_components_render_make", {}, [](valuearray_t array) -> value_t {
        RenderComponent* component = PoolProxy<RenderComponent>::New();
        
        if (component) {
            return (uint32_t)PoolProxy<RenderComponent>::GetPool().index(component);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_components_render_get_model", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Model* model = PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].GetModel();
        return (uint32_t)PoolProxy<Model>::GetPool().index(model);
    });

    SetFunction("__tram_impl_components_render_set_model", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Model* model = &PoolProxy<Model>::GetPool()[(uint32_t)array[1]];
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetModel(model->GetName());
        return true;
    });
    
    SetFunction("__tram_impl_components_render_set_lightmap", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Material* material = &PoolProxy<Material>::GetPool()[(uint32_t)array[1]];
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetLightmap(material->GetName());
        return true;
    });
    
    SetFunction("__tram_impl_components_set_armature", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        AnimationComponent* armature = &PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[1]];
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetArmature(armature);
        return true;
    });

    SetFunction("__tram_impl_components_render_get_location", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].GetLocation();
    });
    
    SetFunction("__tram_impl_components_render_get_rotation", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].GetRotation();
    });
    
    SetFunction("__tram_impl_components_render_set_location", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetLocation(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_render_set_rotation", {TYPE_UINT32, TYPE_QUAT}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetRotation(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_render_set_scale", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetScale(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_render_set_color", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetColor(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_render_set_layer", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetLayer(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_render_set_directional_light", {TYPE_UINT32, TYPE_BOOL}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].SetDirectionaLight(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_render_init", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].Init();
        return true;
    });
    
    SetFunction("__tram_impl_components_render_delete", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<RenderComponent>::Delete(&PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]]);
        return true;
    });






    SetFunction("__tram_impl_components_light_make", {}, [](valuearray_t array) -> value_t {
        LightComponent* component = PoolProxy<LightComponent>::New();
        
        if (component) {
            return (uint32_t)PoolProxy<LightComponent>::GetPool().index(component);
        } else {
            return -1;
        }
    });
    
    SetFunction("__tram_impl_components_light_init", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<LightComponent>::GetPool()[(uint32_t)array[0]].Init();
        return true;
    });
    
    SetFunction("__tram_impl_components_light_delete", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<LightComponent>::Delete(&PoolProxy<LightComponent>::GetPool()[(uint32_t)array[0]]);
        return true;
    });

    SetFunction("__tram_impl_components_light_set_location", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        PoolProxy<LightComponent>::GetPool()[(uint32_t)array[0]].SetLocation(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_light_set_color", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        PoolProxy<LightComponent>::GetPool()[(uint32_t)array[0]].SetColor(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_light_set_distance", {TYPE_UINT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<LightComponent>::GetPool()[(uint32_t)array[0]].SetDistance(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_light_set_direction", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        PoolProxy<LightComponent>::GetPool()[(uint32_t)array[0]].SetDirection(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_light_set_exponent", {TYPE_UINT32, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<LightComponent>::GetPool()[(uint32_t)array[0]].SetExponent(array[1]);
        return true;
    });







    SetFunction("__tram_impl_components_animation_make", {}, [](valuearray_t array) -> value_t {
        AnimationComponent* component = PoolProxy<AnimationComponent>::New();
        
        if (component) {
            return (uint32_t)PoolProxy<AnimationComponent>::GetPool().index(component);
        } else {
            return -1;
        }
    });
    
    SetFunction("__tram_impl_components_animation_init", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].Init();
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_delete", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::Delete(&PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]]);
        return true;
    });


    SetFunction("__tram_impl_components_animation_get_model", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Model* model = PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].GetModel();
        return (uint32_t)PoolProxy<Model>::GetPool().index(model);
    });

    SetFunction("__tram_impl_components_animation_set_model", {TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Model* model = &PoolProxy<Model>::GetPool()[(uint32_t)array[1]];
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetModel(model);
        return true;
    });

    SetFunction("__tram_impl_components_animation_set_keyframe", {TYPE_UINT32, TYPE_NAME, TYPE_VEC3, TYPE_QUAT, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        Keyframe keyframe = {.location = array[2], .rotation = array[3], .scale = array[4]};
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetKeyframe(array[1], keyframe);
        return true;
    });

    SetFunction("__tram_impl_components_animation_add_finish_callback", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetOnAnimationFinishCallback([](AnimationComponent* comp, name_t anim) {
            uint32_t id = PoolProxy<AnimationComponent>::GetPool().index(comp);
            CallFunction("__tram_impl_components_animation_finish_callback", {id, anim});
        });
        return true;
    });


    SetFunction("__tram_impl_components_animation_play", {TYPE_UINT32, TYPE_NAME, TYPE_UINT32, TYPE_FLOAT32, TYPE_FLOAT32, TYPE_BOOL, TYPE_BOOL}, [](valuearray_t array) -> value_t {
        std::cout << "playing! " << (name_t)array[1] << " with " << (uint32_t)array[2] << std::endl;
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].Play(array[1], array[2], array[3], array[4], array[5], array[6]);
        return true;
    });

    SetFunction("__tram_impl_components_animation_is_playing", {TYPE_UINT32, TYPE_NAME}, [](valuearray_t array) -> value_t {
        return PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].IsPlaying(array[1]);
    });
    
    SetFunction("__tram_impl_components_animation_stop", {TYPE_UINT32, TYPE_NAME}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].Stop(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_pause", {TYPE_UINT32, TYPE_NAME}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].Pause(array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_continue", {TYPE_UINT32, TYPE_NAME}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].Continue(array[1]);
        return true;
    });
    
    
    SetFunction("__tram_impl_components_animation_set_weight", {TYPE_UINT32, TYPE_NAME, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetWeight(array[1], array[2]);
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_set_speed", {TYPE_UINT32, TYPE_NAME, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetSpeed(array[1], array[2]);
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_set_repeats", {TYPE_UINT32, TYPE_NAME, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetRepeats(array[1], array[2]);
        return true;
    });
    
    
    SetFunction("__tram_impl_components_animation_fade_in", {TYPE_UINT32, TYPE_NAME, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].FadeIn(array[1], array[2]);
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_fade_out", {TYPE_UINT32, TYPE_NAME, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].FadeOut(array[1], array[2]);
        return true;
    });
    
    
    SetFunction("__tram_impl_components_animation_set_pause", {TYPE_UINT32, TYPE_NAME, TYPE_BOOL}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetPause(array[1], array[2]);
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_set_fade", {TYPE_UINT32, TYPE_NAME, TYPE_BOOL, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetFade(array[1], array[2], array[3]);
        return true;
    });
    
    SetFunction("__tram_impl_components_animation_set_frame", {TYPE_UINT32, TYPE_NAME, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        PoolProxy<AnimationComponent>::GetPool()[(uint32_t)array[0]].SetFrame(array[1], array[2]);
        return true;
    });
    

    LoadScript("api");
}

}
