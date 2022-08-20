#include <iostream>

#include <core.h>
#include <entitycomponents.h>
#include <entities.h>
#include <async.h>
#include <ui.h>
#include <physics.h>

#include <gui.h>

#include <chrono>

//#include <filesystem>

using namespace Core;
using namespace Core::Render;
using namespace Core::UI;


struct PathNode2 {
    PathNode2* next;
    PathNode2* prev;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
    glm::vec3 p4;
    float lens[10];
    
    void ProdPoint(glm::vec3& p, const float& t);
    void CalcLens();
    void Render();
};

void PathNode2::CalcLens(){
    float c = 0.1f;
    glm::vec3 prev_p;
    glm::vec3 this_p;
    ProdPoint(prev_p, 0.0f);
    for (int i = 0; i < 10; i++) {
        ProdPoint(this_p, c);
        lens[i] = glm::distance(prev_p, this_p);
        prev_p = this_p;
        c += 0.1f;
    }
}

void PathNode2::ProdPoint(glm::vec3& p, const float& t){
    glm::vec3 l1 = glm::mix(p1, p2, t);
    glm::vec3 l2 = glm::mix(p3, p4, t);
    p = glm::mix(l1, l2, t);
}

void PathNode2::Render(){
    float c = 0.1f;
    glm::vec3 prev_p;
    glm::vec3 this_p;
    ProdPoint(prev_p, 0.0f);
    for (int i = 0; i < 10; i++) {
        ProdPoint(this_p, c);
        Render::AddLine(prev_p, this_p, Render::COLOR_CYAN);
        prev_p = this_p;
        c += 0.1f;
    }
    Render::AddLineMarker(p1, Render::COLOR_BLUE);
    Render::AddLineMarker(p2, Render::COLOR_BLUE);
    Render::AddLineMarker(p3, Render::COLOR_BLUE);
    Render::AddLineMarker(p4, Render::COLOR_BLUE);
}

struct PathNodeFollower {
    PathNode2* current_node;
    float t = 0.0f;
    
    void GoForth(float ammount);
    void Render();
    void GetPosition(glm::vec3& pos);
};

void PathNodeFollower::GoForth(float ammount){
    //int index = std::floor(t*10.0f);
    //t += (ammount/current_node->lens[index]) * 0.1f;
    
    glm::vec3 v1;
    glm::vec3 v2;
    current_node->ProdPoint(v1, t);
    current_node->ProdPoint(v2, t+0.01f);
    
    t += (ammount/glm::distance(v1, v2)) * 0.01f;
    if (t >= 1.0f) {
        current_node = current_node->next;
        t = 0.0f;
    }
}

void PathNodeFollower::Render(){
    glm::vec3 pp;
    current_node->ProdPoint(pp, t);
    Render::AddLineMarker(pp, Render::COLOR_GREEN);
}

void PathNodeFollower::GetPosition(glm::vec3& pos){
    current_node->ProdPoint(pos, t);
}


PathNode2 pn1 = {
    .p1 = glm::vec3(3.0f, 0.0f, 0.0f),
    .p2 = glm::vec3(1.0f, 1.0f, 0.0f),
    .p3 = glm::vec3(-10.0f, 1.0f, 0.0f),
    .p4 = glm::vec3(0.0f, 0.0f, 1.0f),
};

PathNode2 pn2 = {
    .p1 = glm::vec3(0.0f, 0.0f, 1.0f),
    .p2 = glm::vec3(10.0f, -1.0f, 0.0f),
    .p3 = glm::vec3(15.0f, 0.0f, 3.0f),
    .p4 = glm::vec3(0.0f, 0.0f, 19.0f),
};

PathNode2 pn3 = {
    .p1 = glm::vec3(100.0f, 25.0f, 100.0f),
    .p2 = glm::vec3(100.0f, 10.0f, 0.0f),
    .p3 = glm::vec3(-1.0f, -1.0f, 0.0f),
    .p4 = glm::vec3(3.0f, 0.0f, 0.0f),
};

PathNodeFollower pnf = { .current_node = &pn3, .t = 0.9f};




struct ntrans {
    std::vector<glm::vec3> points;
    std::vector<glm::vec4> planes;
    
