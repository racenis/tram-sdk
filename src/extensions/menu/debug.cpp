// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/menu/debug.h>

#include <render/scene.h>
#include <physics/api.h>
#include <render/api.h>
#include <components/render.h>
#include <components/audio.h>
#include <components/animation.h>
#include <components/controller.h>
#include <components/light.h>
#include <framework/entity.h>
#include <framework/logging.h>
#include <framework/settings.h>
#include <framework/worldcell.h>
#include <framework/stats.h>
#include <framework/script.h>

#include <cstring>
#include <deque>

namespace tram::Ext::Menu {

std::deque<std::string> console_logs;
    
struct Intercept {
    virtual void Display() = 0;
    virtual ~Intercept() = default;
    
    unsigned int time;
};
std::vector<Intercept*> intercepts;

struct MessageIntercept : Intercept {
    std::string receiver;
    std::string sender;
    std::string type;
    std::string value;
    
    void Display() {
        GUI::Text("MSG");
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 40);
        
        GUI::Text(type.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 80);
        
        GUI::Text(sender.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 80);
        
        GUI::Text(receiver.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 80);
        
        GUI::Text(value.c_str());
        
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();

        /*GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 40);
        
        GUI::Text(type.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 90);
        
        GUI::Text(sender.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 50);
        
        GUI::Text(receiver.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 50);
        
        GUI::Text(value.c_str());
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();*/
    }
};

struct EventIntercept : Intercept {
    std::string emitter;
    std::string type;
    std::string subtype;
    std::string value;
    
    void Display() {
        
        GUI::Text("EVT");
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 40);
        
        GUI::Text(type.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 80);
        
        GUI::Text(emitter.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 80);
        
        //GUI::Text(receiver.c_str());
        GUI::Text(subtype.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 80);
        
        GUI::Text(value.c_str());
        
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();
        
        //GUI::Text((std::string("event ") + std::to_string(time)).c_str());
        
        /*GUI::Text("EVT");
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 40);
        
        GUI::Text(type.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 90);
        
        GUI::Text(subtype.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 40);
        
        GUI::Text(emitter.c_str());
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 70);
        
        GUI::Text(value.c_str());
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();*/
        
        
    }
};

struct LogIntercept : Intercept {
    std::string message;
    
    void Display() {
        GUI::Text("LOG");
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 40);
        
        GUI::Text(message.c_str());
        GUI::PopFrame();
    }
};



void InitCallbacks() {
    Message::SetInterceptCallback([](const Message& msg) {
        MessageIntercept* intercept = new MessageIntercept;
        
        intercept->type = (std::string)Message::GetName(msg.type);
        intercept->value = std::to_string((long long)msg.data);
        
        switch (msg.type) {
                case Message::SELECT:
                    intercept->time = GetTick();
                    break;
                default:
                    intercept->time = GetTick() + 60;
            }
        
        if (!msg.sender) {
            intercept->sender = "GOD";
        } else if (Entity* sender = Entity::Find(msg.sender); sender) {
            if (sender->GetName()) {
                intercept->sender = (std::string)sender->GetName();
            } else {
                intercept->sender = sender->GetID();
            }
        } else {
            intercept->sender = "DEAD";
        }
        
        if (Entity* receiver = Entity::Find(msg.receiver); receiver) {
            if (receiver->GetName()) {
                intercept->receiver = (std::string)receiver->GetName();
            } else {
                intercept->receiver = receiver->GetID();
            }
        } else {
            intercept->receiver = "DEAD";
        }
        
        intercepts.push_back(intercept);
    });
    
    SetDisplayLogCallback([](int time, const char* text) {
        LogIntercept* intercept = new LogIntercept;
        
        intercept->message = text;
        intercept->time = time;
        
        intercepts.push_back(intercept);
    });
    
    SetConsoleLogCallback([](int time, const char* text) {
        if (console_logs.size() > 10) console_logs.pop_back();
        console_logs.push_front(text);
    });
}

