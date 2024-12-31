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
#include <framework/script.h>
#include <framework/path.h>
#include <framework/stats.h>
#include <physics/physics.h>
#include <audio/audio.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>
#include <framework/settings.h>

#include <entities/crate.h>
#include <entities/staticworldobject.h>
#include <entities/light.h>
#include <entities/player.h>
#include <entities/sound.h>
#include <entities/decoration.h>
#include <entities/trigger.h>

#include <components/render.h>
#include <components/animation.h>
#include <components/light.h>

#include <components/sprite.h>
#include <components/particle.h>
#include <components/audio.h>

#include <components/controller.h>

#include <components/trigger.h>
#include <components/player.h>

#include <extensions/menu/menu.h>
#include <extensions/camera/camera.h>
#include <extensions/camera/firstperson.h>

#include <extensions/kitchensink/kitchensink.h>
#include <extensions/kitchensink/entities.h>
#include <extensions/kitchensink/inventory.h>
#include <extensions/kitchensink/soundtable.h>
#include <extensions/scripting/lua.h>

#include <extensions/kitchensink/dialog.h>
#include <extensions/kitchensink/quest.h>
#include <extensions/kitchensink/menu.h>

#include <render/api.h>
#include <render/scene.h>

#include <platform/platform.h>
#include <platform/image.h>

using namespace tram;
using namespace tram::Render;
using namespace tram::UI;

#include <templates/octree.h>
Octree<vec3> octree("tree of nodes", 100);












void UIDFunc(UID thing) {
    Log ("//////// {}", thing);
}

Ext::Camera::FirstPersonCamera* camera = nullptr;
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
AnimationComponent* monguser_armature = nullptr;

bool record = false;

PathFollower* follower = nullptr;
vec3 initial_follower = {0.1f, 1.0f, 1.5f};

Component<RenderComponent> chamberpot;

