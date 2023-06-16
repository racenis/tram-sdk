// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/armature.h>
#include <components/armature.h>
#include <framework/logging.h>
#include <framework/file.h>
#include <templates/hashmap.h>

#include <cstring>

using namespace tram;

template <> Pool<Render::Animation> PoolProxy<Render::Animation>::pool ("animation pool", 50, false);
template <> Pool<Render::Pose> PoolProxy<Render::Pose>::pool ("pose pool", 100, false);

namespace tram::Render {
    
Hashmap<Animation*> animation_list ("model name list", 500);
StackPool<uint8_t> animation_pool ("animation keyframe pool", 1024 * 1024);

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


    if (name_t header = file.read_name(); header != "ANIMv1") {
        Log ("Unrecognized header '{}' in animation {}", header, filename);
        return;
    }
    
    NameCount* anim_header = (NameCount*) animation_pool.AddNew(sizeof(NameCount));

    anim_header->first = file.read_name();
    anim_header->second = file.read_uint32();
    
    this->animation_pointer = anim_header;

    for (uint64_t j = 0; j < anim_header->second; j++){
        NameCount* bone_header = (NameCount*) animation_pool.AddNew(sizeof(NameCount));

        bone_header->first = file.read_name();
        bone_header->second = file.read_uint32();

        for (uint64_t k = 0; k < bone_header->second; k++){
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