void UpdateCallbacks() {
    std::vector<Intercept*> filtered;
    for (auto intercept : intercepts) {
        bool expired = GetTick() > intercept->time;
        
        if (expired) {
            delete intercept;
        } else {
            filtered.push_back(intercept);
        }
    }
    std::swap(filtered, intercepts);
    
    static event_t last_callback = 0;
    for (event_t i = last_callback; i < Event::GetLast(); i++) {
        last_callback = Event::GetLast();
        Event::AddListener(i, [](Event& event) {
            EventIntercept* intercept = new EventIntercept;
        
            intercept->subtype = std::to_string(event.subtype);
            intercept->type = (std::string)Event::GetName(event.type);
            intercept->value = std::to_string((long long)event.data);
            
            switch (event.type) {
                case Event::KEYPRESS:
                case Event::TICK:
                case Event::FRAME:
                case Event::CURSORPOS:
                case Event::LOOK_AT:
                case Event::SELECTED:
                    intercept->time = GetTick();
                    break;
                default:
                    intercept->time = GetTick() + 60;
            }
            
            
            if (!event.poster) {
                intercept->emitter = "GOD";
            } else if (Entity* emitter = Entity::Find(event.poster); emitter) {
                if (emitter->GetName()) {
                    intercept->emitter = (std::string)emitter->GetName();
                } else {
                    intercept->emitter = emitter->GetID();
                }
            } else {
                intercept->emitter = "DEAD";
            }
            
            intercepts.push_back(intercept);
        });
    }
}

bool DebugMenu::intercept_enabled = false;
bool DebugMenu::statistics_enabled = false;
InterceptMenu* DebugMenu::intercept_menu = nullptr;
StatisticsMenu* DebugMenu::statistics_menu = nullptr;

void DebugMenu::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
    
        if (GUI::Button("Worldcells")) {
            std::vector<std::string> cells;
            for (auto& cell : PoolProxy<WorldCell>::GetPool()) {
                cells.push_back(cell.GetName());
            }
            
            auto props = new WorldCellProperties;
            auto picks = new ListSelection([=, this](auto p){props->SetPicked(&PoolProxy<WorldCell>::GetPool()[p]);}, cells);
            
            Menu::Push(props);
            Menu::Push(picks);
        }
        
        if (GUI::Button("Select entity")) {
            auto props = new EntityProperties();
            auto picks = new EntityPicker([=](id_t picked){props->SetPicked(picked);});
            
            Menu::Push(props);
            Menu::Push(picks);
        }
        
        if (GUI::Button("Emit event")) {
            auto emit = new EventEmit;
            
            Menu::Push(emit);
        }
        
        if (GUI::Button("Options"))  {
            auto options = new Options;
            
            Menu::Push(options);
        }
        
        if (GUI::Button("Console"))  {
            auto console = new Console;
            
            Menu::Push(console);
        }
    
        if (GUI::CheckBox(intercept_enabled, "Intercept ")) {
            if (intercept_enabled) {
                intercept_menu = new InterceptMenu;
                if (statistics_menu) intercept_menu->SetOffset(24);
                Menu::Add(intercept_menu);
            } else {
                Menu::Remove(intercept_menu);
                intercept_menu = nullptr;
            }
        }
        
        if (GUI::CheckBox(statistics_enabled, "Statistics ")) {
            if (statistics_enabled) {
                statistics_menu = new StatisticsMenu;
                if (intercept_menu) intercept_menu->SetOffset(24);
                Menu::Add(statistics_menu);
            } else {
                Menu::Remove(statistics_menu);
                if (intercept_menu) intercept_menu->SetOffset(0);
                statistics_menu = nullptr;
            }
        }
        
        static bool draw_debug = false;
        if (GUI::CheckBox(draw_debug, "Physics ")) {
            Physics::API::DrawDebug(draw_debug);
        }
        
        static bool noclip = false;
        if (GUI::CheckBox(noclip, "Noclip ")) {
            Message msg;
            msg.sender = 0;
            msg.receiver = Entity::Find("player")->GetID();
            msg.type = noclip ? Message::SET_FLAG_ON : Message::SET_FLAG_OFF;
            msg.data_value = Message::AllocateData<Value>(UID("noclip"));
            Message::Send(msg);
        }
        
        
    GUI::PopFrame();
    GUI::PopFrame();
}

void EntityProperties::SetPicked(id_t entity) {
    this->entity = entity;
}

