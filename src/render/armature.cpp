// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ARMATURE.CPP -- animation code.
// Implementation of animation loading and finding.

#include <render/armature.h>
#include <components/armaturecomponent.h>

#include <fstream>

namespace Core::Render {
    std::unordered_map<name_t, NameCount*> Animation::animationlist;
    Pool<Pose> poseList("pose list", 100, true);
    template <> Pool<Animation> PoolProxy<Animation>::pool("animation pool", 50, false);

    void Animation::LoadFromDisk(){
        using namespace Render;
        std::ifstream file;

        std::string filename = "data/animations/";
        filename += ReverseUID(name);
        filename += ".anim";

        file.open(filename);

        if(!file.is_open()){std::cout << "Can't open " << filename << "!" << std::endl; return;}

        std::cout << "Loading animation: " << filename << std::endl;

        std::string noot;
        uint64_t ac;
        uint64_t bc;
        uint64_t kc;
        uint64_t a_name;
        NameCount* nameptr = (NameCount*)animationpool.begin();
        Keyframe* kframe;


        file >> noot;       //not actually used for anything
        file >> ac;         //count of animation entries in the file

        for (uint64_t i = 0; i < ac; i++){
            file >> noot;       //name of animation as string
            file >> bc;         //count of bones used by animation
            a_name = UID(noot);   //uint64 hash of name string

            nameptr->first = a_name;
            nameptr->second = bc;
            animationlist[a_name] = nameptr;

            nameptr++;
            for (uint64_t j = 0; j < bc; j++){
                file >> noot;       //name of bone as string
                file >> kc;         //count of keyframes for the bone
                a_name = UID(noot);   //uint64 hash of bone name

                nameptr->first = a_name;
                nameptr->second = kc;
                nameptr++;


                for (uint64_t k = 0; k < kc; k++){
                    kframe = (Keyframe*)nameptr;
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
                    kframe++;
                    nameptr = (NameCount*)kframe;
                }
            }
        }

        file.close();
    }

    NameCount* Animation::Find(name_t name){
        std::unordered_map<uint64_t, NameCount*>::iterator ff = animationlist.find(name);

        if(ff == animationlist.end()){
            std::cout << "Can't find animation " << ReverseUID(name) << "!" << std::endl;
            return nullptr;
        } else {
            return ff->second;
        }
    }    
    
    void UpdateArmatures(){
        ArmatureComponent* armobj = PoolProxy<ArmatureComponent>::GetPool().GetFirst();
        ArmatureComponent* armlast = PoolProxy<ArmatureComponent>::GetPool().GetLast();
        for(;armobj < armlast; armobj++){
            if(*((uint64_t*)armobj) == 0)
                continue;
            else
                armobj->Update();
        }
    }
    
    void Animation::LoadAll() {
        for (auto& anim : PoolProxy<Animation>::GetPool()) anim.LoadFromDisk();
    }
}

