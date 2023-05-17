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
#include <framework/loader.h>
#include <framework/event.h>
#include <framework/message.h>
#include <framework/language.h>
#include <physics/physics.h>
#include <audio/audio.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>

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

#include <render/api.h>

#include <platform/platform.h>

using namespace tram;
using namespace tram::Render;
using namespace tram::UI;

void UIDFunc(UID thing) {
    Log ("//////// {}", thing);
}

Ext::Camera::Camera* camera = nullptr;
Player* player = nullptr;
LightComponent* lit = nullptr;
Sprite* tolet_sprite = nullptr;
SpriteComponent* tolet_spinner = nullptr;
ParticleComponent* tolet_emitter = nullptr;
Audio::Sound* derp = nullptr;
Audio::Sound* eerp = nullptr;
AudioComponent* derp_player = nullptr;
RenderComponent* binguser = nullptr;
RenderComponent* monguser = nullptr;
ArmatureComponent* monguser_armature = nullptr;

std::pair<vec3, bool> RayIntersectsTriangle(vec3 ray_pos, vec3 ray_dir, vec3 point1, vec3 point2, vec3 point3) {
    const float epsilon = 0.000001;
    
    const vec3 p1_p2 = point2 - point1;
    const vec3 p1_p3 = point3 - point1;
    
    const vec3 p_vec = glm::cross(ray_dir, p1_p3);
    const float det = glm::dot(p1_p2, p_vec);

    // ray too parallel; yeet
    if (fabs(det) < epsilon) {
        return {vec3{}, false};
    }

    const float inv_det = 1.0 / det;
    const vec3 t_vec = ray_pos - point1;
    const float u = inv_det * glm::dot(t_vec, p_vec);

    if (u < 0.0 || u > 1.0) {
        return {vec3{}, false};
    }
    
    const vec3 q_vec = glm::cross(t_vec, p1_p2);
    const float v = inv_det * glm::dot(ray_dir, q_vec);

    if (v < 0.0 || u + v > 1.0) {
        return {vec3{}, false};
    }

    const float t = inv_det * glm::dot(p1_p3, q_vec);

    if (t > epsilon) {
        return {ray_pos + (ray_dir * t), true};
    } else {
        return {vec3{}, false};
    }

}


std::pair<AABBTriangle, bool> NearestTriangleFromRay (vec3 ray_pos, vec3 ray_dir) {
    std::vector<RenderComponent*> intersected_render_components;
    std::vector<AABBTriangle> intersected_triangles;
    intersected_render_components.reserve(10);
    intersected_triangles.reserve(10);
    
    RenderComponent::FindAllFromRay(ray_pos, ray_dir, intersected_render_components);
    
    float nearest_distance = INFINITY;
    AABBTriangle nearest_triangle;
    
    for (auto comp : intersected_render_components) {
        auto comp_position = comp->GetLocation();
        auto comp_rotation = comp->GetRotation();
        
        vec3 ray_pos_local = glm::inverse(comp_rotation) * (ray_pos - comp_position);
        vec3 ray_dir_local = glm::inverse(comp_rotation) * ray_dir;
        
        
        std::vector<AABBTriangle> intersected_triangles;
        intersected_triangles.reserve(10);

        Model::Find(comp->GetModel())->FindAllFromRay(ray_pos_local, ray_dir_local, intersected_triangles);
        
        
        for (auto& tri : intersected_triangles) {
            auto [intersection, happened] = RayIntersectsTriangle(ray_pos_local, ray_dir_local, tri.point1, tri.point2, tri.point3);
            
            if (!happened) continue;
            
            float intersection_distance = glm::distance(ray_pos_local, intersection);
            
            if (nearest_distance > intersection_distance) {
                nearest_distance = intersection_distance;
                nearest_triangle = {
                    comp_position + (comp_rotation * tri.point1),
                    comp_position + (comp_rotation * tri.point2),
                    comp_position + (comp_rotation * tri.point3),
                    comp_rotation * tri.normal,
                    tri.material
                };
            }
        }
    }
    
    if (nearest_distance != INFINITY) {
        AddLine(nearest_triangle.point1, nearest_triangle.point2, COLOR_WHITE);
        AddLine(nearest_triangle.point2, nearest_triangle.point3, COLOR_WHITE);
        AddLine(nearest_triangle.point3, nearest_triangle.point1, COLOR_WHITE);
    }
    
    return {nearest_triangle, nearest_distance != INFINITY};
}

