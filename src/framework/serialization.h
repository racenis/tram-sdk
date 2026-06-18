// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_SERIALIZATION_H
#define TRAM_SDK_FRAMEWORK_SERIALIZATION_H

namespace tram::Serialization {

void Register(const char*, void (*)(const char*), void (*)(const char*), void (*)(const char*));

void AppendPluginsToLoadList();
void RemovePluginsFromLoadList();

void LoadDataManifest();
void LoadPluginManifest();
void LoadInitial();
void LoadDelayed();
void Save();
void Reload();

}

#endif // TRAM_SDK_FRAMEWORK_SERIALIZATION_H
