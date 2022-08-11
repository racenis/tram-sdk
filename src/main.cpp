#include <iostream>

#include <core.h>
#include <entitycomponents.h>
#include <entities.h>
#include <async.h>
#include <ui.h>
#include <physics.h>

#include <gui.h>

//#include <filesystem>

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
    WorldCell demo(UID("demo"), glm::vec3(0.0f, 0.0f, 0.0f), false, false);
    demo.LoadFromDisk();
    demo.Load();

    // create the player entity
    Player player;
    player.SetLocation(0.0f, 1.0f, 0.0f);
    player.Load();



    // create the mongus model
    RenderComponent* monguser = PoolProxy<RenderComponent>::New();
    //monguser->SetModel(UID("mongus"));
    monguser->SetModel(UID("mongus"));
    monguser->SetPose(poseList.begin());
    monguser->Init();
    monguser->UpdateLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    monguser->UpdateRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    LightComponent* lit = PoolProxy<LightComponent>::New();;
    lit->Init();
    lit->UpdateColor(255.0f, 0.0f, 255.0f);
    lit->UpdateDistance(1000.0f);

    // create the animation player for the mongus model
    ArmatureComponent* monguser_armature = PoolProxy<ArmatureComponent>::New();
    //monguser_armature->SetModel(UID("mongus"));
    monguser_armature->SetModel(UID("mongus"));
    monguser_armature->Init();

    // link the mongus model and his animation player
    monguser->SetPose(monguser_armature->GetPosePtr());
    
    // turn on physics drawing
    DRAW_PHYSICS_DEBUG = true;

    
    while(!SHOULD_CLOSE){
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
        //monguser->UpdateLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.1f, 0.0f));
        
        //SetText("hello i have begonis", 10.0f, 10.0f, 1.2f, 300.0f, false, false, 1, COLOR_PINK);
        //SetText("begonis bepis", 10.0f, 40.0f, 1.0f, 300.0f, false, false, 0, COLOR_PINK);
        
        /*
        GUI::Begin();
        GUI::Frame(Core::GUI::FRAME_CENTER, 640.0f, 480.0);
        GUI::Frame(Core::GUI::FRAME_TOP, 128.0f);
        GUI::FillFrame(0.0f, 0.0f, 256.0f, 64.0f, Render::COLOR_WHITE, 3);
        
        static bool helptext = false;
        if(GUI::SmallButton(GUI::BUTTON_QUESTION)) helptext = !helptext;
        if(helptext) GUI::Text("Amogus!", 1);
        
        GUI::Frame(Core::GUI::FRAME_BOTTOM, 20.0f);
        
        static float pad = 650.0f; pad -= 2.0f;
        if (pad < -170.0f) pad = 650.0f;
        GUI::GlyphText("MAMMU KAD BUUS EEST", 2, pad, 0.0f, 2.0f, Render::COLOR_RED);
        
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

        
        
        static char text_box[25] = "benor!";
        static uint32_t text_colo = 0;
        static uint32_t text_publ = 0;
        static char text_disp[25] = "benor!";
        static char text_caps[25] = "BENOR!";
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
        
        static uint32_t benis = 0;
        
        GUI::RadioButton(benis, 0);GUI::Text("kreisi", 1);GUI::FrameBreakLine();
        GUI::RadioButton(benis, 1);GUI::Text("centrs", 1);GUI::FrameBreakLine();
        GUI::RadioButton(benis, 2);GUI::Text("labi", 1);;;GUI::FrameBreakLine();
        GUI::RadioButton(benis, 3);GUI::Text("slikti", 1);GUI::FrameBreakLine();
        
        
        char* disptext = do_caps ? text_caps : text_disp;
        if (text_publ == 0) {
            GUI::Text(disptext, 1, (GUI::orientation)benis, Render::COLOR_WHITE);
        } else if (text_publ == 1) {
            GUI::Text(disptext, 1, (GUI::orientation)benis, Render::COLOR_PINK);
        } else {
            GUI::Text(disptext, 1, (GUI::orientation)benis, Render::COLOR_CYAN);
        }
        
        static bool give_text = false;
        GUI::FrameBreakLine();
        GUI::CheckBox(give_text); GUI::Text("dot tekstu dibenaa", 1); GUI::FrameBreakLine();
        
        if(give_text) {
            for (int i = 0; i < 20; i++) GUI::FrameBreakLine();
            GUI::Text("es esmu teksts dibenaa", 2); GUI::FrameBreakLine();
        }
        
        GUI::EndFrame();
        GUI::End();
        */

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
    }

    Async::Yeet();

    UI::Uninit();
    return 0;
}