void FindSomePaths(vec3 ray_pos, vec3 ray_dir, uint32_t iterations) {
    auto [triangle, hit] = NearestTriangleFromRay(ray_pos, ray_dir);
    
    if (!hit) {
        AddLine(ray_pos, ray_dir * 10.0f, COLOR_CYAN);
        return;
    } 
    
    vec3 intersection = RayIntersectsTriangle(ray_pos, ray_dir, triangle.point1, triangle.point2, triangle.point3).first;
    
    vec3 new_dir = ray_dir - (2.0f * glm::dot(ray_dir, triangle.normal) * triangle.normal);
    
    AddLine(ray_pos, intersection, COLOR_CYAN);
    //AddLineMarker(intersection, COLOR_PINK);
    
    if (iterations < 10) {
        FindSomePaths(intersection /*- (ray_dir * 0.01f)*/, new_dir, iterations + 1);
    }
}




void mainloop() {
    Core::Update();
    UI::Update();
    
    //Render::AddLineAABB({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, vec3(0.0f, 0.75f, 0.0f), COLOR_GREEN);
    //Render::AddLineAABB({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {3.0f, 0.0f, 3.0f}, vec3(0.75f, 0.0f, 0.0f), COLOR_GREEN);
    //Render::AddLineAABB({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {-3.0f, 3.0f, -3.0f}, vec3(0.0f, 0.0f, 0.75f), COLOR_GREEN);
    
    static int tick = 0;
    tick++;
    
    //vec3 ray_pos = Render::GetCameraPosition();
    //vec3 ray_dir = Render::GetCameraRotation() * DIRECTION_FORWARD;
    

    //NearestTriangleFromRay(ray_pos, ray_dir);
    
    
    //FindSomePaths(ray_pos, ray_dir, 0);


    //time_of_day += 0.001f;
    SetSun(0.8f); // this sets the ambient lighting
    

    
    //if (tick%69 == 0) {
    //    std::cout << "tick: " << tick << std::endl;
    //}
    
    if (tick % 4 == 0) {
        auto aaa = Entity::Find("muca");
        derp_player->SetLocation(aaa->GetLocation() + vec3(0.0f, 1.0f, 0.0f));
    }
    
    if (tick == 300) {
        //auto aaa = Entity::FindByName(UID("estijs"));
        //delete aaa;
        //monguser_armature->StopAnimation("mongus-run");
    }
    
    if (UI::PollKeyboardKey(UI::KEY_Y)) {
        tolet_emitter->EmitParticle({{0.0f, 3.0f, 0.0f}, {0.0f, 0.25f, 0.5f}, 15});
    }
    
    vec3 cast_from = player->GetLocation() + vec3(0.0f, 0.7f, 0.0f);
    vec3 cast_to = cast_from + (GetCameraRotation() * DIRECTION_FORWARD * 10.0f);
    
    auto ray_all = Physics::Raycast(cast_from, cast_to);
    auto ray_world = Physics::Raycast(cast_from, cast_to, Physics::COLL_WORLDOBJ);
    
    if (ray_world.collider) {
        //AddLineMarker(ray_world.point, COLOR_PINK);
    } else if (ray_all.collider) {
        //AddLineMarker(ray_all.point, COLOR_GREEN);
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
    binguser->SetLocation(litloc);
    binguser->SetRotation(litrot);
    lit->SetLocation(litloc);
    
    // this makes the mongus model bob up and down
    monguser->SetLocation(glm::vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.25f, 3.0f));
    
    //SetText("hello i have begonis", 10.0f, 10.0f, 1.2f, 300.0f, false, false, 1, COLOR_PINK);
    //SetText("begonis bepis", 10.0f, 40.0f, 1.0f, 300.0f, false, false, 0, COLOR_PINK);
    
    if (UI::PollKeyboardKey(UI::KEY_A)) {
        camera->SetTilt(0.1f);
    } else if (UI::PollKeyboardKey(UI::KEY_D)) {
        camera->SetTilt(-0.1f);
    } else {
        camera->SetTilt(0.0f);
    }
    
    if (UI::PollKeyboardKey(UI::KEY_W) || UI::PollKeyboardKey(UI::KEY_S) ||
        UI::PollKeyboardKey(UI::KEY_A) || UI::PollKeyboardKey(UI::KEY_D)) {
        camera->SetBobbing(1.0f);
    } else {
        camera->SetBobbing(0.0f);
    }
    
    Ext::Camera::Update();
    
    GUI::Begin();
    Ext::Menu::DebugMenu();
    Ext::Menu::EscapeMenu();
    
    std::string tickstr = std::to_string(GetTick());
    std::string tickstr2 = std::to_string(GetTickTime());
    //std::string tickstr3 = std::to_string(trigga->Poll().size());
    
    GUI::Text(tickstr.c_str(), 2, tram::GUI::TEXT_CENTER); GUI::FrameBreakLine();
    GUI::Text(tickstr2.c_str(), 2, tram::GUI::TEXT_CENTER); GUI::FrameBreakLine();
    //GUI::Text(tickstr3.c_str(), 2, tram::GUI::TEXT_CENTER);
    
    //{
    //    auto p = trigga->Poll();
    //    for (auto& t : p) {
    //        Render::AddLineMarker(t.point, Render::COLOR_CYAN);
    //    }
    //}
    
    /*
    GUI::Frame(tram::GUI::FRAME_CENTER, 640.0f, 480.0);
    GUI::Frame(tram::GUI::FRAME_TOP, 128.0f);
    GUI::FillFrame(0.0f, 0.0f, 256.0f, 64.0f, Render::COLOR_WHITE, 3);
    
    static bool helptext = false;
    if(GUI::SmallButton(GUI::BUTTON_QUESTION)) helptext = !helptext;
    if(helptext) GUI::Text("Amogus!", 1);
    
    GUI::Frame(tram::GUI::FRAME_BOTTOM, 20.0f);
    
    static float pad = 650.0f; pad -= 2.0f;
    if (pad < -170.0f) pad = 650.0f;
    GUI::GlyphText("KAD BUUS EEST", 2, pad, 0.0f, 2.0f, Render::COLOR_RED);
    
    GUI::EndFrame();
    GUI::EndFrame();
    GUI::Frame(tram::GUI::FRAME_BOTTOM, 480.0f - 128.0f);
    GUI::Frame(tram::GUI::FRAME_LEFT, 150.0f);
    GUI::FillFrame(104.0f, 108.0f, 12.0f, 12.0f, Render::COLOR_WHITE, 0);

    GUI::EndFrame();
    GUI::Frame(tram::GUI::FRAME_RIGHT, 640.0f - 150.0f);
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
    Async::ResourceLoader1stStage();
    Async::ResourceLoader2ndStage();
    Async::FinishResource();

    Event::Dispatch();
    Message::Dispatch();
    
    Loader::Update();
    
    ControllerComponent::Update();
    ArmatureComponent::Update();
    //RenderComponent::DrawAllAABB();

    Physics::Update();

    Render::Render();

    UI::EndFrame();
    
}



int main() {
    std::cout << "CURRENT_PLATFORM: " << CURRENT_PLATFORM << std::endl; 
    
    //std::cout << std::filesystem::current_path() << std::endl;

    // register the entity types, so that they can be loaded from level files
    Entity::RegisterType("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    Entity::RegisterType("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});

    SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);

    Core::Init();           // core init should always be first
    UI::Init();
    Physics::Init(); // optional, but needed for StaticWorldObject, Crate and Player entities
    Render::Init();         // render init must always come after the ui inited
    Async::Init(0);          // async init must always come after render init
    Audio::Init();
    GUI::Init();

    Ext::Menu::Init();
    Ext::Camera::Init();

    // load all of the language strings
    Language::Load("lv");

    // texture info stuff
    Material::LoadMaterialInfo("texture");

    Animation::Find(UID("mongus-run"))->LoadFromDisk();

    //UID bepitong;
    
    UIDFunc("bepito");

    //Render::SetScreenClear({0.0f, 0.0f, 0.0f}, false);
    Render::SetScreenClear({0.0f, 0.0f, 0.0f}, true);

    // loading the demo level
    WorldCell::Make(UID("demo_mov"))->LoadFromDisk();


    // create the player entity
    player = new Player;
    //player.SetLocation(vec3(0.0f, 1.0f, 0.0f));
    player->SetLocation(vec3(3.0f, 4.0f, 3.0f));
    player->Load();
    
    camera = new Ext::Camera::Camera;
    camera->SetMouselook(true);
    camera->SetRotateFollowing(true);
    camera->SetFollowingOffset({0.0f, 0.5f, 0.0f});
    camera->SetFollowing(player);
    
    Ext::Camera::SetCamera(camera);
    

    // create the createdwdww model
    binguser = PoolProxy<RenderComponent>::New();
    binguser->SetModel(UID("bingus"));
    binguser->Init();
    binguser->SetLocation(glm::vec3(0.0f, 0.0f, 0.0f));
    binguser->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create the mongus model
    monguser = PoolProxy<RenderComponent>::New();
    monguser->SetModel(UID("mongus"));
    //monguser->SetPose(poseList.begin().ptr);
    monguser->Init();
    monguser->SetLocation(glm::vec3(0.0f, 10.0f, 0.0f));
    monguser->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    lit = PoolProxy<LightComponent>::New();
    lit->Init();
    lit->SetColor({1.0f, 0.0f, 1.0f});
    lit->SetDistance(100.0f);

    // create the animation player for the mongus model
    monguser_armature = PoolProxy<ArmatureComponent>::New();
    monguser_armature->SetModel(UID("mongus"));
    monguser_armature->Init();
    monguser_armature->PlayAnimation(UID("mongus-run"), 100, 1.0f, 1.0f);
    //monguser_armature->SetOnAnimationFinishCallback([](ArmatureComponent* comp, name_t name) { std::cout << "ANIMATION " << name << " IS FINISH!" << std::endl; });

    // link the mongus model and his animation player
    //monguser->SetPose(monguser_armature->GetPosePtr());
    monguser->SetArmature(monguser_armature);
    
    // turn on physics drawing
    //DRAW_PHYSICS_DEBUG = true;
    
    tolet_sprite = new Sprite(UID("poland"));
    //tolet_sprite->SetMaterial(Material::Find(UID("poland")));
    //tolet_sprite->AutogenTiledFrames(0, 0, 40, 40, 6, 24, 15.0f, 1.0f);
    
    tolet_spinner = PoolProxy<SpriteComponent>::New();
    tolet_spinner->SetSprite(tolet_sprite);
    tolet_spinner->UpdateLocation(glm::vec3(3.0f, 1.2f, -1.0f));
    //tolet_spinner->Init();
    //tolet_spinner->Play();
    
    tolet_emitter = PoolProxy<ParticleComponent>::New();
    tolet_emitter->SetSprite(tolet_sprite);
    tolet_emitter->UpdateLocation(glm::vec3(0.0f, 1.2f, -7.0f));
    //tolet_emitter->Init();
    
    derp = Audio::Sound::Find(UID("apelsin"));
    eerp = Audio::Sound::Find(UID("impulse"));
    derp->Load();
    eerp->Load();
    
    derp_player = PoolProxy<AudioComponent>::New();
    derp_player->SetLocation(glm::vec3(0.0f, 1.0f, 0.0f));
    derp_player->SetSound("impulse");
    derp_player->SetRepeating(true);
    derp_player->Init();
    
    derp_player->Play();
    
    UI::BindKeyboardKey(UI::KEY_APOSTROPHE, UI::KeyBinding { .type = UI::KeyBinding::SPECIAL_OPTION, .special_option = [](){
        camera->SetFollowing(nullptr);
        camera->SetMouselook(false);
        camera->SetLocation({0.0f, 25.0f, 0.0f});
        camera->SetRotation(vec3 {glm::radians(-90.0f), glm::radians(86.8f), 0.0f});
        
        std::cout << "aaa" << std::endl;
    }});
    
    
    //auto crate_ent = Entity::Find(UID("estijs"));
    
#ifdef __EMSCRIPTEN__
    UI::SetWebMainLoop(mainloop);
#else
    while(!EXIT){
        mainloop();
    }

    Async::Yeet();
    Audio::Uninit();
    UI::Uninit();
#endif
}

#endif
#ifndef NEKADEEE

#include <framework/core.h>
#include <framework/file.h>

#include <framework/logging.h>

using namespace tram;

#include <templates/macros.h>
#include <templates/aabb.h>

int main() {
    tram::Core::Init();
    
    std::cout << "Hello World!" << std::endl;
    
    
    
    AABBTree tree;
    
    tree.InsertLeaf(123, {-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
    auto yoink4 = tree.InsertLeaf(123, {-2.0f, -2.0f, -2.0f}, {0.0f, 0.0f, 0.0f});
    auto yoink = tree.InsertLeaf(123, {-0.0f, -0.0f, -0.0f}, {2.0f, 2.0f, 2.0f});
    
    tree.RemoveLeaf(yoink);
    
    tree.InsertLeaf(123, {-0.0f, -0.0f, -0.0f}, {2.0f, 2.0f, 2.0f});
    auto yoink2 = tree.InsertLeaf(123, {-32.0f, -21.0f, -23.0f}, {10.0f, 30.0f, 40.0f});
    tree.InsertLeaf(123, {-1.0f, -1.0f, -1.0f}, {1.0f, 12.0f, 11.0f});
    
    tree.RemoveLeaf(yoink2);
    
    tree.InsertLeaf(123, {-1.0f, -1234.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
    auto yoink3 = tree.InsertLeaf(123, {-1314.0f, -2525.0f, -226.0f}, {0.0f, 0.0f, 0.0f});
    
    tree.RemoveLeaf(yoink3);
    tree.RemoveLeaf(yoink4);
    
    tree.InsertLeaf(123, {10.0f, 10.0f, 10.0f}, {100.0f, 100.0f, 100.0f});
    
    
    
}

#endif