// This file is mostly for testing.
// If you're using this as a library, then you need to implement your own main() function.

#include <iostream>

#define NEKADEEE

#ifdef NEKADEEE

#include <framework/core.h>
#include <framework/async.h>
#include <framework/ui.h>
#include <framework/entity.h>
#include <framework/worldcell.h>
#include <framework/event.h>
#include <framework/message.h>
#include <framework/language.h>
#include <physics/physics.h>
#include <audio/audio.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>

#include <glfw3.h>

#include <entities/crate.h>
#include <entities/staticworldobject.h>
#include <entities/player.h>

#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>
#include <components/lightcomponent.h>

#include <components/spritecomponent.h>
#include <components/particlecomponent.h>
#include <components/audiocomponent.h>

#include <components/controllercomponent.h>

#include <components/triggercomponent.h>

#include <extensions/menu/menu.h>
#include <extensions/camera/camera.h>

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;

int main() {
    //std::cout << std::filesystem::current_path() << std::endl;

    // register the entity types, so that they can be loaded from level files
    Entity::Register("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::Register("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});

    Core::Init();           // core init should always be first
    UI::Init();
    Physics::Init(); // optional, but needed for StaticWorldObject, Crate and Player entities
    Render::Init();         // render init must always come after the ui inited
    Async::Init();          // async init must always come after render init
    Audio::Init();
    GUI::Init();

    // any kind of material or model loading must happen after both ui and render are inited
    // TODO: move these into Render::Init()
    Material::SetErrorMaterial(PoolProxy<Material>::New(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(PoolProxy<Model>::New(UID("errorstatic")));

    Ext::Menu::Init();
    Ext::Camera::Init();

    // load all of the language strings
    LoadText("data/lv.lang");

    // texture info stuff
    Material::LoadMaterialInfo("data/texture.list");

    PoolProxy<Animation>::New(UID("mongus"));
    PoolProxy<Animation>::New(UID("turtle"));
    PoolProxy<Animation>::New(UID("bingus_idle"));
    Animation::LoadAll();





    // loading the demo level
    auto demo = PoolProxy<WorldCell>::New();
    demo->SetName(UID("demo_mov"));
    demo->LoadFromDisk();
    //demo->Load();


    // create the player entity
    Player player;
    player.SetLocation(vec3(0.0f, 1.0f, 0.0f));
    player.Load();
    
    Ext::Camera::Camera camera;
    camera.SetMouselook(true);
    camera.SetRotateFollowing(true);
    camera.SetFollowingOffset({0.0f, 0.5f, 0.0f});
    camera.SetFollowing(&player);
    
    Ext::Camera::SetCamera(&camera);

    // create the createdwdww model
    RenderComponent* binguser = PoolProxy<RenderComponent>::New();
    binguser->SetModel(UID("bingus"));
    binguser->Init();
    binguser->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    binguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create the mongus model
    RenderComponent* monguser = PoolProxy<RenderComponent>::New();
    monguser->SetModel(UID("mongus"));
    monguser->SetPose(poseList.begin().ptr);
    monguser->Init();
    monguser->UpdateLocation(glm::vec3(0.0f, 10.0f, 0.0f));
    monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    LightComponent* lit = PoolProxy<LightComponent>::New();
    lit->Init();
    lit->UpdateColor(glm::vec3(1.0f, 0.0f, 1.0f));
    lit->UpdateDistance(100.0f);

    // create the animation player for the mongus model
    ArmatureComponent* monguser_armature = PoolProxy<ArmatureComponent>::New();
    monguser_armature->SetModel(UID("mongus"));
    monguser_armature->Init();
    monguser_armature->PlayAnimation(UID("Run"), 100, 1.0f, 2.0f);
    monguser_armature->SetOnAnimationFinishCallback([](ArmatureComponent* comp, name_t name) { std::cout << "ANIMATION " << name << " IS FINISH!" << std::endl; });

    // link the mongus model and his animation player
    monguser->SetPose(monguser_armature->GetPosePtr());
    
    // turn on physics drawing
    //DRAW_PHYSICS_DEBUG = true;
    
    std::cout << "DCIKS" << std::endl;
    
    auto tolet_sprite = new Sprite(UID("poland"));
    //tolet_sprite->SetMaterial(Material::Find(UID("poland")));
    //tolet_sprite->AutogenTiledFrames(0, 0, 40, 40, 6, 24, 15.0f, 1.0f);
    
    auto tolet_spinner = PoolProxy<SpriteComponent>::New();
    tolet_spinner->SetSprite(tolet_sprite);
    tolet_spinner->UpdateLocation(glm::vec3(3.0f, 1.2f, -1.0f));
    tolet_spinner->Init();
    tolet_spinner->Play();
    
    auto tolet_emitter = PoolProxy<ParticleComponent>::New();
    tolet_emitter->SetSprite(tolet_sprite);
    tolet_emitter->UpdateLocation(glm::vec3(0.0f, 1.2f, -7.0f));
    tolet_emitter->Init();
    
    auto derp = Audio::Sound::Find(UID("apelsin"));
    derp->Load();
    
    auto derp_player = PoolProxy<AudioComponent>::New();
    derp_player->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    derp_player->SetSound(derp);
    derp_player->SetRepeating(true);
    derp_player->Init();
    
    //derp_player->Play();

    
    //auto crate_ent = Entity::Find(UID("estijs"));
    
    while(!EXIT){
        Core::Update();
        UI::Update();

        if (UI::INPUT_STATE == STATE_DEFAULT) {
            //glm::vec3 player_head;
            //player.GetLocation(player_head);
            //player_head += glm::vec3(0.0f, 0.5f, 0.0f);
            //Render::CAMERA_POSITION = player_head;
            //Audio::SetListenerPosition(player_head);
            //Audio::SetListenerOrientation(Render::CAMERA_ROTATION);
        }
        
        //time_of_day += 0.001f;
        SetSun(0.8f); // this sets the ambient lighting
        
        static int tick = 0;
        tick++;
        
        //if (tick % 4 == 0) {
        //    auto aaa = Entity::FindByName(UID("muca"));
        //    derp_player->UpdateLocation(aaa->GetLocation());
        //}
        
        if (tick == 300) {
            //auto aaa = Entity::FindByName(UID("estijs"));
            //delete aaa;
        }
        
        if (UI::PollKeyboardKey(UI::KEY_Y)) {
            tolet_emitter->EmitParticle({{0.0f, 3.0f, 0.0f}, {0.0f, 0.25f, 0.5f}, 15});
        }
        
        //if (tick > 300 && tick % 100 == 0) {
        //    auto aaa = Entity::FindByName(UID("estijs"));
        //    aaa->SetLocation(player.GetLocation() + vec3(0.0f, 5.0f, 0.0f));
        //}
        
        //trigga->SetRotation(player.GetRotation());
        //trigga->SetLocation(player.GetLocation() + (player.GetRotation() * vec3(0.0f, 0.0f, -2.0f)));
        
        // this will make the light spin
        vec3 litloc = glm::vec3(cos(((float)tick) / 60.0f) * 7.0f, 1.25f ,sin(((float)tick) / 60.0f) * 7.0f);
        quat litrot;
        QuatLookAt(litrot, litloc, vec3(0.0f, 1.8f, 0.0f));
        binguser->UpdateLocation(litloc);
        binguser->UpdateRotation(litrot);
        lit->UpdateLocation(litloc);
        
        // this makes the mongus model bob up and down
        monguser->UpdateLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.25f, 3.0f));
        
        //SetText("hello i have begonis", 10.0f, 10.0f, 1.2f, 300.0f, false, false, 1, COLOR_PINK);
        //SetText("begonis bepis", 10.0f, 40.0f, 1.0f, 300.0f, false, false, 0, COLOR_PINK);
        
        if (UI::PollKeyboardKey(UI::KEY_A)) {
            camera.SetTilt(0.1f);
        } else if (UI::PollKeyboardKey(UI::KEY_D)) {
            camera.SetTilt(-0.1f);
        } else {
            camera.SetTilt(0.0f);
        }
        
        if (UI::PollKeyboardKey(UI::KEY_W) || UI::PollKeyboardKey(UI::KEY_S) ||
            UI::PollKeyboardKey(UI::KEY_A) || UI::PollKeyboardKey(UI::KEY_D)) {
            camera.SetBobbing(1.0f);
        } else {
            camera.SetBobbing(0.0f);
        }
        
        Ext::Camera::Update();
        
        GUI::Begin();
        Ext::Menu::DebugMenu();
        Ext::Menu::EscapeMenu();
        
        std::string tickstr = std::to_string(GetTick());
        std::string tickstr2 = std::to_string(GetTickTime());
        //std::string tickstr3 = std::to_string(trigga->Poll().size());
        
        GUI::Text(tickstr.c_str(), 2, Core::GUI::TEXT_CENTER); GUI::FrameBreakLine();
        GUI::Text(tickstr2.c_str(), 2, Core::GUI::TEXT_CENTER); GUI::FrameBreakLine();
        //GUI::Text(tickstr3.c_str(), 2, Core::GUI::TEXT_CENTER);
        
        //{
        //    auto p = trigga->Poll();
        //    for (auto& t : p) {
        //        Render::AddLineMarker(t.point, Render::COLOR_CYAN);
        //    }
        //}
        
        /*
        GUI::Frame(Core::GUI::FRAME_CENTER, 640.0f, 480.0);
        GUI::Frame(Core::GUI::FRAME_TOP, 128.0f);
        GUI::FillFrame(0.0f, 0.0f, 256.0f, 64.0f, Render::COLOR_WHITE, 3);
        
        static bool helptext = false;
        if(GUI::SmallButton(GUI::BUTTON_QUESTION)) helptext = !helptext;
        if(helptext) GUI::Text("Amogus!", 1);
        
        GUI::Frame(Core::GUI::FRAME_BOTTOM, 20.0f);
        
        static float pad = 650.0f; pad -= 2.0f;
        if (pad < -170.0f) pad = 650.0f;
        GUI::GlyphText("KAD BUUS EEST", 2, pad, 0.0f, 2.0f, Render::COLOR_RED);
        
        GUI::EndFrame();
        GUI::EndFrame();
        GUI::Frame(Core::GUI::FRAME_BOTTOM, 480.0f - 128.0f);
        GUI::Frame(Core::GUI::FRAME_LEFT, 150.0f);
        GUI::FillFrame(104.0f, 108.0f, 12.0f, 12.0f, Render::COLOR_WHITE, 0);
    
        GUI::EndFrame();
        GUI::Frame(Core::GUI::FRAME_RIGHT, 640.0f - 150.0f);
        GUI::FillFrame(104.0f, 88.0f, 12.0f, 12.0f, Render::COLOR_WHITE, 0);
        
        GUI::FrameBorder();
        static float scrll = 0.0f, heigh = 0.0f;
        GUI::ScrollBar(scrll, heigh);

        
        
        static char text_box[25] = "izmainaamais teksts!";
        static uint32_t text_colo = 0;
        static uint32_t text_publ = 0;
        static char text_disp[25] = "izmainaamais teksts!";
        static char text_caps[25] = "IZMAINAAMAIS TEKSTS!";
        static bool do_caps = false;
        if(GUI::Button("PUBIBEET")){
            text_publ = text_colo;
            for(int i = 0; text_box[i-1] != '\0' || i == 0; i++){
                text_disp[i] = text_box[i];
                text_caps[i] = toupper(text_box[i]);
            }
        }
        GUI::TextBox(text_box, sizeof(text_box));
        char const* tixs[] = { "BALTS", "ROZAA", "rieksti!"};
        GUI::DropdownBox(tixs, 3, text_colo); GUI::FrameBreakLine();
        
        GUI::CheckBox(do_caps); GUI::Text("lielie burti", 1); GUI::FrameBreakLine();
        
        static uint32_t orentaation = 0;
        
        GUI::RadioButton(orentaation, 0);GUI::Text("kreisi", 1);GUI::FrameBreakLine();
        GUI::RadioButton(orentaation, 1);GUI::Text("centrs", 1);GUI::FrameBreakLine();
        GUI::RadioButton(orentaation, 2);GUI::Text("labi", 1);;;GUI::FrameBreakLine();
        GUI::RadioButton(orentaation, 3);GUI::Text("slikti", 1);GUI::FrameBreakLine();
        
        
        char* disptext = do_caps ? text_caps : text_disp;
        if (text_publ == 0) {
            GUI::Text(disptext, 1, (GUI::orientation)orentaation, Render::COLOR_WHITE);
        } else if (text_publ == 1) {
            GUI::Text(disptext, 1, (GUI::orientation)orentaation, Render::COLOR_PINK);
        } else {
            GUI::Text(disptext, 1, (GUI::orientation)orentaation, Render::COLOR_CYAN);
        }
        
        static bool give_text = false;
        GUI::FrameBreakLine();
        GUI::CheckBox(give_text); GUI::Text("dot tekstu dibenaa", 1); GUI::FrameBreakLine();
        
        if(give_text) {
            for (int i = 0; i < 20; i++) GUI::FrameBreakLine();
            GUI::Text("es esmu teksts dibenaa", 2); GUI::FrameBreakLine();
        }
        
        GUI::EndFrame();*/
        
        
        
        GUI::End();
        GUI::Update();
        
        Audio::Update();
        
        // this loads the models and textures into video memory
        Async::ResourceLoader2ndStage();
        Async::FinishResource();


        Event::Dispatch();
        Message::Dispatch();
        
        WorldCell::Loader::LoadCells();
        
        ControllerComponent::UpdateAll();

        Physics::Update();

        Render::UpdateArmatures();
        Render::Render();

        UI::EndFrame();
    }

    Async::Yeet();
    Audio::Uninit();
    UI::Uninit();
}

#endif
#ifndef NEKADEEE

#include <framework/core.h>
#include <framework/file.h>

#include <framework/logging.h>

using namespace Core;

#include <templates/macros.h>

TRAM_SDK_REGISTER_EVENT_DATATYPE(Event::KEYDOWN, float)

int main() {
    Core::Init();
    
    std::cout << Core::ENGINE_VERSION << std::endl;
    std::cout << "Hello World!" << std::endl;
    
    uint32_t NEW_SYSTEM = System::Register("Legonis", "LEGON");
    Log("System with ID {} is called {}, or {} for short.", NEW_SYSTEM, System::GetName(NEW_SYSTEM), System::GetShortName(NEW_SYSTEM));
    
    //Log("Float version: {}, whereas Int version is: {}. Fascinating!", RetrieveData<float>(Event::KEYUP), RetrieveData<int>(Event::KEYDOWN));
    Event vevent {
        .type = Event::KEYDOWN,
        .data = new float(420)
    };
    
    Log("This my log: {}", Event::RetrieveData<Event::KEYDOWN>(vevent));
}

#endif