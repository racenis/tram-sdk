// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

// This is the configuration file for the library portion of the framework.
// If you change anything in here, you'll need to recompile everything.

#ifndef TRAM_SDK_CONFIG_H
#define TRAM_SDK_CONFIG_H

#include <cstdint>

namespace tram {

const uint32_t MAX_MATERIAL_TYPES = 20;
const uint32_t MAX_VERTEX_FORMATS = 20;

const uint32_t MAX_VERTEX_SHADERS = 20;
const uint32_t MAX_FRAGMENT_SHADERS = 20;
const uint32_t MAX_LINKED_SHADERS = 40;




// +---------------------------------------------------------------------------+
// |                                                                           |
// |                              RESOURCE LIMITS                              |
// |                                                                           |
// +---------------------------------------------------------------------------+

// These numbers determine how much memory will be allocated for each Resource
// type. 

// Currently the Resource pools won't get automatically extended if they run out
// of space, so essentially these numbers limit how many instances of a
// Resource type it will be possible to create.

const uint32_t RESOURCE_LIMIT_SOUND = 100;
const uint32_t RESOURCE_LIMIT_MODEL = 100;

// +---------------------------------------------------------------------------+
// |                                                                           |
// |                             COMPONENT LIMITS                              |
// |                                                                           |
// +---------------------------------------------------------------------------+

// Same as for Resources.

const uint32_t COMPONENT_LIMIT_ANIMATION = 50;
const uint32_t COMPONENT_LIMIT_AUDIO = 150;
const uint32_t COMPONENT_LIMIT_CONTROLLER = 25;
const uint32_t COMPONENT_LIMIT_LIGHT = 250;
const uint32_t COMPONENT_LIMIT_PARTICLE = 100;
const uint32_t COMPONENT_LIMIT_PHYSICS = 500;
const uint32_t COMPONENT_LIMIT_PLAYER = 5;
const uint32_t COMPONENT_LIMIT_RENDER = 1000;
const uint32_t COMPONENT_LIMIT_SPRITE = 100;
const uint32_t COMPONENT_LIMIT_TRIGGER = 50;


// +---------------------------------------------------------------------------+
// |                                                                           |
// |                               OTHER LIMITS                                |
// |                                                                           |
// +---------------------------------------------------------------------------+

// Limit for file path length, e.g. "data/textures/material001.png"
const size_t PATH_LIMIT = 200;

// Limit for scriptable entity types
const uint32_t SCRIPTABLE_ENTITY_TYPE_LIMIT = 50;


}

#endif // TRAM_SDK_CONFIG_H