// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/light.h>

#include <config.h>

using namespace tram;

template<> tram::Pool<tram::Render::API::GLLight> tram::PoolProxy<tram::Render::API::GLLight>::pool("GLLight pool", COMPONENT_LIMIT_LIGHT, {tram::Render::API::GLLight{}});

namespace tram::Render::API {

light_t MakeLight() {
    return {PoolProxy<GLLight>::New()};
}

void YeetLight(light_t light) {
    PoolProxy<GLLight>::Delete(light.gl);
}

void SetLightLocation(light_t light, vec3 location) {
    light.gl->location = location;
}

void SetLightColor(light_t light, vec3 color) {
    light.gl->color = color;
}

void SetLightDistance(light_t light, float distance) {
    light.gl->distance = distance;
}

void SetLightDirection(light_t light, vec3 direction, float exponent) {
    light.gl->direction = direction;
    light.gl->exponent = exponent;
}

}