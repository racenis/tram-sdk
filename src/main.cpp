#include <iostream>

#include <core.h>
#include <async.h>
#include <ui.h>
#include <physics.h>

#include <gui.h>

#include <glfw3.h>

#include <entities/crate.h>
#include <entities/staticworldobject.h>
#include <entities/player.h>

#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>
#include <components/lightcomponent.h>


//#include <filesystem>

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;







PathNode pn1 = {
    .p1 = glm::vec3(3.0f, 0.0f, 0.0f),
    .p2 = glm::vec3(1.0f, 1.0f, 0.0f),
    .p3 = glm::vec3(-10.0f, 1.0f, 0.0f),
    .p4 = glm::vec3(0.0f, 0.0f, 1.0f),
};

PathNode pn2 = {
    .p1 = glm::vec3(0.0f, 0.0f, 1.0f),
    .p2 = glm::vec3(10.0f, -1.0f, 0.0f),
    .p3 = glm::vec3(15.0f, 0.0f, 3.0f),
    .p4 = glm::vec3(0.0f, 0.0f, 19.0f),
};

PathNode pn3 = {
    .p1 = glm::vec3(100.0f, 25.0f, 100.0f),
    .p2 = glm::vec3(100.0f, 10.0f, 0.0f),
    .p3 = glm::vec3(-1.0f, -1.0f, 0.0f),
    .p4 = glm::vec3(3.0f, 0.0f, 0.0f),
};

PathNode::Follower pnf = { .current_node = &pn3, .t = 0.9f};





int main() {
    std::cout << "Hello World! I have autism!" << std::endl;
    //std::cout << std::filesystem::current_path() << std::endl;

    // register the entity types, so that they can be loaded from level files
    Entity::Register(StaticWorldObject::data_name, [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::Register(Crate::data_name, [](std::string_view& params) -> Entity* {return new Crate(params);});

    Core::Init();           // core init should always be first
    UI::Init();
    Physics::InitPhysics(); // optional, but needed for StaticWorldObject, Crate and Player entities
    Render::Init();         // render init must always come after the ui inited
    Async::Init();          // async init must always come after render init

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

    // loading the demo level
    auto demo = PoolProxy<WorldCell>::New();
    demo->SetName(UID("demo"));
    demo->LoadFromDisk();
    demo->Load();
    
    // transition for the demo level
    auto demo_trans = PoolProxy<WorldCell::Transition>::New();
    demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(-5.0f, 0.0f, 5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 0.0f, 5.0f));
    demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, -5.0f));
    demo_trans->AddPoint(glm::vec3(-5.0f, 5.0f, 5.0f));
    demo_trans->AddPoint(glm::vec3(5.0f, 5.0f, 5.0f));
    
    demo_trans->GeneratePlanes();
    
    demo->AddTransition(demo_trans);

    // create the player entity
    Player player;
    player.SetLocation(0.0f, 1.0f, 0.0f);
    player.Load();



    // create the mongus model
    RenderComponent* monguser = PoolProxy<RenderComponent>::New();
    monguser->SetModel(UID("mongus"));
    monguser->SetPose(poseList.begin());
    monguser->Init();
    monguser->UpdateLocation(glm::vec3(0.0f, 10.0f, 0.0f));
    monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    LightComponent* lit = PoolProxy<LightComponent>::New();;
    lit->Init();
    lit->UpdateColor(255.0f, 0.0f, 255.0f);
    lit->UpdateDistance(1000.0f);

    // create the animation player for the mongus model
    ArmatureComponent* monguser_armature = PoolProxy<ArmatureComponent>::New();
    monguser_armature->SetModel(UID("mongus"));
    monguser_armature->Init();

    // link the mongus model and his animation player
    monguser->SetPose(monguser_armature->GetPosePtr());
    
    // turn on physics drawing
    DRAW_PHYSICS_DEBUG = true;

    
    while(!SHOULD_CLOSE){
        //auto time = glfwGetTime();
        UI::Update();

        if (UI::INPUT_STATE == STATE_DEFAULT) {
            player.GetLocation(Render::CAMERA_POSITION);
            Render::CAMERA_POSITION += glm::vec3(0.0f, 0.5f, 0.0f);
        }
        
        //time_of_day += 0.001f;
        SetSun(time_of_day); // this sets the ambient lighting


        static int tick = 0;
        tick++;

        // this will make the light spin
        lit->UpdateLocation(cos(((float)tick) / 60.0f) * 100.0f, 0.01 ,sin(((float)tick) / 60.0f) * 100.0f);
        
        // this makes the mongus model bob up and down
        monguser->UpdateLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.1f, 0.0f));
        
        //SetText("hello i have begonis", 10.0f, 10.0f, 1.2f, 300.0f, false, false, 1, COLOR_PINK);
        //SetText("begonis bepis", 10.0f, 40.0f, 1.0f, 300.0f, false, false, 0, COLOR_PINK);
        
        GUI::Begin();
        GUI::DebugMenu();
        GUI::EscapeMenu();
        GUI::End();
        
        // this loads the models and textures into video memory
        Async::ResourceLoader2ndStage();
        Async::FinishResource();

        if(tick == 100){
            monguser_armature->PlayAnimation(UID("Run"), 100, 1.0f, 1.0f);
        }

        Event::Dispatch();
        Message::Dispatch();

        // updates the physics world
        float phys_step = 1.0f / 60.0f;
        Physics::StepPhysics(phys_step);

        Render::UpdateArmatures();
        Render::Render();

        UI::EndFrame();
        
        
       // auto time2 = glfwGetTime();
        //std::cout << 1.0f/(time2-time) << " fps" << std::endl;
    }

    Async::Yeet();

    UI::Uninit();
    return 0;
}

