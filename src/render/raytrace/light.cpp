// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/light.h>

template<> tram::Pool<tram::Render::API::RTLight> tram::PoolProxy<tram::Render::API::RTLight>::pool("light list", 200);

namespace tram::Render::API {

light_t MakeLight() {
    return light_t {.rt = PoolProxy<RTLight>::New()};
}

void DeleteLight(light_t light) {
    PoolProxy<RTLight>::Delete(light.rt);
}

void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent) {
    RTLight* light_ptr = light.rt;
    
    light_ptr->location = location;
    light_ptr->color = color;
    light_ptr->distance = distance;
    light_ptr->direction = direction;
    light_ptr->exponent = exponent;
}

}