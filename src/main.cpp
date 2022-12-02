#include <iostream>

#ifdef NEKADEEE

#include <core.h>
#include <async.h>
#include <ui.h>
#include <physics/physics.h>
#include <audio.h>

#include <gui.h>

#include <glfw3.h>

#include <entities/crate.h>
#include <entities/staticworldobject.h>
#include <entities/player.h>

#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>
#include <components/lightcomponent.h>

#include <components/spritecomponent.h>
#include <components/particlecomponent.h>
//#include <components/audiocomponent.h>

#include <components/controllercomponent.h>

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;

int main() {
    std::cout << ENGINE_VERSION << std::endl;
    //std::cout << std::filesystem::current_path() << std::endl;

    // register the entity types, so that they can be loaded from level files
    Entity::Register("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::Register("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});

    Core::Init();           // core init should always be first
    UI::Init();
    Physics::InitPhysics(); // optional, but needed for StaticWorldObject, Crate and Player entities
    Render::Init();         // render init must always come after the ui inited
    Async::Init();          // async init must always come after render init
    Audio::Init();

    // any kind of material or model loading must happen after both ui and render are inited
    Material::SetErrorMaterial(PoolProxy<Material>::New(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(PoolProxy<Model>::New(UID("errorstatic")));

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
    demo->SetName(UID("demo"));
    demo->LoadFromDisk();
    //demo->Load();


    // create the player entity
    Player player;
    player.SetLocation(0.0f, 1.0f, 0.0f);
    player.Load();



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
    monguser_armature->SetOnAnimationFinishCallback([](ArmatureComponent* comp, name_t name) { std::cout << "ANIMATION " << ReverseUID(name) << " IS FINISH!" << std::endl; });

    // link the mongus model and his animation player
    monguser->SetPose(monguser_armature->GetPosePtr());
    
    // turn on physics drawing
    //DRAW_PHYSICS_DEBUG = true;
    
    
    auto tolet_sprite = new Sprite;
    tolet_sprite->SetMaterial(Material::Find(UID("poland")));
    tolet_sprite->AutogenTiledFrames(0, 0, 40, 40, 6, 24, 15.0f, 1.0f);
    
    auto tolet_spinner = PoolProxy<SpriteComponent>::New();
    tolet_spinner->SetSprite(tolet_sprite);
    tolet_spinner->UpdateLocation(glm::vec3(0.0f, 1.2f, -2.0f));
    tolet_spinner->Init();
    tolet_spinner->Play();
    
    auto tolet_emitter = PoolProxy<ParticleComponent>::New();
    tolet_emitter->SetSprite(tolet_sprite);
    tolet_emitter->UpdateLocation(glm::vec3(0.0f, 1.2f, -2.0f));
    tolet_emitter->Init();
    
    //auto derp_player = PoolProxy<AudioComponent>::New();
    //derp_player->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    //derp_player->SetSound(&derp);
    //derp_player->SetRepeating(true);
    //derp_player->Init();
    
    //auto crate_ent = Entity::Find(UID("estijs"));
        
    while(!SHOULD_CLOSE){
        UI::Update();

        if (UI::INPUT_STATE == STATE_DEFAULT) {
            glm::vec3 player_head;
            player.GetLocation(player_head);
            player_head += glm::vec3(0.0f, 0.5f, 0.0f);
            Render::CAMERA_POSITION = player_head;
            Audio::SetListenerPosition(player_head);
            Audio::SetListenerOrientation(Render::CAMERA_ROTATION);
        }
        
        //time_of_day += 0.001f;
        SetSun(time_of_day); // this sets the ambient lighting
        
        static int tick = 0;
        tick++;
        
        
        if (tick == 300) {
            auto aaa = Entity::FindName(UID("estijs"));
            std::cout << "estijs: " << aaa << std::endl;
            delete aaa;
        }
        
        if (tick == 350) {
                        auto aaa = Entity::FindName(UID("estijs"));
            std::cout << "estijs: " << aaa << std::endl;
        }
        
        // this will make the light spin
        lit->UpdateLocation(glm::vec3(cos(((float)tick) / 60.0f) * 5.0f, 0.01 ,sin(((float)tick) / 60.0f) * 5.0f));
        
        // this makes the mongus model bob up and down
        monguser->UpdateLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.1f, 0.0f));
        
        SetText("hello i have begonis", 10.0f, 10.0f, 1.2f, 300.0f, false, false, 1, COLOR_PINK);
        SetText("begonis bepis", 10.0f, 40.0f, 1.0f, 300.0f, false, false, 0, COLOR_PINK);
        
        GUI::Begin();
        GUI::DebugMenu();
        GUI::EscapeMenu();
        GUI::End();
        
        Audio::Update();
        
        // this loads the models and textures into video memory
        Async::ResourceLoader2ndStage();
        Async::FinishResource();


        Event::Dispatch();
        Message::Dispatch();
        
        WorldCell::Loader::LoadCells();
        
        ControllerComponent::UpdateAll();

        // updates the physics world
        float phys_step = 1.0f / 60.0f;
        Physics::StepPhysics(phys_step);

        Render::UpdateArmatures();
        Render::Render();

        UI::EndFrame();
    }

    std::cout << "endframe 1" << std::endl;

    Async::Yeet();
    
    std::cout << "endframe 2" << std::endl;

    Audio::Uninit();
    
    std::cout << "endframe 3" << std::endl;
    UI::Uninit();
    
    std::cout << "endframe 4" << std::endl;
}

#endif



#include <core.h>
#include <file.h>

int main() {
    Core::Init();
    
    std::cout << Core::ENGINE_VERSION << std::endl;
    std::cout << "Hello World!" << std::endl;
    
    
    Core::File bepis("bepis.txt", Core::MODE_READ);

    std::cout << bepis.is_open() << std::endl;

    bepis.write_int32(69);
    bepis.write_float32(69.0f);
    bepis.write_uint8(69);
}