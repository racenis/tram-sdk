// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/settings.h>

#include <framework/logging.h>
#include <templates/hashmap.h>
#include <templates/pool.h>

#include <vector>

/**
 * @namespace tram::Settings
 * 
 * Application setting management and CLI command parser.
 */

namespace tram::Settings {

const int SETTING_LIMIT = 200;

struct SettingInfo {
    union {
        int32_t* int32;
        uint32_t* uint32;
        float* float32;
        bool* bool32;
    };
    Type type;
    
    name_t name;
    uint32_t flags;
};

static SettingInfo settings[SETTING_LIMIT];
static size_t last_setting = 0;

static Hashmap<SettingInfo*> all_settings("settings map", SETTING_LIMIT);

static SettingInfo* lookup_setting(name_t name) {
    auto setting = all_settings.Find(name);
    
    if (!setting) {
        for (size_t i = 0; i < last_setting; i++) {
            if (settings[i].name != name) continue;
            
            setting = &settings[i];
            
            all_settings.Insert(name, setting);
        }
    }
    
    return setting;
} 

// raw settings store the values for settings that have been parsed, but haven't
// been registered yet
struct RawSetting {
    const char* key;
    const char* value;
    bool maybe_flag;
};

static std::vector<RawSetting> raw_settings;

void SetFromRaw(SettingInfo& info) {
    if (!info.bool32) {
        Log("Setting {} has no data pointer!", info.name);
        return;
    }
    
    // check if setting has already been parsed
    for (auto& setting : raw_settings) {
        if (strcmp(setting.key, info.name) != 0) continue;
        
        switch (info.type) {
            case TYPE_BOOL:
                if (setting.maybe_flag) {
                    *info.bool32 = true;
                } else if (setting.value && strcmp(setting.value, "false") == 0) {
                    *info.bool32 = false;
                } else {
                    *info.bool32 = true;
                }
                break;
            case TYPE_FLOAT32:
                if (!setting.value) {
                    Log("cannot set {} since ther eisno value", info.name);
                }
                *info.float32 = atof(setting.value);
                break;
            case TYPE_INT32:
                if (!setting.value) {
                    Log("cannot set {} since ther eisno value", info.name);
                }
                *info.int32 = atoi(setting.value);
                break;
            case TYPE_UINT32:
                if (!setting.value) {
                    Log("cannot set {} since ther eisno value", info.name);
                }
                *info.uint32 = atoi(setting.value);
                break;
            default:
                Log("hello setting invalid");
            
        }
    } 
        
    
}

void SetAndStore(SettingInfo info) {
    if (lookup_setting(info.name)) {
        Log("Setting '{}' already registered, skipping!", info.name);
        return;
    }
    
    SetFromRaw(info);
    settings[last_setting++] = info;
}

void Register(bool& value, const char* name, uint32_t flags) {
    SetAndStore(SettingInfo{.bool32 = &value, .type = TYPE_BOOL, .name = name, .flags = flags});
}

void Register(float& value, const char* name, uint32_t flags) {
    SetAndStore(SettingInfo{.float32 = &value, .type = TYPE_FLOAT32, .name = name, .flags = flags});
}

void Register(int32_t& value, const char* name, uint32_t flags) {
    SetAndStore(SettingInfo{.int32 = &value, .type = TYPE_INT32, .name = name, .flags = flags});
}

void Register(uint32_t& value, const char* name, uint32_t flags) {
    SetAndStore(SettingInfo{.uint32 = &value, .type = TYPE_UINT32, .name = name, .flags = flags});
}



Value Get(name_t name) {
    auto setting = lookup_setting(name);
    
    if (!setting) return Value();
    
    switch (setting->type) {
        case TYPE_BOOL:
            return *setting->bool32;
            break;
        case TYPE_FLOAT32:
            return *setting->float32;
            break;
        case TYPE_INT32:
            return *setting->int32;
            break;
        case TYPE_UINT32:
            return *setting->uint32;;
            break;
        default:
            return false;
        
    }
}

void Set(name_t name, Value value) {
    auto setting = all_settings.Find(name);
    
    if (!setting) {
        for (size_t i = 0; i < last_setting; i++) {
            if (settings[i].name != name) continue;
            
            setting = &settings[i];
            
            all_settings.Insert(name, setting);
        }
    }
    
    if (setting) {
        if (value.GetType() != setting->type) {
            Log("Trying to set a setting to {}, but it is a {}", TypeToString(value.GetType()), TypeToString(setting->type));
            return;
        }
        
        if (value.IsInt()) {
            *setting->int32 = value.GetInt();
        } else if (value.IsFloat()) {
            *setting->int32 = value.GetFloat();
        } else if (value.IsBool()) {
            *setting->bool32 = value;
        }
    }
}


void Parse(const char** argv, int argc) {
    Log("Parsing settings from CLI");
    
    for (int i = 1; i < argc; i++) {
        const char* name = argv[i];
        
        if (*name != '-') {
            Log(Severity::WARNING, System::CORE, "Unrecognized CLI parameter: {}", name);
            continue;
        }
        
        name++;
        
        const bool is_another_one = i + 1 < argc;
        const char* another_one = is_another_one ? argv[i + 1] : nullptr;

        RawSetting setting;
        
        // check if is a flag
        if (is_another_one && *another_one == '-') {
            
            setting.key = name;
            setting.value = another_one;
            
            setting.maybe_flag = true;
            
            raw_settings.push_back(setting);
            
            continue;
        }
        
        setting.key = name;
        setting.value = another_one;
        
        setting.maybe_flag = false;
        
        raw_settings.push_back(setting);
        
        i++;
    }
    
    Log("Applying CLI values to settings");
    
    for (size_t i = 0; i < last_setting; i++) SetFromRaw(settings[i]);
}

uint32_t Flags(name_t name) {
    auto setting = lookup_setting(name);
    
    if (!setting) return 0;
    
    return setting->flags;
}

std::vector<name_t> GetSettings(uint32_t filter) {
    std::vector<name_t> filtered;
    
    for (size_t i = 0; i < last_setting; i++) {
        if (settings[i].flags & filter) {
            filtered.push_back(settings[i].name);
        }
    }

    return filtered;
}

void Save(const char* file) {
    // TODO: implement
}

void Load(const char* file) {
    // TODO: implement
}


}