void mainloop() {
    Core::Update();
    UI::Update();
    
    Render::AddText({0, 0, 0}, "HELLOOO");
    

    //Path::Find("test")->Draw();
    
    //octree.Draw();
    
    //vec3 toots[4];
    //size_t count = octree.Find(toots, Render::GetViewPosition());
    //for (size_t i  = 0 ; i < count ; i++)
        //AddLineAABB(vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f), toots[i], vec3(0.0f, 0.0f, 0.0f), COLOR_PINK);
    
    follower->Advance(0.025f);
    
    Entity* mongus = Entity::Find("mongus");
       
    if (mongus && GetTick() > 120) {
        //AddLineMarker(mongus->GetLocation(), COLOR_RED);
        mongus->SetLocation(follower->GetPosition() - 0.7f * DIRECTION_UP);
        mongus->SetRotation(glm::quatLookAt(follower->GetTangent(), DIRECTION_UP));
    }
    
    //std::cout << PollKeyboardAxisDelta(tram::UI::KEY_MOUSE_X) << std::endl;
    //std::cout << PollKeyboardAxis(tram::UI::KEY_MOUSE_X) << std::endl;
    
    //follower->SetOrientation(Render::GetViewRotation() * DIRECTION_FORWARD);
    //AddLineMarker(follower->GetPosition(), COLOR_PINK);
    
    //follower->Project(Render::GetViewPosition());
    
    //Render::AddLineAABB({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, vec3(0.0f, 0.75f, 0.0f), COLOR_GREEN);
    //Render::AddLineAABB({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {3.0f, 0.0f, 3.0f}, vec3(0.75f, 0.0f, 0.0f), COLOR_GREEN);
    //Render::AddLineAABB({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}, {-3.0f, 3.0f, -3.0f}, vec3(0.0f, 0.0f, 0.75f), COLOR_GREEN);

    //Render::AddSphere({1.0f, 1.0f, 1.0f}, 0.5f, COLOR_CYAN);
    
    static int tick = 0;
    tick++;
    
    static int frametick = 0;
    static int lastprint = 0;
    frametick++;

    if ((int) GetTickTime() != lastprint) {
        lastprint = GetTickTime();
        std::cout << frametick << std::endl;
        frametick = 0;
    }

    /*if (UI::GetInputState() != STATE_DEFAULT) {
        for (int i = 0; i < 1; i ++){
        float x = UI::PollKeyboardAxis(UI::KEY_MOUSE_X);
        float y = UI::PollKeyboardAxis(UI::KEY_MOUSE_Y);
        
        vec3 far_point = Render::ProjectInverse({x, y+ i*20, 0.0f});
        vec3 near_point = Render::ProjectInverse({x, y+ i*20, 1000.0f});
        
        vec3 look_direction = glm::normalize(far_point - near_point);
        vec3 look_position = near_point;
        
        auto res = Render::AABB::FindNearestFromRay(look_position, look_direction, -1);
        
        //std::cout << res.triangle.point1.x << " ";
        //std::cout << res.triangle.point1.y << " ";
        //std::cout << res.triangle.point1.z << "\t";
        //std::cout << res.triangle.point2.x << " ";
        //std::cout << res.triangle.point2.y << " ";
        //std::cout << res.triangle.point2.z << "\t";
        //std::cout << res.triangle.point3.x << " ";
        //std::cout << res.triangle.point3.y << " ";
        //std::cout << res.triangle.point3.z << std::endl;
        
        if (res.data) {
            Render::AddLine(res.triangle.point1, res.triangle.point2, Render::COLOR_WHITE);
            Render::AddLine(res.triangle.point2, res.triangle.point3, Render::COLOR_WHITE);
            Render::AddLine(res.triangle.point3, res.triangle.point1, Render::COLOR_WHITE);
        }}
        
        

        //Log("{} {} {} : {} {} {}", res2.x, res2.y, res2.z, res3.x, res3.y, res3.z);
        
        
    }*/

    
    //vec3 ray_pos = Render::GetCameraPosition();
    //vec3 ray_dir = Render::GetCameraRotation() * DIRECTION_FORWARD;
    

    //NearestTriangleFromRay(ray_pos, ray_dir);
    
    
    //FindSomePaths(ray_pos, ray_dir, 0);


    //time_of_day += 0.001f;
    //SetSun(0.8f); // this sets the ambient lighting
    

    
    //if (tick%69 == 0) {
    //    std::cout << "tick: " << tick << std::endl;
    //}
    
    if (tick % 4 == 0) {
        //auto aaa = Entity::Find("muca");
        //derp_player->SetLocation(aaa->GetLocation() + vec3(0.0f, 1.0f, 0.0f));
    }
    
    if (tick == 300) {
        //auto aaa = Entity::FindByName(UID("estijs"));
        //delete aaa;
        //monguser_armature->StopAnimation("mongus-run");
    }
    
    if (UI::PollKeyboardKey(UI::KEY_Y)) {
        //tolet_emitter->EmitParticle({{0.0f, 3.0f, 0.0f}, {0.0f, 0.25f, 0.5f}, 15});
    }
    
    static float fov = 60.0f;
    if (UI::PollKeyboardKey(UI::KEY_EQUAL)) {
        fov += 1.0f;
        SetViewFov(fov);
        std::cout << "fov: " << fov << std::endl;
    }
    if (UI::PollKeyboardKey(UI::KEY_MINUS)) {
        fov -= 1.0f;
        SetViewFov(fov);
        std::cout << "fov: " << fov << std::endl;
    }
    
    // this will make the light spin
    //vec3 litloc = vec3(cos(((float)tick) / 60.0f) * 7.0f, 1.25f ,sin(((float)tick) / 60.0f) * 7.0f);
    //quat litrot;
    //QuatLookAt(litrot, litloc, vec3(0.0f, 1.8f, 0.0f));
    //binguser->SetLocation(litloc);
    //binguser->SetRotation(litrot);
    //lit->SetLocation(litloc);
    
    // this makes the mongus model bob up and down
    //monguser->SetLocation(vec3(0.0f, 0.5f + sin(((float)tick) / 45.0f)*0.25f, 3.0f));
    
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
    
    // ---------
    //    GUI
    // ---------
    
    static bool dingus_menu = false;

    if (PollKeyboardKey(KEY_Y)) {
        if (!dingus_menu) {
            UI::SetInputState(tram::UI::STATE_MENU_OPEN);
        }
        
        dingus_menu = true;
    
    }
    
    GUI::Begin();
    //Ext::Menu::DebugMenu();
    //Ext::Menu::EscapeMenu();
    Ext::Menu::Update();
    
    std::string tickstr = std::to_string(GetTick());
    std::string tickstr2 = std::to_string(GetTickTime());
    //std::string tickstr3 = std::to_string(trigga->Poll().size());
    
    //GUI::Text(tickstr.c_str(), 4, tram::GUI::TEXT_CENTER); GUI::FrameBreakLine();
    //GUI::Text(tickstr2.c_str(), 4, tram::GUI::TEXT_CENTER); GUI::FrameBreakLine();
    //GUI::Text("AAAAAAAAAA", 4, tram::GUI::TEXT_CENTER); GUI::FrameBreakLine();
    //GUI::Text(tickstr3.c_str(), 2, tram::GUI::TEXT_CENTER);
    
    /*if (dingus_menu) {
        if (GUI::Button("close menu!")) {
           UI::SetInputState(tram::UI::STATE_DEFAULT); 
           dingus_menu = false;
        }
        
        GUI::FrameBreakLine();
    }*/
    
    
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
    
    //AABB::DebugDrawTree();
    
    if (record && GetTick() % 4 == 0) {
        char* buffer = (char*)malloc(GetScreenWidth() * GetScreenHeight() * 3);
        std::string filename = std::string("screenshot") + std::to_string(GetTick()) + ".png";
        Render::API::GetScreen(buffer, GetScreenWidth(), GetScreenHeight());
        Platform::SaveImageToDisk(filename.c_str(), GetScreenWidth(), GetScreenHeight(), buffer);
        free(buffer);
    }
    
    
    
    GUI::End();
    GUI::Update();
    
    Audio::Update();
    
    // this loads the models and textures into video memory
#ifdef __EMSCRIPTEN__
    Async::LoadResourcesFromDisk();
#endif
    Async::LoadResourcesFromMemory();
    Async::FinishResources();

    Event::Dispatch();
    Message::Dispatch();
    
    Entity::Update();
    
    Loader::Update();
    
    if (tick > 100) ControllerComponent::Update();
    AnimationComponent::Update();
    //RenderComponent::DrawAllAABB();

    Physics::Update();

    Render::Render();

    Stats::Collate();

    UI::EndFrame();
}

