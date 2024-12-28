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

// Rate of fixed updates
const float TICK_RATE = 1.0f / 60.0f;

// Initial first person camera sensitivity
const float CAMERA_MULTIPLIER = 1.0f / 8.0f;

// +---------------------------------------------------------------------------+
// |                                                                           |
// |                                  PHYSICS                                  |
// |                                                                           |
// +---------------------------------------------------------------------------+

const float PHYSICS_GROUND_PLANE = -9.0f;
const float PHYSICS_GRAVITY = -9.8f;

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
const uint32_t RESOURCE_LIMIT_PATH = 100;
const uint32_t RESOURCE_LIMIT_COLLISION_MODEL = 500;
const uint32_t RESOURCE_LIMIT_MATERIAL = 500;
const uint32_t RESOURCE_LIMIT_SPRITE = 500;

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

// Limit for system count
const size_t SYSTEM_LIMIT = 128;

// Limit for scriptable entity types
const uint32_t SCRIPTABLE_ENTITY_TYPE_LIMIT = 50;

// Limit for all entity types
const uint32_t ENTITY_TYPE_LIMIT = 50;

// Limits for Async system resoruce streaming requests
const uint32_t RESOURCE_LOADER_QUEUE_LIMIT = 1000;
const uint32_t RESOURCE_LOADER_REQUEST_LIMIT = 1750;

// Limits for entities
const uint32_t ENTITY_LIMIT = 5000;
const uint32_t NAMED_ENTITY_LIMIT = 500;

// Limit for total sum of characters that can be stored in all language strings
const uint32_t LANUGAGE_STRING_CHARACTER_LIMIT = 10000;

// Limit for the number of lanugage strings
const uint32_t LANGUAGE_STRING_LIMIT = 500;

// Limit for enqueued messages before dispatch
const uint32_t MESSAGE_QUEUE_LIMIT = 500;

// Limit for enqueued messages with a delay
const uint32_t MESSAGE_PRIORITY_QUEUE_LIMIT = 250;

// Limit for message data, in bytes
const uint32_t MESSAGE_DATA_LIMIT = 2000;

// Limit for message types
const uint32_t MESSAGE_TYPE_LIMIT = 100;

// Limit for signal types
const uint32_t SIGNAL_TYPE_LIMIT = 100;

const uint32_t WORLDCELL_TRANSITION_LIMIT = 250;
const uint32_t KEYBOARDACTION_LIMIT = 100;

// Limit for total sum of characters that can be stored in all names
const uint32_t UID_CHARACTER_LIMIT = 20000;

// Limit for the number of names
const uint32_t UID_LIMIT = 2000
;
const uint32_t WORLDCELL_LIMIT = 250;

const uint32_t RIGIDBODY_LIMIT = 500;

// +---------------------------------------------------------------------------+
// |                                                                           |
// |                             EXTENSION LIMITS                              |
// |                                                                           |
// +---------------------------------------------------------------------------+


const uint32_t EXT_KITCHENSINK_ANIMATIONTABLE_LIMIT = 100;
const uint32_t EXT_KITCHENSINK_SOUNDTABLE_LIMIT = 100;
}

#endif // TRAM_SDK_CONFIG_H