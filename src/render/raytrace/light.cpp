// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/light.h>

template<> tram::Pool<tram::Render::API::RTLight> tram::PoolProxy<tram::Render::API::RTLight>::pool("light list", 200);

namespace tram::Render::API {

light_t MakeLight() {
    return light_t {.rt = PoolProxy<RTLight>::make()};
}

void YeetLight(light_t light) {
    PoolProxy<RTLight>::yeet(light.rt);
}

void SetLightLocation(light_t light, vec3 location) {
    light.rt->location = location;
}

void SetLightColor(light_t light, vec3 color) {
    light.rt->color = color;
}

void SetLightDistance(light_t light, float distance) {
    light.rt->distance = distance;
}

void SetLightDirection(light_t light, vec3 direction, float exponent) {
    light.rt->direction = direction;
    light.rt->exponent = exponent;
}

}