int main(int argc, const char** argv) {
    //bool poopis = false;

    //Settings::Register(vec4(0, 0, 0, 0), "toopis");
    //Settings::Register(&poopis, "poopis");

    //vec4 toopis = Settings::Get("toopis");

    //std::cout << "toopis " << toopis.x << " " << toopis.y << " " << toopis.z << " " << toopis.w << " " << std::endl;
    //std::cout << "poopis " << poopis << std::endl;
    
    Path::Find("test")->LoadFromDisk();
    
    follower = new PathFollower(Path::Find("test"), initial_follower, PATH_LINEAR);
    
    octree.Insert({0.1f, 0.2f, 0.3f}, {0.1f, 0.2f, 0.3f});
    octree.Insert({0.67f, 0.23, 0.7f}, {0.67f, 0.23, 0.7f});
    octree.Insert({-0.11f, 0.52f, -0.163f}, {-0.11f, 0.52f, -0.163f});
    octree.Insert({1.1f, 1.2f, -3.163f}, {1.1f, 1.2f, -3.163f});
    octree.Insert({4.1f, 2.2f, 6.163f}, {4.1f, 2.2f, 6.163f});
    octree.Insert({-4.1f, 2.2f, 6.163f}, {-4.1f, 2.2f, 6.163f});
    octree.Insert({4.1f, 1.2f, -6.163f}, {4.1f, 1.2f, -6.163f});
    
    octree.Insert({1.0f, 2.0f, 3.0f}, {1.0f, 2.0f, 3.0f});
    octree.Insert({-1.0f, 2.0f, 3.0f}, {-1.0f, 2.0f, 3.0f});
    octree.Insert({1.0f, -2.0f, 3.0f}, {1.0f, -2.0f, 3.0f});
    octree.Insert({1.0f, 2.0f, -3.0f}, {1.0f, 2.0f, -3.0f});
    octree.Insert({-1.0f, -2.0f, 3.0f}, {-1.0f, -2.0f, 3.0f});
    octree.Insert({1.0f, -2.0f, -3.0f}, {1.0f, -2.0f, -3.0f});
    octree.Insert({-1.0f, -2.0f, -3.0f}, {-1.0f, -2.0f, -3.0f});
    
    octree.Insert({3.0f, 2.0f, 1.0f}, {3.0f, 2.0f, 1.0f});
    octree.Insert({-3.0f, 2.0f, 1.0f}, {-3.0f, 2.0f, 1.0f});
    octree.Insert({3.0f, -2.0f, 1.0f}, {3.0f, -2.0f, 1.0f});
    octree.Insert({3.0f, 2.0f, -1.0f}, {3.0f, 2.0f, -1.0f});
    octree.Insert({-3.0f, -2.0f, 1.0f}, {-3.0f, -2.0f, 1.0f});
    octree.Insert({3.0f, -2.0f, -1.0f}, {3.0f, -2.0f, -1.0f});
    octree.Insert({-3.0f, -2.0f, -1.0f}, {-3.0f, -2.0f, -1.0f});
    
    octree.Insert({3.0f, 1.0f, 2.0f}, {3.0f, 1.0f, 2.0f});
    octree.Insert({-3.0f, 1.0f, 2.0f}, {-3.0f, 1.0f, 2.0f});
    octree.Insert({3.0f, -1.0f, 2.0f}, {3.0f, -1.0f, 2.0f});
    octree.Insert({3.0f, 1.0f, -2.0f}, {3.0f, 1.0f, -2.0f});
    octree.Insert({-3.0f, -1.0f, 2.0f}, {-3.0f, -1.0f, 2.0f});
    octree.Insert({3.0f, -1.0f, -2.0f}, {3.0f, -1.0f, -2.0f});
    octree.Insert({-3.0f, -1.0f, -2.0f}, {-3.0f, -1.0f, -2.0f});
    
    octree.Insert({3.0f, 2.0f, 1.0f}, {3.0f, 2.0f, 1.0f});
    octree.Insert({-3.0f, 2.0f, 1.0f}, {-3.0f, 2.0f, 1.0f});
    octree.Insert({3.0f, -2.0f, 1.0f}, {3.0f, -2.0f, 1.0f});
    octree.Insert({3.0f, 2.0f, -1.0f}, {3.0f, 2.0f, -1.0f});
    octree.Insert({-3.0f, -2.0f, 1.0f}, {-3.0f, -2.0f, 1.0f});
    octree.Insert({-3.0f, -2.0f, -1.0f}, {-3.0f, -2.0f, -1.0f});
    octree.Insert({3.0f, -2.0f, -1.0f}, {3.0f, -2.0f, -1.0f});
    
    
    Settings::Parse(argv, argc);
    
    //std::cout << std::filesystem::current_path() << std::endl;

    // register the entity types, so that they can be loaded from level files
    //Entity::RegisterType("staticwobj", [](std::string_view& params) -> Entity* {return new StaticWorldObject(params);});
    //Entity::RegisterType("crate", [](std::string_view& params) -> Entity* {return new Crate(params);});
    Light::Register();
    Crate::Register();
    Sound::Register();
    Decoration::Register();
    Trigger::Register();
    StaticWorldObject::Register();
    Ext::Kitchensink::Button::Register();

    Core::Init();           // core init should always be first
    UI::Init();
    Physics::Init(); // optional, but needed for StaticWorldObject, Crate and Player entities
    Render::Init();         // render init must always come after the ui inited
    Async::Init();          // async init must always come after render init
    Audio::Init();
    GUI::Init();

    Ext::Menu::Init();
    Ext::Camera::Init();
    
    Ext::Scripting::Lua::Init();

    Script::Init();
    
    Script::LoadScript("init");

    //static materialtype_t special_material = RegisterMaterialType("special");
    //API::RegisterShader(VERTEX_DYNAMIC, special_material, "normal_dynamic", "special_dynamic");

    // load all of the language strings
    Language::Load("lv");

    // texture info stuff
    Material::LoadMaterialInfo("material");

    Animation::Find(UID("mongus-run"))->LoadFromDisk();
    Animation::Find(UID("mongus-jump"))->LoadFromDisk();

    Ext::Kitchensink::DialogTopic::LoadFromDisk("dialogs");
    Ext::Kitchensink::Quest::LoadFromDisk("quests");
    Ext::Kitchensink::Inventory::LoadFromDisk("items");

    //UID bepitong;
    
    UIDFunc("bepito");

    //Render::SetScreenClear({0.0f, 0.0f, 0.0f}, false);
    Render::API::SetScreenClear({0.0f, 0.0f, 0.0f}, true);

    // loading the demo level
    WorldCell::Make(UID("demo_mov"))->LoadFromDisk();


    // create the player entity
    player = new Player;
    //player.SetLocation(vec3(0.0f, 1.0f, 0.0f));
    player->SetLocation(vec3(0.0f, (1.85f/2.0f) + 0.05f, 0.0f));
    player->Load();
    Loader::Track(player);
    //WorldCell::Find(player->GetLocation())->Add(player);
    
    player->controllercomponent->SetFriction(0.82f);
    player->controllercomponent->SetWalkSpeed(0.2f);
    player->controllercomponent->SetRunSpeed(0.3f);
    
    camera = new Ext::Camera::FirstPersonCamera;
    camera->SetMouselook(true);
    camera->SetRotateFollowing(true);
    camera->SetFollowingOffset({0.0f, 0.5f, 0.0f});
    camera->SetFollowing(player);
    
    camera->SetBobbingDistance(0.1f); // why does removing this make camera not work?????
    camera->SetBobbingTilt(0.1f);
    //camera->SetBobSpeed(0.1f);
    //camera->SetFollowingInterpolation(0.2f);
    
    //Ext::Camera::SetCamera(camera);
    camera->SetActive();
    
    /*camera->SetBobbingCallback([](Ext::Camera::Camera* c){
        auto res = Render::AABB::FindNearestFromRay(GetCameraPosition(), vec3(0.0f, -1.0f, 0.0f), -1);
        
        if (res.data) {
            auto comp = (RenderComponent*)res.data;
            auto mat = comp->GetModel()->GetMaterials()[res.triangle.material];
            std::cout << mat->GetName() << std::endl;
            
            //new Sound("step", 1.0f, GetCameraPosition() - vec3(0.0f, 1.0f, 0.0f));
        } else {
            std::cout << "No intersection." << std::endl;
        }
    });*/
    

    // create the createdwdww model
    //binguser = PoolProxy<RenderComponent>::New();
    //binguser->SetModel(UID("bingus"));
    //binguser->Init();
    //binguser->SetLocation(vec3(0.0f, 0.0f, 0.0f));
    //binguser->SetRotation(quat(vec3(0.0f, 0.0f, 0.0f)));

    // create the mongus model
    //monguser = PoolProxy<RenderComponent>::New();
    //monguser->SetModel(UID("mongus"));
    //monguser->Init();
    //monguser->SetLocation(vec3(0.0f, 10.0f, 0.0f));
    //monguser->SetRotation(quat(vec3(0.0f, 0.0f, 0.0f)));

    // create a light
    //lit = PoolProxy<LightComponent>::New();
    //lit->Init();
    //lit->SetColor({1.0f, 0.0f, 1.0f});
    //lit->SetDistance(100.0f);

    for (int x = 0; x < 20; x++) {
        for (int y = 0; y < 20; y++) {
            RenderComponent* pot = PoolProxy<RenderComponent>::New();
            pot->SetModel("chamberpot2");
            pot->SetScale({0.2, 0.2, 0.2});
            pot->SetLocation({10 - x, 25, 10 - y});
            pot->Init();
        }
    }

    // create the animation player for the mongus model
    //monguser_armature = PoolProxy<AnimationComponent>::New();
    //monguser_armature->SetModel(Render::Model::Find(UID("mongus")));
    //monguser_armature->Init();
    //monguser_armature->PlayAnimation(UID("mongus-run"), 100, 1.0f, 1.0f);
    //monguser_armature->SetOnAnimationFinishCallback([](ArmatureComponent* comp, name_t name) { std::cout << "ANIMATION " << name << " IS FINISH!" << std::endl; });

    // link the mongus model and his animation player
    //monguser->SetArmature(monguser_armature);
    
    // turn on physics drawing
    //DRAW_PHYSICS_DEBUG = true;
    
    tolet_sprite = new Sprite(UID("poland"));
    
    tolet_spinner = PoolProxy<SpriteComponent>::New();
    tolet_spinner->SetSprite(tolet_sprite);
    tolet_spinner->UpdateLocation(vec3(3.0f, 1.2f, -1.0f));
    tolet_spinner->Init();
    tolet_spinner->Play();
    
    tolet_emitter = PoolProxy<ParticleComponent>::New();
    tolet_emitter->SetSprite(tolet_sprite);
    tolet_emitter->UpdateLocation(vec3(0.0f, 1.2f, -7.0f));
    tolet_emitter->Init();
    
    derp = Audio::Sound::Find(UID("apelsin"));
    //eerp = Audio::Sound::Find(UID("dekpunkta"));
    derp->Load();
    //eerp->Load();
    
    derp_player = PoolProxy<AudioComponent>::New();
    derp_player->SetLocation(vec3(0.0f, 1.0f, 0.0f));
    derp_player->SetSound("apelsin");
    derp_player->SetRepeating(true);
    derp_player->Init();
    
    chamberpot.make();
    chamberpot->SetModel("chamberpot2");
    chamberpot->SetScale({0.2, 0.2, 0.2});
    chamberpot->Init();
    
    Event::AddListener(Event::TICK, [](Event& event) {
        chamberpot->SetLocation({0, 1, 0});
        //chamberpot->SetLocation({-10, 3, 0});
        chamberpot->SetRotation(quat(vec3{sinf(GetTickTime() / 3.0) / 3.0, GetTickTime(), cosf(GetTickTime() / 2.0) / 5.0}));
    });
    
    //derp_player->Play();
    
    Event::AddListener(Event::KEYPRESS, [](Event& event) {
        if (event.subtype != KEY_ACTION_ACTIVATE) return;
        
        vec3 start = Render::GetViewPosition();
        vec3 direction = Render::GetViewRotation() * DIRECTION_FORWARD;
        
        //auto result = Physics::Raycast(start, start + 4.0f * direction, -1 ^ Physics::COLL_TRIGGER);
        auto result = Physics::Raycast(start, start + 4.0f * direction, -1);
        
        if (result.collider) {
            Message::Send({Message::ACTIVATE, 0, result.collider->GetParent()->GetID(), 0});
        }
    });
    
    Event::AddListener(Event::KEYDOWN, [](Event& event) {
        if (event.subtype != KEY_ACTION_ACTIVATE) return;
        
        vec3 start = Render::GetViewPosition();
        vec3 direction = Render::GetViewRotation() * DIRECTION_FORWARD;
        
        auto result = Physics::Raycast(start, start + 2.0f * direction, -1 ^ Physics::COLL_TRIGGER);
        
        AddLineMarker(result.point, COLOR_CYAN);
        
        if (result.collider) {
            Message::Send({Message::ACTIVATE_ONCE, 0, result.collider->GetParent()->GetID(), 0});
        }
    });
    
    Event::AddListener(Event::TICK, [](Event& event) {
        vec3 start = Render::GetViewPosition();
        vec3 direction = Render::GetViewRotation() * DIRECTION_FORWARD;
        
        auto result = Physics::Raycast(start, start + 2.0f * direction, -1 ^ Physics::COLL_TRIGGER);

        if (result.collider) {
            Message::Send({Message::SELECT, 0, result.collider->GetParent()->GetID(), 0});
        }
    });
    
    Event::AddListener(Event::SELECTED, [](Event& event) {
        AddLineMarker(Entity::Find(event.poster)->GetLocation(), COLOR_PINK);
    });
    
    UI::BindKeyboardKey(UI::KEY_APOSTROPHE, [](){
        //WorldCell::Find(UID("demo_mov"))->Unload();
        
        //Script::LoadScript("bepis");
        
        /*
        camera->SetFollowing(nullptr);
        camera->SetMouselook(false);
        camera->SetLocation({0.0f, 25.0f, 0.0f});
        camera->SetRotation(vec3 {glm::radians(-90.0f), glm::radians(86.8f), 0.0f});
        
        std::cout << "aaa" << std::endl;
        
        Script::SetGlobal("nice_number", 420);
        
        Script::SetFunction("epic_function", {TYPE_INT32}, [](auto params) -> value_t {
            return (int32_t)params[0] + 69;
        });
        
        Script::LoadScript("bepis");
        
        vec3 v = Script::CallFunction("give_vector", {1, 2, 3});
        
        std::cout << v.x << " " << v.y << " " << v.z << std::endl;*/
    });
    
    Event::AddListener(Event::LOOK_AT, [](Event& event) {
        //AddLine(vec3(0, 0, 0), vec3(0, 0, 0) + (quat)*(Value*)event.data * DIRECTION_FORWARD, COLOR_CYAN);
    });
        
    
    UI::BindKeyboardKey(UI::KEY_T, [](){
        follower->Advance(0.01f);
    });
    
    UI::BindKeyboardKey(UI::KEY_R, [](){
        //record = !record;
    });
    
    UI::BindKeyboardKey(UI::KEY_O, [](){
        Ext::Menu::Menu* menu = new Ext::Kitchensink::KitchensinkMenuList;
        Ext::Menu::Menu::Push(menu);
        UI::SetInputState(UI::STATE_MENU_OPEN);
    });
    
    UI::BindKeyboardKey(UI::KEY_L, [](){
        Log(Severity::ERROR, System::MISC, "Key L was pressed and that now is an error.");
    });
    
    //UI::SetWindowSize(320, 240);
        
    //auto crate_ent = Entity::Find(UID("estijs"));
    
#ifdef __EMSCRIPTEN__
    UI::SetWebMainLoop(mainloop);
#else
    while(!UI::ShouldExit()){
        mainloop();
    }

    Async::Yeet();
    Ext::Scripting::Lua::Uninit();
    Audio::Uninit();
    UI::Uninit();
#endif
}

