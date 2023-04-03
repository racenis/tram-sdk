// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/armature.h>
#include <components/armaturecomponent.h>
#include <framework/logging.h>
#include <framework/file.h>
#include <templates/hashmap.h>

#include <cstring>

using namespace tram;

template <> Pool<Render::Animation> PoolProxy<Render::Animation>::pool ("animation pool", 50, false);
template <> Pool<Render::Pose> PoolProxy<Render::Pose>::pool ("pose pool", 100, false);

namespace tram::Render {
    
Hashmap<Animation*> animation_list ("model name list", 500);
StackPool<uint8_t> animation_pool ("animation keyframe pool", 50000);

Pose* BLANK_POSE = nullptr;

void Animation::LoadFromDisk(){
    char filename [100] = "data/animations/";
    strcat (filename, name);
    strcat (filename, ".anim");

    File file (filename, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't open " << filename << "!" << std::endl; 
        return;
    }

    std::cout << "Loading animation: " << filename << std::endl;


    NameCount* nameptr;

    if (name_t header = file.read_name(); header != "ANIMv1") {
        Log ("Unrecognized header '{}' in animation {}", header, filename);
        return;
    }
    
    nameptr = (NameCount*) animation_pool.AddNew(sizeof(NameCount));

    nameptr->first = file.read_name();
    nameptr->second = file.read_uint32();
    
    this->animation_pointer = nameptr;

    for (uint64_t j = 0; j < nameptr->second; j++){
        nameptr = (NameCount*) animation_pool.AddNew(sizeof(NameCount));

        nameptr->first = file.read_name();
        nameptr->second = file.read_uint32();

        for (uint64_t k = 0; k < nameptr->second; k++){
            Keyframe* kframe = (Keyframe*) animation_pool.AddNew(sizeof(Keyframe));
            
            kframe->frame = file.read_float32();
            kframe->location = {file.read_float32(), file.read_float32(), file.read_float32()};
            kframe->rotation.x = file.read_float32();
            kframe->rotation.y = file.read_float32();
            kframe->rotation.z = file.read_float32();
            kframe->rotation.w = file.read_float32();
            kframe->scale = {file.read_float32(), file.read_float32(), file.read_float32()};
        }
    }
}

Animation* Animation::Find (name_t name) {
    Animation* animation = animation_list.Find(name);
    
    if (!animation) {
        animation = PoolProxy<Animation>::New(name);
        animation_list.Insert(UID(name), animation);
    }
    
    return animation;
}
    
}

