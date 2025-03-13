// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/kitchensink/probe.h>

#include <render/light.h>
#include <render/api.h>

#include <framework/ui.h>
#include <framework/gui.h>
#include <framework/logging.h>

#include <platform/image.h>

namespace tram::Ext::Kitchensink {

void RegisterRenderEnvironmentMapShortcut() {
    UI::BindKeyboardKey(UI::KEY_KP_5, []() {
        auto cell = WorldCell::Find(Render::GetViewPosition());
        
        if (!cell) {
            Log("Couldn't find cell for capture!");
        }
        
        RenderEnvironmentMaps(cell);
    });
}

void RenderEnvironmentMaps(WorldCell* cell) {
    // PoolProxy<Render::LightGraph>::GetPool()
    auto graph = Render::LightGraph::Find(cell->GetName());
    
    if (graph->GetStatus() != Resource::READY) {
        Log("Cell graph {} not loaded!", cell->GetName());
        return;
    }
    
    Log("Capturing cell {}...", cell->GetName());

    const int map_size = 512;
    
    // back up screen parameters
    auto screen_width = UI::GetScreenWidth();
    auto screen_height = UI::GetScreenHeight();
    auto view_fov = Render::GetViewFov();
    
    // set up screen for cubemap rendering
    Render::SetViewFov(90.0f);
    Render::SetScreenSize(map_size, map_size);
    
    GUI::Update();
    
    Render::Render(); UI::EndFrame();
    Render::Render(); UI::EndFrame();
    
    // allocate buffers
    char* front = (char*)malloc(map_size * map_size * 3);   // negative z
    char* left = (char*)malloc(map_size * map_size * 3);    // negative x
    char* right = (char*)malloc(map_size * map_size * 3);   // positive x
    char* back = (char*)malloc(map_size * map_size * 3);    // positive z
    char* top = (char*)malloc(map_size * map_size * 3);     // positive y
    char* bottom = (char*)malloc(map_size * map_size * 3);  // negative y
    
    char* sphere_front = (char*)malloc(map_size * map_size * 3);
    char* sphere_back = (char*)malloc(map_size * map_size * 3);
    
    char* sphere_output = (char*)malloc(map_size * 2 * map_size * 3);
    
    // step through each environment probe position and render the environment map
    for (auto [id, probe] : graph->GetEnvironmentProbes()) {
        
        auto get_coords = [](vec2 coords) {
            int x = floorf(coords.x * (float)map_size);
            int y = floorf(coords.y * (float)map_size);
            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x >= map_size) x = map_size - 1;
            if (y >= map_size) y = map_size - 1;
            return std::pair<int, int>{x, y};
        };
        
        auto get_index = [=](vec2 coords) {
            auto [x, y] = get_coords(coords);
            return 3 * (y * map_size + x);
        };
        
        auto get_pixel = [=](char* image, vec2 coords) {
            auto index = get_index(coords);
            return std::make_tuple(image[index], image[index + 1], image[index + 2]);
        };
        
        auto set_pixel = [=](char* image, vec2 coords, std::tuple<char, char, char> color) {
            auto index = get_index(coords);
            image[index + 0] = std::get<0>(color);
            image[index + 1] = std::get<1>(color);
            image[index + 2] = std::get<2>(color);
        };
        
        auto sample_cubemap = [=](vec3 nrm) {
            char* image = nullptr;
            float axis; vec2 uv;

            vec3 abs = glm::abs(nrm);

            bool x_positive = nrm.x > 0;
            bool y_positive = nrm.y > 0;
            bool z_positive = nrm.z > 0;

            if (z_positive && abs.z >= abs.x && abs.z >= abs.y) {
                image = front;
                uv.x = nrm.x;
                uv.y = nrm.y;
                axis = abs.z;
            } else if (!x_positive && abs.x >= abs.y && abs.x >= abs.z) {
                image = left;
                uv.x = nrm.z;
                uv.y = nrm.y;
                axis = abs.x;
            }  else if (x_positive && abs.x >= abs.y && abs.x >= abs.z) {
                image = right;
                uv.x = -nrm.z;
                uv.y = nrm.y;
                axis = abs.x;
            }  else if (!z_positive && abs.z >= abs.x && abs.z >= abs.y) {
                image = back;
                uv.x = -nrm.x;
                uv.y = nrm.y;
                axis = abs.z;
            }  else if (y_positive && abs.y >= abs.x && abs.y >= abs.z) {
                image = top;
                uv.x = nrm.x;
                uv.y = -nrm.z;
                axis = abs.y;
            }  else if (!y_positive && abs.y >= abs.x && abs.y >= abs.z) {
                image = bottom;
                uv.x = nrm.x;
                uv.y = nrm.z;
                axis = abs.y;
            }

            uv = 0.5f * (uv / axis + 1.0f);

            if (!image) return std::tuple<char, char, char>{uv.y * 255.0f, 0, uv.x * 255.0f};

            return get_pixel(image, uv);
        };
        
        
        
        
        
        // render the cubemap
        Render::SetViewPosition(probe);
        Render::SetViewRotation(vec3(0.0f, 0.0f, 0.0f));
        Render::Render(); UI::EndFrame(); Render::Render(); UI::EndFrame();
        
        Render::API::GetScreen(front, map_size, map_size);
        
        
        Render::SetViewRotation(vec3(0.0f, 3.1415f / 2.0f, 0.0f));
        Render::Render(); UI::EndFrame(); Render::Render(); UI::EndFrame();
        
        Render::API::GetScreen(left, map_size, map_size);
        
        
        Render::SetViewRotation(vec3(0.0f, -(3.1415f / 2.0f), 0.0f));
        Render::Render(); UI::EndFrame(); Render::Render(); UI::EndFrame();
        
        Render::API::GetScreen(right, map_size, map_size);
        
        
        Render::SetViewRotation(vec3(0.0f, 3.1415f, 0.0f));
        Render::Render(); UI::EndFrame(); Render::Render(); UI::EndFrame();
        
        Render::API::GetScreen(back, map_size, map_size);
        
        
        Render::SetViewRotation(vec3(3.1415f / 2.0f, 0.0f, 0.0f));
        Render::Render(); UI::EndFrame(); Render::Render(); UI::EndFrame();
        
        Render::API::GetScreen(top, map_size, map_size);
        
        
        Render::SetViewRotation(vec3(-(3.1415f / 2.0f), 0.0f, 0.0f));
        Render::Render(); UI::EndFrame(); Render::Render(); UI::EndFrame();
        
        Render::API::GetScreen(bottom, map_size, map_size);
        
        // convert cubemap into double sphere maps
        for (float u = 0.0f; u < 1.0f; u += 1.0f / (float)map_size) {
            for (float v = 0.0f; v < 1.0f; v += 1.0f / (float)map_size) {            
                float x = u * 2.0f - 1.0f;
                float y = v * 2.0f - 1.0f;

                //float z = 1.0f / sqrtf(1 + x * x + y * y);
                float z = sqrtf(1 - x * x - y * y);

                auto color_front = sample_cubemap({x, y, z});
                auto color_back = sample_cubemap({x, y, -z});
                
                set_pixel(sphere_front, {u, v}, color_front);
                set_pixel(sphere_back, {u, v}, color_back);
            }
        }
        
        // put both sphere maps next to each other in a final single image
        for (int x = 0; x < map_size; x++) {
            for (int y = 0; y < map_size; y++) {
                auto offset_in = (y * map_size + x) * 3;
                auto offset_o1 = (y * 2 * map_size + x) * 3;
                auto offset_o2 = (y * 2 * map_size + x + map_size) * 3;
                
                sphere_output[offset_o1 + 0] = sphere_front[offset_in + 0];
                sphere_output[offset_o1 + 1] = sphere_front[offset_in + 1];
                sphere_output[offset_o1 + 2] = sphere_front[offset_in + 2];
                sphere_output[offset_o2 + 0] = sphere_back[offset_in + 0];
                sphere_output[offset_o2 + 1] = sphere_back[offset_in + 1];
                sphere_output[offset_o2 + 2] = sphere_back[offset_in + 2];
            }
        }
        
        auto filename = std::string("data/textures/environment/") + (const char*)graph->GetName() + "." + std::to_string(id);
        
        Platform::SaveImageToDisk((filename + ".png").c_str(), 2 * map_size, map_size, sphere_output);
    }
    
    // release buffers
    free(front);
    free(left);
    free(right);
    free(back);
    free(top);
    free(bottom);
    
    free(sphere_front);
    free(sphere_back);
    
    // reset rendering
    Render::SetScreenSize(screen_width, screen_height);
    Render::SetViewFov(view_fov);
}

}