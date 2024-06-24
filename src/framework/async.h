// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_ASYNC_H
#define TRAM_SDK_FRAMEWORK_ASYNC_H

#include <cmath>

namespace tram {

class EntityComponent;
class Resource;

}

namespace tram::Async {

void RequestResource(EntityComponent* requester, Resource* resource);
void CancelRequest(EntityComponent* requester, Resource* resource);
void LoadDependency(Resource* res);
void Init(size_t threads = 1);
void Yeet();
void ResourceLoader1stStage();
void ResourceLoader2ndStage();
void FinishResource();
size_t GetWaitingResources();
    
}

#endif // TRAM_SDK_FRAMEWORK_ASYNC_H