#include <framework/system.h>
#include <vector>

namespace Core::System {
    struct SystemInfo {
        char const* name;
        char const* short_name;
        bool is_initialized;
    };

    std::vector<SystemInfo> ALL_SYSTEMS;

    uint32_t Register (char const* name, char const* short_name) {
        static uint32_t last_issued_ID = 0;
        
        ALL_SYSTEMS.push_back(SystemInfo{
            .name = name,
            .short_name = short_name,
            .is_initialized = false
        });
        
        return last_issued_ID++;
    }

    char const* GetName (uint32_t system) {
        return ALL_SYSTEMS[system].name;
    }

    char const* GetShortName (uint32_t system) {
        return ALL_SYSTEMS[system].short_name;
    }
    
    void SetInitialized (uint32_t system, bool is_initialized) {
        ALL_SYSTEMS[system].is_initialized = is_initialized;
    }
    
    bool IsInitialized (uint32_t system) {
        return ALL_SYSTEMS[system].is_initialized;
    }
}
