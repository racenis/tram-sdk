// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_ASYNC_H
#define TRAM_SDK_FRAMEWORK_ASYNC_H

#include <cmath>

namespace tram {

class EntityComponent;
class Resource;

}

namespace tram::Async {

void RequestResource(EntityComponent* requester, Resource* resource);
void RequestResource(void(*callback)(void* data), void* data, Resource* resource);
void CancelRequest(EntityComponent* requester, Resource* resource);
void LoadDependency(Resource* res);
void Init(size_t threads = 1);
void Yeet();
void LoadResourcesFromDisk();
void LoadResourcesFromMemory();
void FinishResources();
size_t GetWaitingResources();
    
}

#endif // TRAM_SDK_FRAMEWORK_ASYNC_H