#endif
#ifndef NEKADEEE

#include <framework/core.h>
#include <framework/logging.h>
#include <framework/ui.h>
#include <framework/event.h>
#include <framework/message.h>
#include <render/render.h>
#include <physics/physics.h>
#include <entities/player.h>
#include <components/player.h>
#include <components/controller.h>

using namespace tram;
using namespace tram::UI;

#include <framework/core.h>
#include <framework/async.h>
#include <framework/ui.h>
#include <framework/entity.h>
#include <framework/worldcell.h>
#include <framework/loader.h>
#include <framework/event.h>
#include <framework/message.h>
#include <framework/language.h>
#include <framework/script.h>
#include <physics/physics.h>
#include <audio/audio.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>

#include <entities/crate.h>
#include <entities/staticworldobject.h>
#include <entities/light.h>
#include <entities/player.h>
#include <entities/sound.h>
#include <entities/decoration.h>
#include <entities/trigger.h>
#include <entities/marker.h>

#include <components/render.h>
#include <components/animation.h>
#include <components/light.h>

#include <components/sprite.h>
#include <components/particle.h>
#include <components/audio.h>

#include <components/controller.h>

#include <components/trigger.h>
#include <components/player.h>

#include <extensions/menu/menu.h>
#include <extensions/camera/camera.h>
#include <extensions/camera/firstperson.h>

