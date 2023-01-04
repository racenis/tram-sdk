// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

namespace Core {
    class EntityComponent;
    class Resource;
}

namespace Core::Async {
    void RequestResource (EntityComponent* requester, Resource* requested_resource);
    void ForceLoadResource(Resource* res);
    void Init();
    void Yeet();
    void ResourceLoader2ndStage();
    void FinishResource();
}