void EntityProperties::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        if (Entity* ptr = Entity::Find(entity); !ptr) {
            GUI::Text("No entity available.");
        } else {
            GUI::Text("Entity ID ");
            GUI::TextBox(std::to_string(ptr->GetID()).c_str(), 50);
            GUI::Text(" Name ");
            GUI::TextBox(ptr->GetName(), 100);
            GUI::Text(" Distance ");
            float dist = glm::distance(Render::GetViewPosition(), ptr->GetLocation());
            int dist0 = dist;
            int dist1 = dist*10 - dist0*10;
            GUI::TextBox((std::to_string(dist0) + "." + std::to_string(dist1)).c_str(), 50);
            GUI::Text(" Type ");
            GUI::TextBox(ptr->GetType(), 75);
            
            if (GUI::Button("Load")) {
                ptr->Load();
            }
            
            if (GUI::Button("Unload")) {
                ptr->Unload();
            }
            
            if (GUI::Button("Signals")) {
                auto signals = new SignalMenu;
                signals->SetEntity(ptr->GetID());
                Menu::Push(signals);
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
    GUI::FillFrame(GUI::WIDGET_BUTTON);
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
                GUI::Text(text.c_str());
                
                if (GUI::ClickHandled()) {
                    callback(comp->GetParent()->GetID());
                    Menu::Pop();
                    return;
                }
            } else {
                GUI::Text("No entity found for RenderComponent.");
            }
            
            UI::SetCursor(UI::CURSOR_CLICK);
            
        } else {
            GUI::Text("No pick available.");
            UI::SetCursor(UI::CURSOR_DEFAULT);
        }
        
        
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

ListSelection::ListSelection(std::function<void(uint32_t)> callback, std::vector<std::string> list) {
    this->callback = callback;
    this->list = list;
}

