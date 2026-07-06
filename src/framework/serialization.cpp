// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/serialization.h>
#include <framework/logging.h>
#include <framework/file.h>

#include <framework/language.h>
#include <framework/path.h>
#include <framework/navmesh.h>
#include <framework/worldcell.h>
#include <framework/script.h>
#include <audio/sound.h>
#include <physics/collisionmodel.h>
#include <render/animation.h>
#include <render/environment.h>
#include <render/lightmap.h>
#include <render/material.h>
#include <render/model.h>
#include <render/particle.h>
#include <render/sprite.h>

#include <platform/file.h>

#include <vector>

namespace tram::Serialization {

struct DataType {
    std::string name;
    void (*load)(const char*);
    void (*save)(const char*);
    void (*reload)(const char*);
};

enum {
    ITEM_INITIAL = 1,
    ITEM_DELAYED = 2,
    ITEM_SAVEABLE = 4
};

struct ManifestItem {
    std::string name;
    std::string type;
    uint32_t flags;
};

enum PluginType {
    PLUGIN_DIRECTORY,
    PLUGIN_ARCHIVE,
    PLUGIN_BASE
};

struct Plugin {
    std::string name;
    PluginType type;
    std::vector<ManifestItem> items;
};

std::vector<Plugin> plugins;
std::vector<DataType> data_types = {
    {"language",        [](const char* name) { Language::Load(name); },
                        [](const char* name) {}, [](const char* name) {}},
    {"path",            [](const char* name) { Path::Find(name)->LoadFromDisk(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"navmesh",         [](const char* name) { Navmesh::Find(name)->LoadFromDisk(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"worldcell",       [](const char* name) { WorldCell::Find(name)->LoadFromDisk(); },
                        [](const char* name) { WorldCell::Find(name)->SaveToDisk(); },
                        [](const char* name) { auto cell = WorldCell::Find(name); cell->Reset(); cell->LoadFromDisk(); }},
    {"sound",           [](const char* name) { Audio::Sound::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"collisionmodel",  [](const char* name) { Physics::CollisionModel::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"animation",       [](const char* name) { Render::Animation::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"environment",     [](const char* name) { Render::Environment::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"lightmap",        [](const char* name) { Render::Lightmap::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"material",        [](const char* name) { Render::Material::LoadMaterialInfo(name); },
                        [](const char* name) {}, [](const char* name) {}},
    {"model",           [](const char* name) { Render::Model::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"particle",        [](const char* name) { Render::Particle::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"sprite",          [](const char* name) { Render::Sprite::Find(name)->Load(); },
                        [](const char* name) {}, [](const char* name) {}},
    {"script",          [](const char* name) { Script::LoadScript(name); },
                        [](const char* name) {}, [](const char* name) {}},
};

void Register(const char* name, void (*load)(const char*), void (*save)(const char*), void (*reload)(const char*)) {
    data_types.push_back({name, load, save, reload});
}

static std::string plugin_protocol(const Plugin& plugin) {
    switch (plugin.type) {
        case PLUGIN_DIRECTORY:  return "file";
        case PLUGIN_ARCHIVE:    return "archive";
        default:                return "file";
    }
}

static std::string plugin_location(const Plugin& plugin) {
    switch (plugin.type) {
        case PLUGIN_DIRECTORY:  return std::string("plugins/") + plugin.name + ".pkg";
        case PLUGIN_ARCHIVE:    return std::string("plugins/") + plugin.name;
        default:                return "";
    }
}

static std::vector<FileSource> plugin_sources_yeet() {
    std::vector<FileSource> sources = FileReader::GetSearchList();
    std::vector<FileSource> backup = sources;
    for (auto& plugin : plugins) {
        std::string location = plugin_protocol(plugin);
        std::string protocol = plugin_location(plugin);
        
        std::erase_if(sources, [&](const FileSource& source){ return source.location == location && source.protocol == protocol; });
    }
    return backup;
}

void AppendPluginsToLoadList() {
    std::vector<FileSource> sources = FileReader::GetSearchList();
    
    std::vector<std::string> protocols;
    std::vector<std::string> locations;
    for (auto& plugin : plugins) {
        protocols.push_back(plugin_protocol(plugin));
        locations.push_back(plugin_location(plugin));
        
        sources.push_back({locations.back().c_str(), protocols.back().c_str()});
    }
    FileReader::SetSearchList(sources);
}
void RemovePluginsFromLoadList() {
    plugin_sources_yeet();
}

void LoadDataManifest() {
    std::vector<FileSource> backup = FileReader::GetSearchList();
    for (auto& plugin : plugins) {
        std::string location = plugin_protocol(plugin);
        std::string protocol = plugin_location(plugin);
        
        FileReader::SetSearchList({{location.c_str(), protocol.c_str()}});
        
        File file("resource.manifest", File::READ | File::PAUSE_LINE);
        
        if (!file.is_open()) {
            Log(Severity::ERROR, System::CORE, "Plugin {} missing manifest!", plugin.name);
            continue;
        }
        
        auto header = file.read_token();
        file.skip_linebreak();
        if (header != "MANIFESTv1") {
            Log(Severity::ERROR, System::CORE, "Data manifest has incompatible type {}!", header);
            continue;
        }
        
        while (file.is_continue()) {
            auto record = file.read_token();
            auto type = file.read_token();
            auto name = file.read_token();
            
            uint32_t flags = 0;
            if (record == "initial") {
                flags = ITEM_INITIAL;
            } else if (record == "delayed") {
                flags = ITEM_DELAYED;
            } else if (record == "saveable") {
                flags = ITEM_SAVEABLE;
            } else {
                Log(Severity::WARNING, System::CORE, "Data manifest invalid record type: {} on line {}", record, file.get_line());
            }
            
            bool inserted = false;
            for (auto& item : plugin.items) {
                if (item.name != name || item.type != type) continue;
                item.flags |= flags;
                inserted = true;
                break;
            }
            
            if (!inserted) {
                plugin.items.push_back({.name = (std::string)name, .type = (std::string)type, .flags = flags});
            }
            
            file.skip_linebreak();
        }
    }
    
    FileReader::SetSearchList(backup);
}

void LoadPluginManifest() {
    File file("plugin.manifest", File::READ | File::PAUSE_LINE);
    
    if (!file.is_open()) {
        Log(Severity::ERROR, System::CORE, "Couldn't open plugin manifest!");
        return;
    }
    
    auto header = file.read_token();
    file.skip_linebreak();
    if (header != "PLUGINSv1") {
        Log(Severity::ERROR, System::CORE, "Plugin manifest has incompatible type {}!", header);
        return;
    }
    
    plugins.clear();
    plugins.push_back({.name = "base", .type = PLUGIN_BASE});
    
    bool enabled = true;
    while (file.is_continue()) {
        auto record = file.read_token();
        
        if (record == "enabled") {
            enabled = true;
        } else if (record == "disabled") {
            enabled = false;
        } else if (record == "plugin") {
            auto type = file.read_token();
            auto name = file.read_token();
            
            if (type == "pkg") {
                if (enabled) plugins.push_back({.name = (std::string)name, .type = PLUGIN_ARCHIVE});
            } else if (type == "dir") {
                if (enabled) plugins.push_back({.name = (std::string)name, .type = PLUGIN_DIRECTORY});
            } else {
                Log(Severity::WARNING, System::CORE, "Plugin manifest invalid plugin type: {} on line {}", type, file.get_line());
            }
        } else {
            Log(Severity::WARNING, System::CORE, "Plugin manifest invalid record: {} on line {}", record, file.get_line());
        }
        
        file.skip_linebreak();
    }
}

static void item_action(uint32_t flag, void(*action)(DataType&, ManifestItem&)) {
    std::vector<FileSource> backup = plugin_sources_yeet();
    for (auto& plugin : plugins) {
        std::string location = plugin_protocol(plugin);
        std::string protocol = plugin_location(plugin);
        
        std::vector<FileSource> sources = FileReader::GetSearchList();
        sources.push_back({location.c_str(), protocol.c_str()});
        FileReader::SetSearchList(sources);
        
        for (auto& item : plugin.items) {
            if (~item.flags & flag) continue;
            for (auto& type : data_types) {
                if (type.name != item.type) continue;
                action(type, item);
                break;
            }
        }
    }
    FileReader::SetSearchList(backup);
}

void LoadInitial() {
    item_action(ITEM_INITIAL, [](DataType& type, ManifestItem& item) {
        type.load(item.name.c_str());
    });
}

void LoadDelayed() {
    item_action(ITEM_DELAYED, [](DataType& type, ManifestItem& item) {
        type.load(item.name.c_str());
    });
}

void Save() {
    item_action(ITEM_SAVEABLE, [](DataType& type, ManifestItem& item) {
        type.save(item.name.c_str());
    });
}

void Reload() {
    item_action(ITEM_SAVEABLE, [](DataType& type, ManifestItem& item) {
        type.reload(item.name.c_str());
    });
}

void LoadFlat() {
    for (auto& plugin : plugins) {
        for (auto& item : plugin.items) {
            if (~item.flags & ITEM_SAVEABLE) continue;
            for (auto& type : data_types) {
                if (type.name != item.type) continue;
                type.load(item.name.c_str());
                break;
            }
        }
    }
}

}