// This file is mostly for testing.
// If you're using this as a library, then you need to implement your own main() function.

#include <iostream>

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
#include <components/decal.h>
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
#include <extensions/kitchensink/probe.h>
#include <extensions/kitchensink/imageassembly.h>
#include <extensions/scripting/lua.h>

#include <extensions/kitchensink/dialog.h>
#include <extensions/kitchensink/quest.h>
#include <extensions/kitchensink/menu.h>

#include <render/api.h>
#include <render/scene.h>
#include <render/light.h>

#include <platform/platform.h>
#include <platform/api.h>
#include <platform/image.h>

#include <render/raytrace/raytrace.h>

using namespace tram;
using namespace tram::Render;
using namespace tram::UI;

Ext::Camera::FirstPersonCamera* camera = nullptr;
Player* player = nullptr;
LightComponent* lit = nullptr;
Sprite* tolet_sprite = nullptr;
SpriteComponent* tolet_spinner = nullptr;
ParticleComponent* tolet_emitter = nullptr;
DecalComponent* tolet_sprayer = nullptr;
Audio::Sound* derp = nullptr;
Audio::Sound* eerp = nullptr;
AudioComponent* derp_player = nullptr;
RenderComponent* binguser = nullptr;
RenderComponent* monguser = nullptr;
RenderComponent* dingbat = nullptr;
AnimationComponent* monguser_armature = nullptr;

bool record = false;

PathFollower* follower = nullptr;
vec3 initial_follower = {0.1f, 1.0f, 1.5f};

Component<RenderComponent> chamberpot;

void mainloop();