void ListSelection::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34 * 2);
    GUI::PushFrameRelative(GUI::FRAME_LEFT, 200);
    
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        
        uint32_t selected = 255;
        for (uint32_t i = 0; i < list.size(); i++) {
            if (GUI::Button(list[i].c_str())) {
                selected = i;
            }
            
            GUI::NewLine();
        }
        
        if (selected != 255) {
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
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        if (Entity* entity = Entity::Find(entity_id); !entity) {
            GUI::Text("No entity available.");
        } else {
            // Send [type] to [name] parameter [none|int|uint|float] [entry] .. send
            GUI::Text("Send ");
            GUI::TextBox(Message::GetName(message_type), 100);
            if (GUI::Button("(?)")) {
                std::vector<std::string> messages;
                for (message_t i = 0; i < Message::GetLast(); i++) {
                    messages.push_back(Message::GetName(i));
                }
                auto select = new ListSelection([=, this](auto p){this->SetMessageType(p);}, messages);
                Menu::Push(select);
            }
            GUI::Text(" to ");
            if (entity->GetName()) {
                GUI::TextBox(entity->GetName(), 100);
            } else {
                GUI::TextBox(std::to_string(entity->GetID()).c_str(), 100);
            }
            GUI::Text(" with a parameter of ");
            GUI::RadioButton(0, parameter_type, "none");
            GUI::RadioButton(1, parameter_type, "int");
            GUI::RadioButton(2, parameter_type, "name");
            GUI::RadioButton(3, parameter_type, "float");
            GUI::TextBox(parameter_string, 32, parameter_type != 0, 100);
            if (GUI::Button("Send!")) {
                Message msg;
                msg.type = message_type;
                msg.sender = 0;
                msg.receiver = entity->GetID();
                switch (parameter_type) {
                    case 0:
                        msg.data_value = Event::AllocateData<Value>((int32_t)atoi(parameter_string));
                        break;
                    case 1:
                        msg.data_value = Event::AllocateData<Value>((float)atof(parameter_string));
                        break;
                    case 2:
                        msg.data_value = Event::AllocateData<Value>((name_t)parameter_string);
                        break;
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

void InterceptMenu::Display() {
    int lines = 1;
    int total_lines = intercepts.size();
    if (total_lines > 20) total_lines = 20;
    
    GUI::PushFrameRelative(GUI::FRAME_BOTTOM_INV, offset);
    GUI::PushFrameRelative(GUI::FRAME_BOTTOM, total_lines * 16 + 10);
    GUI::PushFrameRelative(GUI::FRAME_RIGHT, 400);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        for (auto intercept : intercepts) {
            GUI::PushFrameRelative(GUI::FRAME_BOTTOM, lines * 16);
                intercept->Display();
            GUI::PopFrame();
            
            if (++lines > 20) break;
        }
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

void WorldCellProperties::SetPicked(WorldCell* cell) {
    this->cell = cell;
}

void WorldCellProperties::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        if (!cell) {
            GUI::Text("No cell available.");
        } else {
            bool loaded = cell->IsLoaded();
            bool interior = cell->IsInterior();
            bool interior_lighting = cell->HasInteriorLighting();
            bool debug_draw = cell->IsDebugDraw();
            
            GUI::Text("Worldcell ");
            GUI::TextBox(cell->GetName(), 100);
            GUI::Text(" Entities ");
            GUI::TextBox(std::to_string(cell->GetEntityCount()).c_str(), 50);
            if (GUI::Button("(view)")) {
                std::vector<std::string> entities;
                for (auto entity : cell->GetEntities()) {
                    entities.push_back((std::string)entity->GetType() + " | " + (std::string)entity->GetName());
                }
                
                // possible error here:
                // worldcell entity list might change between entity list name
                // generation and callback
                // TODO: fix
                // possible fix -> instead of ListSelection just having a single
                // vector of strings, it would also have a second vector with
                // data values
                auto props = new EntityProperties;
                auto picks = new ListSelection([=, this](uint32_t p){props->SetPicked(cell->GetEntities()[p]->GetID());}, entities);
                
                Menu::Push(props);
                Menu::Push(picks);
            }
            if (GUI::CheckBox(loaded, "Is loaded ")) {
                if (loaded) {
                    cell->Load();
                } else {
                    cell->Unload();
                }
            }
            if (GUI::CheckBox(interior, "Is interior ")) {
                cell->SetInterior(interior);
            }
            if (GUI::CheckBox(interior_lighting, "Has interior lighting ")) {
                cell->SetInteriorLights(interior_lighting);
            }
            if (GUI::CheckBox(interior, "Debug draw ")) {
                cell->SetDebugDraw(debug_draw);
            }
        }
        /*if (Entity* ptr = Entity::Find(entity); !ptr) {
            GUI::Text("No entity available.");
        } else {
            GUI::Text("Entity ID ");
            GUI::TextBox(std::to_string(ptr->GetID()).c_str(), 50);
            GUI::Text(" Name ");
            GUI::TextBox(ptr->GetName(), 100);
            GUI::Text(" Distance ");
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
        }*/
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

void StatisticsMenu::Display() {
    GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 24);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 2);

        GUI::PushFrameRelative(GUI::FRAME_LEFT, 100);
        GUI::FillFrame(GUI::WIDGET_REVERSE_WINDOW);
        GUI::PushFrameRelative(GUI::FRAME_INSET, 2);
            GUI::Text("Tramway SDK");
        GUI::PopFrame();
        GUI::PopFrame();

        char render_str1[16];
        char render_str2[24];
        char render_str3[16];
        char physics_str[24];
        
        uint32_t vram_kb = Stats::GetStat(Stats::RESOURCE_VRAM) / 1024;
        uint32_t vram_mb_w = vram_kb / 1000;
        uint32_t vram_mb_f = vram_kb % 1000;
        
        snprintf(render_str1, 16, "Render %.2fms", 1000 * Stats::GetStatAverage(System::RENDER));
        snprintf(render_str2, 24, "VRAM %i %i KB", vram_mb_w, vram_mb_f);
        snprintf(render_str3, 16, "Draw %i", (int)Stats::GetStat(Stats::RESOURCE_DRAWCALL));
        snprintf(physics_str, 24, "Physics %.2fms", 1000 * Stats::GetStatAverage(System::PHYSICS));

        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 100);
        GUI::PushFrameRelative(GUI::FRAME_LEFT, 275);
        GUI::FillFrame(GUI::WIDGET_REVERSE_WINDOW);
        GUI::PushFrameRelative(GUI::FRAME_INSET, 2);
            GUI::Text(render_str1);
            
            
            GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 105);
                GUI::Text(render_str3);
            GUI::PopFrame();
            
            GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 150);
                GUI::Text(render_str2, GUI::TEXT_RIGHT);
            GUI::PopFrame();
            
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();
        
        GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 375);
        GUI::PushFrameRelative(GUI::FRAME_LEFT, 100);
        GUI::FillFrame(GUI::WIDGET_REVERSE_WINDOW);
        GUI::PushFrameRelative(GUI::FRAME_INSET, 2);
            GUI::Text(physics_str);
        GUI::PopFrame();
        GUI::PopFrame();
        GUI::PopFrame();

    GUI::PopFrame();
    GUI::PopFrame();
}