#include <extensions/kitchensink/kitchensink.h>
#include <extensions/kitchensink/entities.h>
#include <extensions/kitchensink/soundtable.h>
#include <extensions/scripting/lua.h>

#include <render/api.h>
#include <render/scene.h>

#include <platform/platform.h>
#include <platform/image.h>

int main() {
    Light::Register();
    Crate::Register();
    Sound::Register();
    Decoration::Register();
    Trigger::Register();
    Marker::Register();
    StaticWorldObject::Register();
    Ext::Kitchensink::Button::Register();

    SetSystemLoggingSeverity(System::PLATFORM, Severity::WARNING);
    
	Core::Init();
	UI::Init();
	Render::Init();
	//Physics::Init();
	
	//Player* player = new Player();
	//player->SetLocation({0.0f, 1.0f, 0.0f});
	//player->Load();
	
	//Physics::DRAW_PHYSICS_DEBUG = true;
	
	while (!EXIT) {
		Core::Update();
		UI::Update();
		//Physics::Update();
		
		//vec3 v1 = {1.0f, 0.0f, 1.0f};
		//vec3 v2 = {-1.0f, 0.0f, 1.0f};
		//vec3 v3 = {1.0f, 0.0f, -1.0f};
		//vec3 v4 = {-1.0f, 0.0f, -1.0f};
		
		//Render::AddLine(v1, v2, Render::COLOR_WHITE);
		//Render::AddLine(v2, v4, Render::COLOR_WHITE);
		//Render::AddLine(v3, v4, Render::COLOR_WHITE);
		//Render::AddLine(v3, v1, Render::COLOR_WHITE);
		
		//ControllerComponent::Update();
		
        Event::Dispatch();
        Message::Dispatch();
        
		Render::Render();
		UI::EndFrame();
	}
}

#endif