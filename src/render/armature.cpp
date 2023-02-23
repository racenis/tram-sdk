// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ARMATURE.CPP -- animation code.
// Implementation of animation loading and finding.

#include <render/armature.h>
#include <components/armaturecomponent.h>
#include <framework/logging.h>
#include <templates/hashmap.h>

#include <fstream>

template <> Core::Pool<Core::Render::Animation> Core::PoolProxy<Core::Render::Animation>::pool("animation pool", 50, false);

namespace Core::Render {
    Hashmap<Animation*> ANIMATION_LIST("model name list", 500);
    StackPool<uint8_t> ANIMATION_POOL("animation keyframe pool", 50000);
    Pool<Pose> poseList("pose list", 100, true);
    

    void Animation::LoadFromDisk(){
        using namespace Render;
        std::ifstream file;

        std::string filename = "data/animations/";
        filename += std::string(name);
        filename += ".anim";

        file.open(filename);

        if(!file.is_open()){std::cout << "Can't open " << filename << "!" << std::endl; return;}

        std::cout << "Loading animation: " << filename << std::endl;

        std::string noot;
        //uint64_t ac;
        uint64_t bc;
        uint64_t kc;
        //uint64_t a_name;
        UID a_name;
        NameCount* nameptr;
        Keyframe* kframe;

        file >> noot; // file type
        
        if (noot != "ANIMv1") {
            Log ("Unrecognized header '{}' in animation {}", noot, filename);
            return;
        }
        
        file >> noot;       //name of animation as string
        file >> bc;         //count of bones used by animation

        nameptr = (NameCount*) ANIMATION_POOL.AddNew(sizeof(NameCount));
        a_name = noot;   //uint64 hash of name string

        nameptr->first = a_name;
        nameptr->second = bc;
        //ANIMATION_LIST[a_name] = nameptr;
        this->animation_pointer = nameptr;

        for (uint64_t j = 0; j < bc; j++){
            nameptr = (NameCount*) ANIMATION_POOL.AddNew(sizeof(NameCount));
            file >> noot;       //name of bone as string
            file >> kc;         //count of keyframes for the bone
            a_name = noot;   //uint64 hash of bone name

            nameptr->first = a_name;
            nameptr->second = kc;


            for (uint64_t k = 0; k < kc; k++){
                kframe = (Keyframe*) ANIMATION_POOL.AddNew(sizeof(Keyframe));
                file >> kframe->frame;
                file >> kframe->location.x;
                file >> kframe->location.y;
                file >> kframe->location.z;
                file >> kframe->rotation.x;
                file >> kframe->rotation.y;
                file >> kframe->rotation.z;
                file >> kframe->rotation.w;
                file >> kframe->scale.x;
                file >> kframe->scale.y;
                file >> kframe->scale.z;
            }
        }
    

        file.close();
    }

    Animation* Animation::Find (name_t name) {
        Animation* animation = ANIMATION_LIST.Find(name);
        
        if (!animation) {
            animation = PoolProxy<Animation>::New(name);
            ANIMATION_LIST.Insert(UID(name), animation);
        }
        
        return animation;
    }
}

