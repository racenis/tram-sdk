#include <extensions/menu/debug.h>

#include <render/aabb.h>
//#include <render/render.h>
#include <components/render.h>
#include <framework/entity.h>

namespace tram::Ext::Menu {

void DebugMenu::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
    
        GUI::Button("Worldcells");
        
        if (GUI::Button("Select entity")) {
            auto props = new EntityProperties();
            auto picks = new EntityPicker([=](id_t picked){props->SetPicked(picked);});
            
            Menu::Push(props);
            Menu::Push(picks);
        }
        
        GUI::Button("Emit event");
        
        GUI::Button("Options");
    
        
        
    GUI::PopFrame();
    GUI::PopFrame();
}

void EntityProperties::SetPicked(id_t entity) {
    this->entity = entity;
}

void EntityProperties::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        if (Entity* ptr = Entity::Find(entity); !ptr) {
            GUI::Text(FONT_TEXT, "No entity available.");
        } else {
            GUI::Text(FONT_TEXT, "Entity ID ");
            GUI::TextBox(std::to_string(ptr->GetID()).c_str(), 50);
            GUI::Text(FONT_TEXT, " Name ");
            GUI::TextBox(ptr->GetName(), 100);
            GUI::Text(FONT_TEXT, " Distance ");
            float dist = glm::distance(Render::GetViewPosition(), ptr->GetLocation());
            int dist0 = dist;
            int dist1 = dist*10 - dist0*10;
            GUI::TextBox((std::to_string(dist0) + "." + std::to_string(dist1)).c_str(), 50);
            
            if (GUI::Button("Load")) {
                ptr->Load();
            }
            
            if (GUI::Button("Unload")) {
                ptr->Unload();
            }
                        
            if (GUI::Button("Message")) {
                auto send = new MessageSend;
                send->SetEntity(ptr->GetID());
                Menu::Push(send);
                //auto props = new MessageTypeSelection([](auto){});
                //Menu::Push(props);
            }
        }
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

EntityPicker::EntityPicker(std::function<void(id_t)> callback) {
    this->callback = callback;
}

void EntityPicker::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        float x = UI::PollKeyboardAxis(UI::KEY_MOUSE_X);
        float y = UI::PollKeyboardAxis(UI::KEY_MOUSE_Y);
        
        vec3 far_point = Render::ProjectInverse({x, y, 0.0f});
        vec3 near_point = Render::ProjectInverse({x, y, 1000.0f});
        
        vec3 look_direction = glm::normalize(far_point - near_point);
        vec3 look_position = near_point;
        
        auto res = Render::AABB::FindNearestFromRay(look_position, look_direction, -1);
        
        if (res.data) {
            Render::AddLine(res.triangle.point1, res.triangle.point2, Render::COLOR_WHITE);
            Render::AddLine(res.triangle.point2, res.triangle.point3, Render::COLOR_WHITE);
            Render::AddLine(res.triangle.point3, res.triangle.point1, Render::COLOR_WHITE);
            
            RenderComponent* comp = (RenderComponent*)res.data;
            
            if (comp->GetParent()) {
                std::string text = "Picked: ";
                text += std::to_string(comp->GetParent()->GetID());
                text += " | ";
                text += (const char*)comp->GetParent()->GetName();
                GUI::Text(FONT_TEXT, text.c_str());
                
                if (GUI::ClickHandled()) {
                    callback(comp->GetParent()->GetID());
                    Menu::Pop();
                    return;
                }
            } else {
                GUI::Text(FONT_TEXT, "No entity found for RenderComponent.");
            }
            
            UI::SetCursor(UI::CURSOR_CLICK);
            
        } else {
            GUI::Text(FONT_TEXT, "No pick available.");
            UI::SetCursor(UI::CURSOR_DEFAULT);
        }
        
        
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

MessageTypeSelection::MessageTypeSelection(std::function<void(id_t)> callback) {
    this->callback = callback;
}

void MessageTypeSelection::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34 * 2);
    GUI::PushFrameRelative(GUI::FRAME_LEFT, 200);
    
    GUI::FillFrame(FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        
        message_t selected = Message::NONE;
        
        for (message_t i = 0; i < Message::LAST_MESSAGE; i++) {
            if (GUI::Button(Message::GetName(i))) {
                selected = i;
            }
            
            GUI::NewLine();
        }
        
        if (selected) {
            callback(selected);
        
            Menu::Pop();
        }
        
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

void MessageSend::SetEntity(id_t entity) {
    this->entity_id = entity;
}

void MessageSend::SetMessageType(uint32_t type) {
    this->message_type = type;
}

void MessageSend::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        if (Entity* entity = Entity::Find(entity_id); !entity) {
            GUI::Text(FONT_TEXT, "No entity available.");
        } else {
            // Send [type] to [name] parameter [none|int|uint|float] [entry] .. send
            GUI::Text(FONT_TEXT, "Send ");
            GUI::TextBox(Message::GetName(message_type), 100);
            if (GUI::Button("(?)")) {
                auto select = new MessageTypeSelection([=, this](auto p){this->SetMessageType(p);});
                Menu::Push(select);
            }
            GUI::Text(FONT_TEXT, " to ");
            if (entity->GetName()) {
                GUI::TextBox(entity->GetName(), 100);
            } else {
                GUI::TextBox(std::to_string(entity->GetID()).c_str(), 100);
            }
            GUI::Text(FONT_TEXT, " with a parameter of ");
            GUI::RadioButton(0, parameter_type, "none");
            GUI::RadioButton(1, parameter_type, "int");
            GUI::RadioButton(2, parameter_type, "name");
            GUI::RadioButton(3, parameter_type, "float");
            GUI::TextBox("", 100);
            if (GUI::Button("Send!")) {
                Message msg;
                msg.type = message_type;
                msg.sender = 0;
                msg.receiver = entity->GetID();
                switch (parameter_type) {
                    default:
                        msg.data = nullptr;
                }
                Message::Send(msg);
            }
        }
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

}