#include <iostream>

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

#include <components/triggercomponent.h>

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;

int main() {
    std::cout << "Hello World! I have autism!" << std::endl;
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
    Material::SetErrorMaterial(new Material(UID("defaulttexture"), Material::TEXTURE));
    Model::SetErrorModel(new Model(UID("errorstatic")));

    // load all of the language strings
    LoadText("data/lv.lang");

    // texture info stuff
    Material::LoadMaterialInfo("data/texture.list");

    // animations
    Animation mongusrun(UID("mongus"));
    Animation floppaidle(UID("turtle"));
    Animation bingusidle(UID("bingus_idle"));
    mongusrun.LoadFromDisk();
    floppaidle.LoadFromDisk();
    bingusidle.LoadFromDisk();
    
    // audios
    Audio::Sound derp (UID("derp"));
    derp.LoadFromDisk();

    // loading the demo level
    auto demo = PoolProxy<WorldCell>::New();
    demo->SetName(UID("demo"));
    demo->LoadFromDisk();
    //demo->Load();
    
    // transition for the demo level
    //auto demo_trans = PoolProxy<WorldCell::Transition>::New();
    //demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, 5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, 5.0f));
    //demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, -5.0f));
    //demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, 5.0f));
    //demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, 5.0f));
    
    //demo_trans->GeneratePlanes();
    
    //demo->AddTransition(demo_trans);


    /*
    auto cell1 = PoolProxy<WorldCell>::New();
    auto cell2 = PoolProxy<WorldCell>::New();
    auto cell3 = PoolProxy<WorldCell>::New();
    auto cell4 = PoolProxy<WorldCell>::New();
    auto cell5 = PoolProxy<WorldCell>::New();
    cell1->SetName(UID("loader-test-cell1"));
    cell2->SetName(UID("loader-test-cell2"));
    cell3->SetName(UID("loader-test-cell3"));
    cell4->SetName(UID("loader-test-cell4"));
    cell5->SetName(UID("loader-test-cell5"));
    cell1->LoadFromDisk();
    cell2->LoadFromDisk();
    cell3->LoadFromDisk();
    cell4->LoadFromDisk();
    cell5->LoadFromDisk();
    cell1->AddLink(cell2);
    cell1->AddLink(cell3);
    cell1->AddLink(cell4);
    cell2->AddLink(cell1);
    cell3->AddLink(cell1);
    cell4->AddLink(cell1);
    cell4->AddLink(cell5);
    cell5->AddLink(cell4);
    */
    


    // create the player entity
    Player player;
    player.SetLocation(0.0f, 1.0f, 0.0f);
    player.Load();



    // create the mongus model
    //RenderComponent* monguser = PoolProxy<RenderComponent>::New();
    //monguser->SetModel(UID("mongus"));
    //monguser->SetPose(poseList.begin().ptr);
    //monguser->Init();
    //monguser->UpdateLocation(glm::vec3(0.0f, 10.0f, 0.0f));
    //monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    LightComponent* lit = PoolProxy<LightComponent>::New();;
    lit->Init();
    lit->UpdateColor(255.0f, 0.0f, 255.0f);
    lit->UpdateDistance(1000.0f);

    // create the animation player for the mongus model
    //ArmatureComponent* monguser_armature = PoolProxy<ArmatureComponent>::New();
    //monguser_armature->SetModel(UID("mongus"));
    //monguser_armature->Init();

    // link the mongus model and his animation player
    //monguser->SetPose(monguser_armature->GetPosePtr());
    
    // turn on physics drawing
    DRAW_PHYSICS_DEBUG = true;
    
    while(!SHOULD_CLOSE){
        UI::Update();

        if (UI::INPUT_STATE == STATE_DEFAULT) {
            player.GetLocation(Render::CAMERA_POSITION);
            player.GetLocation(Audio::LISTENER_POSITION);
            Render::CAMERA_POSITION += glm::vec3(0.0f, 0.5f, 0.0f);
            Audio::LISTENER_POSITION += glm::vec3(0.0f, 0.5f, 0.0f);
            Audio::LISTENER_ORIENTATION[0] = Render::CAMERA_ROTATION * Render::CAMERA_FORWARD;
            Audio::LISTENER_ORIENTATION[1] = Render::CAMERA_UP;
        }
        
        //time_of_day += 0.001f;
        SetSun(time_of_day); // this sets the ambient lighting


        static int tick = 0;
        tick++;

        // this will make the light spin
        lit->UpdateLocation(cos(((float)tick) / 60.0f) * 100.0f, 0.01 ,sin(((float)tick) / 60.0f) * 100.0f);
        
        // this makes the mongus model bob up and down
        //monguser->UpdateLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.1f, 0.0f));
        
        //SetText("hello i have begonis", 10.0f, 10.0f, 1.2f, 300.0f, false, false, 1, COLOR_PINK);
        //SetText("begonis bepis", 10.0f, 40.0f, 1.0f, 300.0f, false, false, 0, COLOR_PINK);
        
        GUI::Begin();
        GUI::DebugMenu();
        GUI::EscapeMenu();        
        GUI::End();
        
        Audio::Update();
        
        // this loads the models and textures into video memory
        Async::ResourceLoader2ndStage();
        Async::FinishResource();

        if(tick == 100){
            //monguser_armature->PlayAnimation(UID("Run"), 100, 1.0f, 1.0f);
            //Audio::PlaySound(&derp, glm::vec3(0.0f, 0.0f, 0.0f));
        }

        Event::Dispatch();
        Message::Dispatch();
        
        WorldCell::Loader::LoadCells();

        // updates the physics world
        float phys_step = 1.0f / 60.0f;
        Physics::StepPhysics(phys_step);

        Render::UpdateArmatures();
        Render::Render();

        UI::EndFrame();
    }

    Async::Yeet();

    Audio::Uninit();
    UI::Uninit();
}

