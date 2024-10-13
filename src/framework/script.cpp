// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/script.h>

#include <cstring>

#include <iostream>

#include <framework/core.h>
#include <framework/entity.h>
#include <framework/event.h>
#include <framework/ui.h>
#include <audio/audio.h>
#include <render/render.h>
#include <render/material.h>
#include <render/model.h>
#include <render/sprite.h>
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
        return GetTickTime();
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
    
    SetFunction("__tram_impl_event_get_name", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        return Event::GetName(array[0]);
    });
    
    SetFunction("__tram_impl_event_get_last", {}, [](valuearray_t) -> value_t {
        return Event::GetLast();
    });
    
    SetFunction("__tram_impl_event_post", {TYPE_UINT16, TYPE_UINT16, TYPE_UINT32, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        Event event;
        
        event.type = array[0];
        event.subtype = array[1];
        event.poster_id = array[2];
        
        if (array[3].GetType() == TYPE_UNDEFINED) {
            event.data = nullptr;
        } else {
            event.data_value = Event::AllocateData<Value>(array[3]);
        }
    
        Event::Post(event);
        
        return name_t();
    });
    
    SetFunction("__tram_impl_event_add_listener", {TYPE_UINT16, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        size_t data_int = (uint32_t)array[1];
        void* data_ptr = (void*)data_int;
        
        return Event::AddListener(array[0], data_ptr, [](Event& event, void* data) {
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
    
    SetFunction("__tram_impl_ui_bind_keyboard_action", {TYPE_UINT16, TYPE_UINT16}, [](valuearray_t array) -> value_t {
        UI::BindKeyboardKey((UI::KeyboardKey)((uint16_t)array[0]), (uint16_t)array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_ui_bind_keyboard_callback", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        UI::BindKeyboardKey((UI::KeyboardKey)((uint16_t)array[0]), [](UI::KeyboardKey key) {
            CallFunction("__tram_impl_ui_keyboard_callback", {key});
        });
        return true;
    });
    
    SetFunction("__tram_impl_ui_poll_keyboard_key", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        return UI::PollKeyboardKey((UI::KeyboardKey)((uint16_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_poll_keyboard_axis", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        return UI::PollKeyboardAxis((UI::KeyboardAxis)((uint16_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_poll_keyboard_axis_delta", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        return UI::PollKeyboardAxis((UI::KeyboardAxis)((uint16_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_set_input_state", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        UI::SetInputState((UI::InputState)((uint16_t)array[0]));
        return true;
    });
    
    SetFunction("__tram_impl_ui_get_input_state", {}, [](valuearray_t) -> value_t {
        return UI::GetInputState();
    });
    
    SetFunction("__tram_impl_ui_get_axis_sensitivity", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        return UI::GetAxisSensitivity((UI::KeyboardAxis)((uint16_t)array[0]));
    });
    
    SetFunction("__tram_impl_ui_set_axis_sensitivity", {TYPE_UINT16, TYPE_FLOAT32}, [](valuearray_t array) -> value_t {
        UI::SetAxisSensitivity((UI::KeyboardAxis)((uint16_t)array[0]), array[1]);
        return true;
    });
    
    SetFunction("__tram_impl_ui_register_keyboard_action", {TYPE_STRING}, [](valuearray_t array) -> value_t {
        return UI::RegisterKeyboardAction(array[0]);
    });
    
    SetFunction("__tram_impl_ui_get_keyboard_action", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        return UI::GetKeyboardAction(array[0]);
    });
    
    SetFunction("__tram_impl_ui_get_keyboard_action_name", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        return UI::GetKeyboardActionName((uint16_t)array[0]);
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

    
    
    
    
    // RENDER/RENDER.H
    SetFunction("__tram_impl_render_set_sun_direction", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].IsInt() ? array[1].GetInt() : 0;
        Render::SetSunDirection(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_sun_color", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].IsInt() ? array[1].GetInt() : 0;
        Render::SetSunColor(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_ambient_color", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].IsInt() ? array[1].GetInt() : 0;
        Render::SetAmbientColor(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_fov", {TYPE_FLOAT32, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].IsInt() ? array[1].GetInt() : 0;
        Render::SetViewFov(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_distance", {TYPE_FLOAT32, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].IsInt() ? array[1].GetInt() : 0;
        Render::SetViewDistance(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_position", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].IsInt() ? array[1].GetInt() : 0;
        Render::SetViewPosition(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_set_view_rotation", {TYPE_VEC3, TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[1].IsInt() ? array[1].GetInt() : 0;
        Render::SetViewRotation(array[0], layer);
        return true;
    });
    
    SetFunction("__tram_impl_render_get_view_position", {TYPE_UNDEFINED}, [](valuearray_t array) -> value_t {
        layer_t layer = array[0].IsInt() ? array[0].GetInt() : 0;
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
            return PoolProxy<Animation>::GetPool().index(animation);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_render_animation_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Animation>::GetPool()[(uint32_t)array[0]].GetName();
    });




    // RENDER/MATERIAL.H
    SetFunction("__tram_impl_render_material_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        Material* material = Material::Find(array[0]);
        
        if (material) {
            return PoolProxy<Material>::GetPool().index(material);
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
            return PoolProxy<Model>::GetPool().index(model);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_render_model_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        return PoolProxy<Model>::GetPool()[(uint32_t)array[0]].GetName();
    });




    // RENDER/SPRITE.H
    SetFunction("__tram_impl_render_sprite_find", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        Sprite* sprite = Sprite::Find(array[0]);
        
        if (sprite) {
            return PoolProxy<Sprite>::GetPool().index(sprite);
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
            return PoolProxy<RenderComponent>::GetPool().index(component);
        } else {
            return -1;
        }
    });

    SetFunction("__tram_impl_components_render_get_model", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Model* model = PoolProxy<RenderComponent>::GetPool()[(uint32_t)array[0]].GetModel();
        return PoolProxy<Model>::GetPool().index(model);
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
            return PoolProxy<LightComponent>::GetPool().index(component);
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




    LoadScript("api");
}

}