int main(int argc, const char** argv) {
    Settings::Parse(argv, argc);
    
    // register the entity types, so that they can be loaded from level files
    Light::Register();
    Crate::Register();
    Sound::Register();
    Decoration::Register();
    Trigger::Register();
    StaticWorldObject::Register();
    Ext::Kitchensink::Button::Register();

    Platform::Window::SetScale(3);

    Core::Init();           // core init should always be first
    UI::Init();
    Physics::Init();        // optional, but needed for StaticWorldObject, Crate and Player entities
    Render::Init();         // render init must always come after the ui inited
    Async::Init();          // async init must always come after render init
    Audio::Init();
    GUI::Init();

    Ext::Menu::Init();
    Ext::Camera::Init();
    
    Ext::Scripting::Lua::Init();

    Script::Init();
    
    Script::LoadScript("init");

    // load all of the language strings
    Language::Load("lv");

    // texture info stuff
    Material::LoadMaterialInfo("material");

    Animation::Find(UID("mongus-run"))->LoadFromDisk();
    Animation::Find(UID("mongus-jump"))->LoadFromDisk();
    
    // data test files for kitchensink framework
    Ext::Kitchensink::DialogTopic::LoadFromDisk("dialogs");
    Ext::Kitchensink::Quest::LoadFromDisk("quests");
    Ext::Kitchensink::Inventory::LoadFromDisk("items");
    
    Ext::Kitchensink::RegisterRenderEnvironmentMapShortcut();

    Render::API::SetScreenClear({0.0f, 0.0f, 0.0f}, true);

    // loading the demo level
    LightGraph::Find("demo_mov")->Load();
    WorldCell::Make(UID("demo_mov"))->LoadFromDisk();
    

    // create the player entity
    player = new Player;
    player->SetLocation(vec3(0.0f, (1.85f/2.0f) + 0.15f, 0.0f));
    player->Load();
    
    // have world streaming control track player
    Loader::Track(player);
    
    player->controllercomponent->SetFriction(0.82f);
    player->controllercomponent->SetWalkSpeed(0.2f);
    player->controllercomponent->SetRunSpeed(0.3f);
    
    camera = new Ext::Camera::FirstPersonCamera;
    camera->SetMouselook(true);
    camera->SetRotateFollowing(true);
    camera->SetFollowingOffset({0.0f, 0.5f, 0.0f});
    camera->SetFollowing(player);
    
    camera->SetBobbingDistance(0.1f);
    camera->SetBobbingTilt(0.1f);

    camera->SetActive();
    
    // load up the path that mongus will run around
    Path::Find("test")->LoadFromDisk();
    follower = new PathFollower(Path::Find("test"), initial_follower, PATH_LINEAR);
    
    // initialize 400 chamberpots to test frustrum culling
    /*for (int x = 0; x < 20; x++) {
        for (int y = 0; y < 20; y++) {
            RenderComponent* pot = PoolProxy<RenderComponent>::New();
            pot->SetModel("chamberpot2");
            pot->SetScale({0.2, 0.2, 0.2});
            pot->SetLocation({10 - x, 25, 10 - y});
            pot->Init();
        }
    }*/
    
    auto teapotmat = Material::Find("Material.01");
    teapotmat->Load();
    Render::API::SetMaterialAssemblyIndex(teapotmat->GetMaterial(), 1);
    Render::API::SetUseAssembly(true);
    //AssemblyLayers GetAssemblyLayers();
    
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
    
    tolet_sprayer = PoolProxy<DecalComponent>::New();
    tolet_sprayer->SetSprite(tolet_sprite);
    tolet_sprayer->Init();
    
    derp = Audio::Sound::Find(UID("apelsin"));
    //eerp = Audio::Sound::Find(UID("dekpunkta"));
    derp->Load();
    //eerp->Load();
    
    derp_player = PoolProxy<AudioComponent>::New();
    derp_player->SetLocation(vec3(0.0f, 1.0f, 0.0f));
    derp_player->SetSound("apelsin");
    derp_player->SetRepeating(true);
    derp_player->Init();
    
    //derp_player->Play();
    
    dingbat = PoolProxy<RenderComponent>::New();
    dingbat->SetModel("sphere");
    dingbat->SetScale({0.2f, 0.2f, 0.2f});
    dingbat->SetDirectionaLight(false);
    //dingbat->Init();
    
    chamberpot.make();
    chamberpot->SetModel("chamberpot2");
    chamberpot->SetScale({0.2, 0.2, 0.2});
    chamberpot->Init();
    
    Event::AddListener(Event::TICK, [](Event& event) {
        chamberpot->SetLocation({0, 1, 0});
        //chamberpot->SetLocation({-10, 3, 0});
        chamberpot->SetRotation(quat(vec3{sinf(GetTickTime() / 3.0) / 3.0, GetTickTime(), cosf(GetTickTime() / 2.0) / 5.0}));
        chamberpot->SetEnvironmentMap(Render::LightGraph::LookupEnvironmentMap({0, 1, 0}));
    });
    
    
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
        
        //AddLineMarker(result.point, COLOR_CYAN);
        
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
        //AddLineMarker(Entity::Find(event.poster)->GetLocation(), COLOR_PINK);
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
    
    UI::BindKeyboardKey(UI::KEY_KP_ADD, [](){
        Render::API::SetInteractiveMode(!Render::API::IsInteractiveMode());
    });
    
    
    UI::BindKeyboardKey(UI::KEY_UP, [](){
        Core::SetPlatformTime(true);
    });
    
    UI::BindKeyboardKey(UI::KEY_DOWN, [](){
        Core::SetPlatformTime(false);
    });
    
    UI::BindKeyboardKey(UI::KEY_LEFT, [](){
        Core::AddTime(-1.0f / 30.0f);
    });
    
    UI::BindKeyboardKey(UI::KEY_RIGHT, [](){
        Core::AddTime(1.0f / 30.0f);
    });
    
    
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


void mainloop() {
    Core::Update();
    UI::Update();
    
    // this renders debug text at the origin
    //Render::AddText({0, 0, 0}, "HELLOOO");
    
    //vec3 ff = LightGraph::LookupHarmonic(Render::GetViewPosition(), -1).l00;
    //Render::AddText(20, 20, std::to_string(ff.x).c_str(), ff);
    //Render::AddText(20, 40, std::to_string(ff.y).c_str(), ff);
    //Render::AddText(20, 60, std::to_string(ff.z).c_str(), ff);
    
    if (API::IsFinishedRendering()) {
        API::SetInteractiveMode(true);
        
        Kitchensink::ImageAssembly::Add(API::GetAssemblyLayers());
        Kitchensink::ImageAssembly::Save("assembly.image");
        Kitchensink::ImageAssembly::Reset();
    }
    
    dingbat->SetLocation(Render::GetViewPosition() + Render::GetViewRotation() * DIRECTION_FORWARD);
    //tolet_sprayer->SetLocation(Render::GetViewPosition() + Render::GetViewRotation() * DIRECTION_FORWARD);
    //tolet_sprayer->SetRotation(Render::GetViewRotation());
    //tolet_sprayer->SetFrame((GetTick() >> 2) % 24);
    //tolet_sprayer->SetFrame(GetTick() % 24);
    //tolet_sprayer->SetFrame(0);
    
    // have mongus run around on a path
    follower->Advance(GetDeltaTime());
    
    Entity* mongus = Entity::Find("mongus");
    if (mongus && GetTick() > 120) {
        mongus->SetLocation(follower->GetPosition() - 0.7f * DIRECTION_UP);
        mongus->SetRotation(glm::quatLookAt(follower->GetTangent(), DIRECTION_UP));
    }
    
    // change FOV with plus and minus
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
    
    // animate camera when strafing
    if (UI::PollKeyboardKey(UI::KEY_A)) {
        camera->SetTilt(0.1f);
    } else if (UI::PollKeyboardKey(UI::KEY_D)) {
        camera->SetTilt(-0.1f);
    } else {
        camera->SetTilt(0.0f);
    }
    
    // make camera bob when walking
    if (UI::PollKeyboardKey(UI::KEY_W) || UI::PollKeyboardKey(UI::KEY_S) ||
        UI::PollKeyboardKey(UI::KEY_A) || UI::PollKeyboardKey(UI::KEY_D)) {
        camera->SetBobbing(1.0f);
    } else {
        camera->SetBobbing(0.0f);
    }
    
    Ext::Camera::Update();
    
    // draw default menus
    GUI::Begin();
        Ext::Menu::Update();
    GUI::End();
    
    // draw frame numbers at the top of the screen
    std::string tickstr = std::to_string(GetTick());
    std::string tickstr2 = std::to_string(GetTickTime());

    GUI::Begin();
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 20);
        GUI::SetFont(Ext::Menu::FONT_PIXELART);
        GUI::SetColor(COLOR_WHITE);
        
            //GUI::Text(tickstr.c_str(), GUI::TEXT_CENTER); GUI::NewLine();
            //GUI::Text(tickstr2.c_str(), GUI::TEXT_CENTER); GUI::NewLine();
            
        GUI::RestoreColor();
        GUI::RestoreFont();
    GUI::PopFrame();
    GUI::End();
    GUI::Update();
    
    // if recording, take screenshots and save them as a sequence of images
    if (record && GetTick() % 4 == 0) {
        char* buffer = (char*)malloc(GetScreenWidth() * GetScreenHeight() * 3);
        std::string filename = std::string("screenshot") + std::to_string(GetTick()) + ".png";
        Render::API::GetScreen(buffer, GetScreenWidth(), GetScreenHeight());
        Platform::SaveImageToDisk(filename.c_str(), GetScreenWidth(), GetScreenHeight(), buffer);
        free(buffer);
    }
    
    Audio::Update();
    
    Async::LoadResourcesFromDisk();
    Async::LoadResourcesFromMemory();
    Async::FinishResources();

    Event::Dispatch();
    Message::Dispatch();
    
    Entity::Update();
    
    Loader::Update();
    
    if (!Async::GetWaitingResources()) ControllerComponent::Update();
    AnimationComponent::Update();
    //RenderComponent::DrawAllAABB();

    Physics::Update();

    Render::Render();

    Stats::Collate();

    UI::EndFrame();
}