void EventEmit::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        // Emit [event] subtype [type] with parameter of ''
        GUI::Text("Emit ");
        GUI::TextBox(Event::GetName(event_type), 100);
        if (GUI::Button("(?)")) {
            std::vector<std::string> events;
            for (message_t i = 0; i < Event::GetLast(); i++) {
                events.push_back(Event::GetName(i));
            }
            auto select = new ListSelection([=, this](auto p){this->SetEventType(p);}, events);
            Menu::Push(select);
        }
        GUI::Text(" subtype ");
        if (GUI::TextBox(subtype_string, 8, true, 100)) {
            if (strlen(subtype_string)) {
                subtype = atoi(subtype_string);
                snprintf(subtype_string, 8, "%i", subtype);
            }
        }
        GUI::Text(" with a parameter of ");
        GUI::RadioButton(0, parameter_type, "none");
        GUI::RadioButton(1, parameter_type, "int");
        GUI::RadioButton(2, parameter_type, "name");
        GUI::RadioButton(3, parameter_type, "float");
        GUI::TextBox(parameter_string, 32, parameter_type != 0, 100);
        if (GUI::Button("Emit!")) {
            Event evt;
            evt.type = event_type;
            evt.subtype = subtype;
            evt.poster = 0;
            
            switch (parameter_type) {
                case 0:
                    evt.data_value = Event::AllocateData<Value>((int32_t)atoi(parameter_string));
                    break;
                case 1:
                    evt.data_value = Event::AllocateData<Value>((float)atof(parameter_string));
                    break;
                case 2:
                    evt.data_value = Event::AllocateData<Value>((name_t)parameter_string);
                    break;
                default:
                    evt.data = nullptr;
            }
            Event::Post(evt);
        }
        
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();    
    
}

void SignalMenu::Display() {
    Entity* entity = Entity::Find(entity_id);
    SignalTable* signals = entity ? entity->GetSignalTable() : nullptr;
    
    uint32_t signal_offset = signals ? 10 + 16 * signals->signal_count : 24;
    
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34 * 2);
    GUI::PushFrameRelative(GUI::FRAME_LEFT, 500);
    GUI::PushFrameRelative(GUI::FRAME_TOP, signal_offset);
    
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        
        if (!entity)  {
            GUI::Text("Entity unavailable.");
        } else if (!signals) {
            GUI::Text("Entity has no signals.");
        } else {
            for (uint32_t i = 0; i < signals->signal_count; i++) {
                Signal& signal = signals->signals[i];
                
                GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 24 * i);
                
                    
                
                    GUI::Text(Signal::GetName(signal.type));
                    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 100);
                    
                    GUI::Text(Message::GetName(signal.message_type));
                    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 100);
                    
                    GUI::Text(std::to_string(signal.delay).c_str());
                    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 60);
                    
                    GUI::Text(std::to_string(signal.limit).c_str());
                    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 40);
                    
                    GUI::Text(signal.receiver);
                    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 100);
                    
                    if (Value* data = (Value*)signal.data; data) {
                        switch (data->GetType()) {
                            case TYPE_INT32:
                                GUI::Text(std::to_string(data->GetInt()).c_str());
                                break;
                            case TYPE_FLOAT32:
                                GUI::Text(std::to_string(data->GetFloat()).c_str());
                                break;
                            case TYPE_NAME:
                                GUI::Text((name_t)*data);
                                break;
                            default:
                                break;
                        }
                    }
                    
                    GUI::PopFrame();
                    GUI::PopFrame();
                    GUI::PopFrame();
                    GUI::PopFrame();
                    GUI::PopFrame();
                
                
                
                
                
                
                
                
                
                
                
                
                
                
                
                
                
                
                
                
                GUI::PopFrame();
            }
        }
        
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}



