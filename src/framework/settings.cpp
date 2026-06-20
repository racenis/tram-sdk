// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/settings.h>

#include <framework/logging.h>
#include <framework/file.h>
#include <framework/ui.h>
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
    
    const char* name;
    uint32_t flags;
    
    void (*callback)(const char* name) = nullptr;
};

static SettingInfo settings[SETTING_LIMIT];
static size_t last_setting = 0;

static Hashmap<SettingInfo*> all_settings("settings map", SETTING_LIMIT);

// okay so if you enter anything into the hashmap before the it's constructed?
// instant crash. the problem is that the hashmap is touched by properties that
// are like static? and depending on how the program is linked, they may decide
// to initialize before the hashmap has been initialized. and then it crashes.
// but if the parse command is called, that means that the main function has
// started to run and the hashmap has been totally initialized, so we can start
// using it. so that's why we have this variable here. it's so that we don't get
// UB and NO it's NOT the fun kind of UB.
static bool parsed = false;

static SettingInfo* lookup_setting(const char* name) {
    auto setting = parsed ? all_settings.find(name) : nullptr;
    
    if (!setting) {
        for (size_t i = 0; i < last_setting; i++) {
            if (strcmp(settings[i].name, name)) continue;
            
            setting = &settings[i];
            
            all_settings.insert(name, setting);
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
        Log(Severity::WARNING, System::CORE, "Setting {} has no data pointer!", info.name);
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
                    Log(Severity::WARNING, System::CORE, "cannot set {} since ther eisno value", info.name);
                }
                *info.float32 = atof(setting.value);
                break;
            case TYPE_INT32:
                if (!setting.value) {
                    Log(Severity::WARNING, System::CORE, "cannot set {} since ther eisno value", info.name);
                }
                *info.int32 = atoi(setting.value);
                break;
            case TYPE_UINT32:
                if (!setting.value) {
                    Log(Severity::WARNING, System::CORE, "cannot set {} since ther eisno value", info.name);
                }
                *info.uint32 = atoi(setting.value);
                break;
            default:
                Log(Severity::WARNING, System::CORE, "hello setting invalid");
            
        }
    } 
        
    
}

