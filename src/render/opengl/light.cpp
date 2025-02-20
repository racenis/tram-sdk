// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/light.h>

#include <config.h>

using namespace tram;

template<> tram::Pool<tram::Render::API::GLLight> tram::PoolProxy<tram::Render::API::GLLight>::pool("GLLight pool", COMPONENT_LIMIT_LIGHT, {tram::Render::API::GLLight{}});

namespace tram::Render::API {

light_t MakeLight() {
    return {PoolProxy<GLLight>::New()};
}

void DeleteLight(light_t light) {
    PoolProxy<GLLight>::Delete(light.gl);
}

void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent) {
    GLLight* light_ptr = light.gl;
    
    light_ptr->location = location;
    light_ptr->color = color;
    light_ptr->distance = distance;
    light_ptr->direction = direction;
    light_ptr->exponent = exponent;
}

}