void Options::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);

        bool renderer_debug = Render::API::IsDebugMode();
        
        bool audio_debug = AudioComponent::IsDebugInfoDraw();
        bool sound_debug = AudioComponent::IsSourceDraw();
        
        bool anim_debug = AnimationComponent::IsDebugInfoDraw();
        bool light_debug = LightComponent::IsLightDraw();
        bool controller_debug = ControllerComponent::IsDebugInfoDraw();
        
        GUI::CheckBox(renderer_debug, "Renderer Debug ");
        GUI::CheckBox(audio_debug, "Audio Debug ");
        GUI::CheckBox(sound_debug, "Draw AudioComponent ");
        GUI::CheckBox(anim_debug, "Animation Debug ");
        GUI::CheckBox(light_debug, "Light Debug ");
        GUI::CheckBox(controller_debug, "Controller Debug ");
        
        
        Render::API::SetDebugMode(renderer_debug);
        AudioComponent::SetDebugInfoDraw(audio_debug);
        AudioComponent::SetSourceDraw(sound_debug);
        AnimationComponent::SetDebugInfoDraw(anim_debug);
        LightComponent::SetLightDraw(light_debug);
        ControllerComponent::SetDebugInfoDraw(controller_debug);
        
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

Console::Console() {
    text = new char[250];
    *text = '\0';
}

Console::~Console() {
    delete[] text;
}

void Console::Display() {
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 34);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 34);
    GUI::FillFrame(GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        
        static std::vector<std::string> console_history;
        
        static bool last_enter = false;
        static bool last_up = false;
        static bool last_down = false;
        
        const bool this_enter = !last_enter && UI::PollKeyboardKey(UI::KEY_ENTER);
        const bool this_up = !last_up && UI::PollKeyboardKey(UI::KEY_UP);
        const bool this_down = !last_down && UI::PollKeyboardKey(UI::KEY_DOWN);
    
        last_enter = UI::PollKeyboardKey(UI::KEY_ENTER);
        last_up = UI::PollKeyboardKey(UI::KEY_UP);
        last_down = UI::PollKeyboardKey(UI::KEY_DOWN);
    
        if (GUI::TextBox(text, 250, true, 400)) {
            GUI::SetSelectedText(text);
        };

        if (this_enter) {
            history_cursor = -1;
            console_history.push_back(text);
            
            value_t value = Script::Evaluate(text);
            
            //std::cout << TypeToString(value.GetType()) << " : " << value.ToString() <<std::endl;
            Log("{} : {}", TypeToString(value.GetType()), value.ToString());
            
            *text = '\0';
        }
        
        if (this_up) {
            if (history_cursor == -1 && console_history.size()) {
                history_cursor = (int)console_history.size() - 1;
            } else if (history_cursor > 0) {
                history_cursor--;
            }
            
            if (history_cursor != -1) strcpy(text, console_history[history_cursor].data());
        }
        
        if (this_down) {
            if (history_cursor != -1) {
                history_cursor++;
                
                if (history_cursor >= (int)console_history.size()) {
                    history_cursor = -1;
                    *text = '\0';
                }
            }
            
            if (history_cursor != -1) strcpy(text, console_history[history_cursor].data());
        }
            
        
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 68);
    GUI::SetColor(Render::COLOR_BLACK);
    for (auto& log : console_logs) {
        GUI::Text(log.c_str());
        GUI::NewLine();
    }
    GUI::PushFrameRelative(GUI::FRAME_INSET, 2);
        GUI::SetColor(Render::COLOR_WHITE);
        for (auto& log : console_logs) {
            GUI::Text(log.c_str());
            GUI::NewLine();
        }
        GUI::SetColor(Render::COLOR_BLACK);
    GUI::PopFrame();
    GUI::PopFrame();
}

}