void SetAndStore(SettingInfo info) {
    if (lookup_setting(info.name)) {
        Log(Severity::WARNING, System::CORE, "Setting '{}' already registered, skipping!", info.name);
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

void SetCallback(const char* name, void (*callback)(const char* name)) {
    auto setting = lookup_setting(name);
    if (setting) {
        setting->callback = callback;
    }
}

value_t Get(const char* name) {
    auto setting = lookup_setting(name);
    
    if (!setting) {
        Log(Severity::WARNING, System::CORE, "Setting::Get() called for '{}' setting, but it was not found!", name);
        return value_t();
    }
    
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

void Set(const char* name, value_t value) {
    auto setting = lookup_setting(name);
    
    if (!setting) {
        Log(Severity::WARNING, System::CORE, "Setting::Set() called for '{}' setting, but it was not found!", name);
        return;
    }
    
    if (value.GetType() != setting->type) {
        Log(Severity::WARNING, System::CORE, "Trying to set a setting to {}, but it is a {}", TypeToString(value.GetType()), TypeToString(setting->type));
        return;
    }
    
    if (value.IsInt()) {
        *setting->int32 = value.GetInt();
    } else if (value.IsFloat()) {
        *setting->float32 = value.GetFloat();
    } else if (value.IsBool()) {
        *setting->bool32 = value;
    } else {
        Log(Severity::WARNING, System::CORE, "Setting::Set() called for '{}' setting, but set to value {}!", TypeToString(value.GetType()));
    }
    
    if (setting->callback) {
        setting->callback(setting->name);
    }
}


void Parse(const char** argv, int argc) {
    Log(Severity::WARNING, System::CORE, "Parsing settings from CLI");
    
    parsed = true;
    
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
    
    Log(Severity::WARNING, System::CORE, "Applying CLI values to settings");
    
    for (size_t i = 0; i < last_setting; i++) SetFromRaw(settings[i]);
}

uint32_t Flags(const char* name) {
    auto setting = lookup_setting(name);
    
    if (!setting) return 0;
    
    return setting->flags;
}

void SetFlag(const char* name, uint32_t flag, bool enabled) {
    auto setting = lookup_setting(name);
    if (!setting) return;
    
    if (enabled) {
        setting->flags |= flag;
    } else {
        setting->flags &= ~flag;
    }
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

static void save_settings(const char* path, uint32_t filter) {
    File file(path, File::WRITE);
    if (!file.is_open()) {
        Log(Severity::WARNING, System::CORE, "Can't open {} for saving settings!", path);
        return;
    }
    
    file.write_token("SETTINGSv1");
    file.write_newline();
    
    for (size_t i = 0; i < last_setting; i++) {
        const auto& setting = settings[i];
        if (~setting.flags & MODIFIED) continue;
        if (~setting.flags & filter) continue;
        
        file.write_token(setting.name);
        
        switch (setting.type) {
            case TYPE_BOOL:
                file.write_int32(*setting.bool32);
                break;
            case TYPE_FLOAT32:
                file.write_float32(*setting.float32);
                break;
            case TYPE_INT32:
                file.write_int32(*setting.int32);
                break;
            case TYPE_UINT32:
                file.write_uint32(*setting.uint32);
                break;
            default: break;
        }
    }
}

void Save(const char* path) {
    if (path) {
        save_settings(path, USER | APPLICATION);
        return;
    }
    
    save_settings("user://settings.cfg", USER);
    save_settings("app://settings.cfg", APPLICATION);
}

static void load_settings(const char* path) {
    File file(path, File::READ | File::PAUSE_LINE);
    if (!file.is_open()) {
        Log(Severity::WARNING, System::CORE, "Can't open {} for loading settings!", path);
        return;
    }
    
    auto header = file.read_token();
    file.skip_linebreak();
    if (header != "SETTINGSv1") {
        Log(Severity::WARNING, System::CORE, "Invalid settings header {} in file {}", header, path);
        return;
    }
    
    while (file.is_continue()) {
        auto name = file.read_token();
        auto value = file.read_token();
        
        RawSetting setting;
        
        // this will cause a little memory leak, but in the name of expediency
        // we will overlook such a trivial matter
        setting.key = (new std::string(name))->c_str();
        setting.value = (new std::string(value))->c_str();
        setting.maybe_flag = false;
        
        raw_settings.push_back(setting);
        
        file.skip_linebreak();
    }
}

void Load(const char* path) {
    if (path) {
        load_settings(path);
    } else {
        load_settings("app://settings.cfg");
        load_settings("user://settings.cfg");
    }
    
    for (size_t i = 0; i < last_setting; i++) SetFromRaw(settings[i]);
}

static const char* keybind_path(const char* path) {
    return path ? path : "app://keybinds.cfg";
}

void SaveKeybinds(const char* path) {
    File file(keybind_path(path), File::WRITE);
    if (!file.is_open()) {
        Log(Severity::WARNING, System::CORE, "Can't open {} for saving keybinds!", keybind_path(path));
        return;
    }
    
    file.write_token("KEYBINDSv1");
    file.write_newline();

    for (auto& binding : UI::GetAllKeyboardKeyBindings()) {
        for (auto& key : binding.second) {
            file.write_token(UI::GetKeyboardActionName(binding.first));
            file.write_int32(key);
            file.write_string(" # code for ", '\0');
            file.write_string(GetKeyboardKeyName(key), '\0');
        }
    }
}

void LoadKeybinds(const char* path) {
    File file(keybind_path(path), File::READ | File::PAUSE_LINE);
    if (!file.is_open()) {
        Log(Severity::WARNING, System::CORE, "Can't open {} for loading keybinds!", keybind_path(path));
        return;
    }
    
    auto header = file.read_token();
    file.skip_linebreak();
    if (header != "KEYBINDSv1") {
        Log(Severity::WARNING, System::CORE, "Invalid keybinds header {} in file {}", header, keybind_path(path));
        return;
    }
    
    while (file.is_continue()) {
        UI::keyboardaction_t action = UI::GetKeyboardAction(file.read_name());
        UI::KeyboardKey key = (UI::KeyboardKey)file.read_int32();
        
        UI::BindKeyboardKey(key, action);
        
        file.skip_linebreak();
    }
}

}