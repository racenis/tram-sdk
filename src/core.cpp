// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CORE.CPP -- Core functions and globals.

#include <core.h>
#include <render.h>
#include <armature.h>

#include <cstring>
#include <fstream>

#include <glfw3.h>



namespace Core {
    
    bool SHOULD_CLOSE = false;
    float FRAME_TIME = 0;

    PoolAddOnly<char> stringPool("stringpool", 10000);
    std::unordered_map<std::string, uint64_t> stringHashMap;
    std::unordered_map<uint64_t, uint64_t> langStringHashMap;

    Queue<Event> Core::Event::queue("event queue", 500);
    Queue<Message> Core::Message::queue("message queue", 500);

    Pool<PathNode> pathNodePool("path node pool", 200, false);


    Pool<Event::Listener> Core::Event::listeners[10] = {
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
        Pool<Event::Listener>("event listener: ", 50, false),
    };

       
    Event::Listener* Event::AddListener(Event::Type type){
        return listeners[type].AddNew();
    }

    void Event::RemoveListener(Event::Type type, Event::Listener* listener){
        // you could probably deduct the type of listener from its pointer
        listeners[type].Remove(listener);
    }

    void Event::Dispatch(){
        Event* event = Event::queue.GetFirstPtr();
        while (event){
            Event::Type type = event->type;

            Event::Listener* ls = listeners[type].GetFirst();
            Event::Listener* nd = listeners[type].GetLast();

            // TODO: add the thing that check the distances
            for (; ls < nd; ls++){
                if(ls->ent != nullptr){
                    ls->ent->EventHandler(*event);
                }
            }

            Event::queue.Remove();
            event = Event::queue.GetFirstPtr();
        }
    }

    void Message::Dispatch(){
        Message* message = Message::queue.GetFirstPtr();
        while(message){
            Entity* receiver = Entity::Find(message->receiverID);
            if (receiver) receiver->MessageHandler(*message);

            Message::queue.Remove();
            message = Message::queue.GetFirstPtr();
        }
    }

    uint64_t GenerateID(){
        static uint64_t num = 0;
        num++;
        return num;
    }

    name_t FindLangStr(name_t name){
        std::unordered_map<uint64_t, uint64_t>::iterator ff = langStringHashMap.find(name);

        if(ff == langStringHashMap.end()){
            return 0;
        } else {
            return ff->second;
        }
    }

    const char* ReverseUID(name_t uid){
        return stringPool.begin() + uid;
    }

    namespace Stats {
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
            start_time[stat] = glfwGetTime();
        }

        void Stop(Stats::Type stat){
            total_time[stat] += glfwGetTime() - start_time[stat];
        }

        void Reset(){
            for (size_t i = 0; i < sizeof(Stats::Type); i++){
                final_time[i] = ((float) total_time[i]) * 1000.0f;
                final_time_cum[i] += final_time[i];
                total_time[i] = 0.0;
            }

            frame_passed++;

            uint64_t this_frame = (uint64_t)glfwGetTime();
            if(this_frame != full_frame){
                full_frame = this_frame;

                for (size_t i = 0; i < sizeof(Stats::Type); i++){
                    final_time_avg[i] = final_time_cum[i] / frame_passed;
                    final_time_cum [i] = 0.0f;
                }

                frame_passed = 0;
            }

        }
    }

    uint64_t UID(std::string name){
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
    }




    void Event::Post (Event &event){
        *(Event::queue.AddNew()) = event;
    }

    void Message::Send(Message &message){
        *(Message::queue.AddNew()) = message;
    }

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

    void LoadText(const char* filename){
        std::ifstream file;
        file.open(filename);
        while(file){
            std::string strid;
            std::string langstr;

            file >> strid;
            file.ignore(1);
            std::getline(file, langstr);

            uint64_t strkey = UID(strid);
            uint64_t langkey = stringPool.GetSize();

            char* tbr = stringPool.AddNew(langstr.length() + 1);

            strcpy(tbr, langstr.c_str());

            langStringHashMap[strkey] = langkey;

        }
        file.close();
    }

    void LoadPath(const char* filename){
        std::ifstream file;
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

        file.close();
    }

    void Init(){
        // set the 0th string to 'none'
        std::string none = "none";
        UID(none);
    }




}