    void GenPlanes(bool disp = false) {
        assert(points.size() > 3);
        
        planes.clear();
        
        for (size_t i = 0; i < points.size(); i++) {
            for (size_t j = i+1; j < points.size(); j++) {
                for (size_t k = j+1; k < points.size(); k++) {
                    auto& A = points[i];
                    auto& B = points[j];
                    auto& C = points[k];
                    auto AB = B - A;
                    auto AC = C - A;
                    auto cross = glm::cross(AB, AC);
                    auto d = -(cross.x*A.x + cross.y*A.y + cross.z*A.z);
                    
                    glm::vec4 plane = glm::vec4(cross, d);
                    
                    for (size_t it = 0; it < points.size(); it++) {
                        if(i == it || j == it || k == it) continue;
                        if(glm::dot(plane, glm::vec4(points[it], 1.0f)) < 0.0f) {
                            plane *= -1.0f;
                            goto tryagain;
                        }
                    }
                    
                    okay:
                    
                    if (disp) {
                        AddLine(A, B, COLOR_WHITE);
                        AddLine(A, C, COLOR_WHITE);
                        AddLine(B, C, COLOR_WHITE);
                        
                        AddLine(A, A+(cross*0.1f)+glm::vec3(0.002f), COLOR_CYAN);
                        AddLine(B, B+(cross*0.1f)+glm::vec3(0.002f), COLOR_CYAN);
                        AddLine(C, C+(cross*0.1f)+glm::vec3(0.002f), COLOR_CYAN);
                    }
                    
                    planes.push_back(plane);
                    
                    yeet:
                    continue;
                    
                    tryagain:
                    
                    for (size_t it = 0; it < points.size(); it++) {
                        if(i == it || j == it || k == it) continue;
                        if(glm::dot(plane, glm::vec4(points[it], 1.0f)) < 0.0f) {
                            goto yeet;
                        }
                    }
                    
                    goto okay;
                }
            }
        }
        
        std::sort(planes.begin(), planes.end(), [](const glm::vec4& a, const glm::vec4& b){ 
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            if (a.z != b.z) return a.z < b.z;
            return a.w < b.w;});
        planes.erase(unique( planes.begin(), planes.end() ), planes.end());
        
        
    }
    
    bool IsInside (const glm::vec3& point) {
        for(size_t i = 0; i < planes.size(); i++)
            if(glm::dot(planes[i], glm::vec4(point, 1.0f)) < 0.0f) return false;    
        return true;
    }
};

ntrans tran = { .points = {
    glm::vec3(-2.5f, 0.0f, -2.5f),
    glm::vec3(2.5f, 0.0f, -2.5f),
    glm::vec3(-2.5f, 0.0f, 2.5f),
    glm::vec3(2.5f, 0.0f, 2.5f),
    glm::vec3(0.0f, 5.0f, 0.0f),
    glm::vec3(-2.5f, 5.0f, -2.5f),
    glm::vec3(2.5f, 5.0f, -2.5f),
    glm::vec3(-2.5f, 5.0f, 2.5f),
    glm::vec3(2.5f, 5.0f, 2.5f),
    
    
    }};






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
    auto demo = PoolProxy<WorldCell>::New();
    demo->SetName(UID("demo"));
    demo->LoadFromDisk();
    demo->Load();

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
    monguser->UpdateLocation(glm::vec3(0.0f, 10.0f, 0.0f));
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
        
        GUI::Begin();
        GUI::DebugMenu();
        GUI::EscapeMenu();
        GUI::End();
        
        //pn1.next = &pn2;
        //pn2.next = &pn3;
        //pn3.next = &pn1;
        
        //pn1.Render();
        //pn2.Render();
        //pn3.Render();
        
        //pn1.CalcLens();
        //pn2.CalcLens();
        //pn3.CalcLens();
        
        //pnf.GoForth(0.01f);
        //pnf.Render();
        
        //pnf.GetPosition(CAMERA_POSITION);
        
        
        tran.GenPlanes(true);
        
        if (tran.IsInside(CAMERA_POSITION)) {
            AddLineMarker(CAMERA_POSITION, COLOR_GREEN);
        } else {
            AddLineMarker(CAMERA_POSITION, COLOR_RED);    
        }
        

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

