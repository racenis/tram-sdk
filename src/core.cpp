// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CORE.CPP -- Core functions and globals.

#include <core.h>
#include <render/render.h>
#include <render/armature.h>

#include <ui.h>

#include <cstring>
#include <fstream>

namespace Core {
    char const* ENGINE_VERSION = "Tramway SDK v0.0.3-alpha";
    
    bool SHOULD_CLOSE = false;
    float FRAME_TIME = 0;


    // this one should actually go into the ?? uhh ??
    // the extension. right.
    bool DRAW_PHYSICS_DEBUG = false;
    bool DRAW_RENDER_DEBUG = false;
    bool DRAW_PATH_DEBUG = false;
    
    size_t RESOURCE_VRAM_USAGE = 0;
    

    

    



    
    

    // Some of this code here should be salvageable

    //const char* ReverseUID(name_t uid){
    //    return stringPool.begin() + uid;
    //}

    /*namespace Stats {
        double total_time[sizeof(Stats::Type)] = {0.0};
        double start_time[sizeof(Stats::Type)] = {0.0};
        float final_time[sizeof(Stats::Type)] = {0.0f};
        float final_time_cum[sizeof(Stats::Type)] = {0.0f};
        float final_time_avg[sizeof(Stats::Type)] = {0.0f};

        uint64_t full_frame = 0;
        size_t frame_passed = 0;

        uint64_t renderobjects = 0;
        uint64_t animationcomponents = 0;
        uint64_t physicscomponents = 0;
        
        void Start(Stats::Type stat){
            #ifndef ENGINE_EDITOR_MODE
            start_time[stat] = UI::GetTime();
            #endif // ENGINE_EDITOR_MODE
        }

        void Stop(Stats::Type stat){
            #ifndef ENGINE_EDITOR_MODE
            total_time[stat] += UI::GetTime() - start_time[stat];
            #endif // ENGINE_EDITOR_MODE
        }

        void Reset(){
            #ifndef ENGINE_EDITOR_MODE
            for (size_t i = 0; i < sizeof(Stats::Type); i++){
                final_time[i] = ((float) total_time[i]) * 1000.0f;
                final_time_cum[i] += final_time[i];
                total_time[i] = 0.0;
            }

            frame_passed++;

            uint64_t this_frame = (uint64_t)UI::GetTime();
            if(this_frame != full_frame){
                full_frame = this_frame;

                for (size_t i = 0; i < sizeof(Stats::Type); i++){
                    final_time_avg[i] = final_time_cum[i] / frame_passed;
                    final_time_cum [i] = 0.0f;
                }

                frame_passed = 0;
            }
            #endif // ENGINE_EDITOR_MODE
        }
    }*/

    /*uint64_t UID(std::string name){
        std::unordered_map<std::string, uint64_t>::iterator ff = stringHashMap.find(name);
        if(ff == stringHashMap.end()){
            uint64_t key = stringPool.GetSize();
            char* newstr = stringPool.AddNew(name.size() + 1);

            stringHashMap.emplace(name, key);
            strcpy(newstr, name.c_str());
            return key;
        } else {
            return ff->second;
        }
        
    }

    uint64_t UID(std::string_view name, size_t length){
        std::string name_str(name);
        std::unordered_map<std::string, uint64_t>::iterator ff = stringHashMap.find(name_str);
        if(ff == stringHashMap.end()){
            uint64_t key = stringPool.GetSize();
            char* newstr = stringPool.AddNew(name.size() + 1);

            stringHashMap.emplace(name_str, key);
            strcpy(newstr, name_str.c_str());
            return key;
        } else {
            return ff->second;
        }
    }*/

    

    //const char* ReverseUID(name_t uid){
    //    return stringPool.begin() + uid;
    //}







    void QuatLookAt(glm::quat& quaternion, const glm::vec3& from, const glm::vec3& to){
        const glm::vec3 start = normalize(from);
        const glm::vec3 desu = normalize(to);

        const glm::vec3 axis = glm::cross(start, desu);

        const float square = sqrt((1.0f + glm::dot(start, desu)) * 2.0f);
        const float inverse = 1 / square;

        quaternion.x = axis.x * inverse;
        quaternion.y = axis.y * inverse;
        quaternion.z = axis.z * inverse;
        quaternion.w = square * 0.5f;
    }

    void ProjectLine(glm::vec3& point, const glm::vec3& from, const glm::vec3& to){
        const glm::vec3 projectable = point - from;
        const glm::vec3 line = to - from;
        point = from + (glm::dot(projectable, line) / glm::dot(line, line) * line);
    }

    

    void LoadPath(const char* filename){
        abort();
/*        std::ifstream file;
        size_t path_verts;
        size_t path_edges;
        file.open(filename);
        if(file){
            file >> path_verts;
            file >> path_edges;

            for (size_t i = 0; i < path_verts; i++){
                PathNode* pp = pathNodePool.AddNew();
                file >> pp->coords[0];
                file >> pp->coords[1];
                file >> pp->coords[2];
                file >> pp->type;
                pp->id = i;
            }

            for (size_t i = 0; i < path_edges; i++){
                PathNode* f_node;
                PathNode* s_node;
                size_t f_index;
                size_t s_index;

                file >> f_index;
                file >> s_index;

                f_node = pathNodePool.begin() + f_index;
                s_node = pathNodePool.begin() + s_index;

                for (size_t k = 0; k < 3; k++){
                    if (f_node->next[k] != nullptr) continue;
                    f_node->next[k] = s_node;
                    break;
                }
            }
        }

        file.close();*/
    }

    void Init(){
        // set the 0th string to 'none'
        UID none("none");
        
        
        // these are all of the default systems, as of now.
        // feel free to extend the list, but do note that this list
        // is in the same order as the System::System enumeration.
        System::Register("Core", "CORE");
        System::Register("User Interface", "UI");
        System::Register("Async", "ASYNC");
        System::Register("Rendering", "RENDER");
        System::Register("Physics", "PHYSICS");
        System::Register("Audio", "AUDIO");
        System::Register("Misc", "MISC");
        
        
    }




}


