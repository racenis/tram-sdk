// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CORE.H -- Engine core definitons
// Basic engine datatypes, templates, useful functions.

#ifndef TRAM_CORE_H
#define TRAM_CORE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <templates.h>

namespace Core {
    typedef uint64_t name_t;

    extern bool SHOULD_CLOSE;
    extern float FRAME_TIME;
    
    extern bool DRAW_PHYSICS_DEBUG;
    extern bool DRAW_RENDER_DEBUG;
    extern bool DRAW_PATH_DEBUG;
    
    extern char const* ENGINE_VERSION;
    
    extern size_t RESOURCE_VRAM_USAGE;

    const glm::vec3 DIRECTION_FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 DIRECTION_SIDE = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 DIRECTION_UP = glm::vec3(0.0f, 1.0f, 0.0f);

    // forward declarations

    // TODO: check which ones can be yeeted
    class WorldCell;
    class EntityComponent;
    class SerializedEntityData;

    uint64_t UID(std::string name);
    uint64_t UID(std::string_view name, size_t length);
    name_t FindLangStr(name_t name);
    const char* ReverseUID(uint64_t uid);
    
    void LoadText(const char* filename);

    void Init();

    void QuatLookAt(glm::quat& quaternion, const glm::vec3& from, const glm::vec3& to);
    void ProjectLine(glm::vec3& point, const glm::vec3& from, const glm::vec3& to);

    uint64_t GenerateID();

    struct Path;
    struct Navmesh;

    typedef uint16_t event_t;
    typedef uint64_t message_t;

    struct Event {
        enum Type : uint16_t {
            KEYPRESS,
            KEYDOWN,
            KEYUP,
            CURSORPOS,
            LAST_EVENT
        };

        struct Listener {
            EntityComponent* ent = nullptr; // TODO: rename this
            uint64_t poster = 0;
            glm::vec3 location;
            float dist = 0.0f;
            event_t type = 0;
        };

        static void Post (Event &event);
        static void Dispatch();
        static Event::Listener* AddListener(Event::Type type);
        static void RemoveListener(Event::Listener* listener);

        event_t type;
        event_t subtype;
        glm::vec3 location;
        uint64_t posterID;
        void* data;

        static Pool<Event::Listener> listeners;
        static std::vector<std::vector<Event::Listener*>> dispatch_table;
        static Queue<Event> queue;
    };

    struct Message {
        enum Type : uint64_t {
            PING,
            HELLO,
            ACTIVATE,
            GET_IN,
            GET_OUT,
            LAST_MESSAGE
        };

        static void Send (Message &message);
        static void Dispatch();

        message_t type;
        uint64_t receiverID;
        uint64_t senderID;
        void* data = nullptr;

        static Queue<Message> queue;
    };

    class Entity {
    public:
        virtual void Load() = 0;

        virtual void Unload() = 0;

        virtual void Serialize() = 0;

        void SerializeString(std::string& str);

        Entity() = default;

        inline void Init(){
            if(!id){
                id = GenerateID();
                Register();
            }
        }
        
        void Yeet();

        inline name_t GetName() const { return name; }
        inline uint64_t GetID() const { return id; }
        inline WorldCell* GetCell() { return cell; }
        inline bool IsLoaded() const { return isloaded; }
        inline bool IsAutoLoad() const { return auto_load; }
        inline bool IsInInterior() { return in_interior; }
        inline bool IsPersistent() { return  persistent; }
        inline bool IsChanged() { return changed; }

        void SetParameters (std::string_view& params);

        void ParametersString(std::string& str);

        void virtual UpdateParameters() = 0;
        
        void virtual SetParameters() = 0;
        
        inline void SetPersistent(bool persistent) { this->persistent = persistent; }

        void SetLocation(float x, float y, float z){ location = glm::vec3(x, y, z); SetParameters(); CheckTransition(); }
        void SetLocation(const glm::vec3& loc) { location = loc; SetParameters(); CheckTransition();}

        void SetRotation(float x, float y, float z){ rotation = glm::quat(glm::vec3(x, y, z)); SetParameters(); }
        void SetRotation(glm::quat& rot){ rotation = rot; SetParameters(); }

        inline void UpdateTransform(const glm::vec3& loc, const glm::quat& rot){
            location = loc;
            rotation = rot;
            UpdateParameters();
            CheckTransition();
        }

        inline void Move(const glm::vec3& move_by){
            SetLocation(location + move_by);
        }

        inline void GetLocation(glm::vec3& loc){ loc = location; }
        inline const glm::vec3& GetLocation(){ return location; }

        inline void GetRotation(glm::quat& rot){ rot = rotation; }
        inline const glm::quat& GetRotation(){ return rotation; }

        virtual void MessageHandler(Message& msg) = 0;

        void CheckTransition();

        void RemoveSerializedData();

        void Activate (Message& msg){
            std::cout << ReverseUID(name) << " got activated" << std::endl;
            return;
        }

        static Entity* Make (std::string_view& params);

        static void Register (const char* name, Entity* (*constr_func)(std::string_view& params));

        void Register();

        void Deregister();

        static Entity* Find (uint64_t entityID);

        static Entity* FindName(name_t entityName);
    protected:
        uint64_t id = 0;
        name_t name = 0;
        name_t action = 0;

        bool isloaded = false;
        bool auto_load = true;
        bool in_interior = false;
        bool persistent = true;
        bool changed = false;
        
        WorldCell* cell = nullptr;
        
        glm::quat rotation = {0.0f, 0.0f, 0.0f, 1.0f};
        glm::vec3 location = {0.0f, 0.0f, 0.0f};

        SerializedEntityData* serialized_data = nullptr; // TODO: yeet this

        static std::unordered_map<uint64_t, Entity*> List;
        static std::unordered_map<name_t, Entity*> NameList;
        static std::unordered_map<std::string, Entity* (*)(std::string_view& params)> entityConstructors;
        friend class WorldCell;
    };

    /// Base class for EntityComponents.
    class EntityComponent {
    public:
        EntityComponent(){ id = GenerateID(); }
        virtual ~EntityComponent() {}

        virtual void Init() {
            is_init = true;
            if (resources_waiting == 0) Start();
        }

        // I think that we should remove the Uninit method
        // and replace it with desturcotrs
        virtual void Uninit() = 0;

        virtual void EventHandler(Event &event) = 0;

        inline bool IsReady(){ return is_ready; }
        inline bool IsInit(){ return is_init; }
        
        inline bool GetParent(){ return parent; }
        inline void SetParent(Entity* parent){ this->parent = parent; }

        inline uint64_t GetID(){ return id; }

        // probably a good idea to make this a protected method
        // and add async function a friend function
        void ResourceReady(){ resources_waiting--; if (resources_waiting == 0 && is_init) Start();}
    protected:
        uint64_t id;
        size_t resources_waiting = 0;
        bool is_ready = false;
        bool is_init = false;
        Entity* parent = nullptr;

        virtual void Start() = 0;
        template <typename T> friend class ResourceProxy;
    };
    
    /// Wrapper for an EntityComponent pointer.
    template <typename T>
    class Component {
    public:
        Component() {}
        ~Component() { if (ptr) PoolProxy<T>::Delete(ptr); }
        void make() { ptr = PoolProxy<T>::New(); }
        void clear() { PoolProxy<T>::Delete(ptr); ptr = nullptr; }
        T* get() { return ptr; }
        T* operator->() { return ptr; }
        T& operator*() { return ptr; }
        explicit operator bool() { return ptr != nullptr; }
    protected:
        T* ptr = nullptr;
    };
    
    
    class WorldCell {
    public:
        class Transition {
        public:
            void AddPoint(const glm::vec3& point);
            void GeneratePlanes(bool disp = false);
            bool IsInside(const glm::vec3& point);
            void SetInto(WorldCell* new_into) { into = new_into; }
            WorldCell* GetInto() { return into; }
        protected:
            name_t name = 0;
            WorldCell* into;
            std::vector<glm::vec3> points;
            std::vector<glm::vec4> planes;
            friend class WorldCell;
        };
        class Loader {
        public:
            void SetLocation(const glm::vec3& new_location) { location = new_location; current_cell = Find(new_location); }
            void UpdateLocation(const glm::vec3& new_location) { location = new_location; auto n_trans = current_cell->FindTransition(location); if (n_trans) { current_cell = n_trans; } }
            static void LoadCells();
        private:
            glm::vec3 location;
            WorldCell* current_cell;
        };
    protected:
        name_t name = 0;
        bool interior = false;
        bool interiorLights = false;
        bool loaded = false;
        bool draw = false;
        std::vector<Entity*> entities;
        std::vector<Transition*> trans_in;  // rename
        std::vector<Transition*> trans_out; // this one too?
        std::vector<Path*> paths;
        std::vector<Navmesh*> navmeshes;
        static std::unordered_map<uint64_t, WorldCell*> List;
        friend void LoadCells();
    public:
        WorldCell(){};

        bool IsLoaded() {return loaded;};

        bool IsInterior() {return interior;};
        bool HasInteriorLighting() {return interiorLights;};
        bool IsDrawn() {return draw;};
        
        void SetInterior(bool interior) { this->interior = interior; }
        void SetInteriorLights(bool interior_lights) { this->interiorLights = interior_lights; }
        
        name_t GetName(){return name;};   

        void SetName(name_t new_name);
        
        // what is this and why do we need this?
        void SetDrawn(bool drawn) { draw = drawn; }

        void Load();

        void Unload();

        void LoadFromDisk();
        
        void DrawTransitions();
        void DrawNavmeshes();
        void DrawPaths();

        void AddEntity(Entity* entPtr);

        void RemoveEntity(Entity* entPtr){
            entities.erase(std::find(entities.begin(), entities.end(), entPtr));
        };
        
        size_t EntityCount() { return entities.size(); }

        // Adds a transition *into* the cell.
        void AddTransition(Transition* transPtr){
            trans_in.push_back(transPtr);
            transPtr->SetInto(this);
        };

        // Adds a transition *from* the cell.
        void AddTransitionFrom(Transition* transPtr){
            trans_out.push_back(transPtr);
        };

        void AddLink(WorldCell* cell){
            trans_out.insert(trans_out.end(), cell->trans_in.begin(), cell->trans_in.end());
        };

        // Find if a point is in a transition *out of* the cell.
        inline WorldCell* FindTransition(glm::vec3& point){
            for(size_t i = 0; i < trans_out.size(); i++){
                if(trans_out[i]->IsInside(point)) return trans_out[i]->GetInto();
            }
            return nullptr;
        }

        // Checks if a point is inside the cell.
        bool IsInside(const glm::vec3& point){
            for(size_t i = 0; i < trans_in.size(); i++){
                if(trans_in[i]->IsInside(point)) return true;
            }
            return false;
        }

        static WorldCell* Find (const glm::vec3& point);
        static WorldCell* Find (name_t name);
    };



    class Resource  {
    public:
        enum Status {
            UNLOADED,          /// Resource not in memory. Can't be used.
            WAITING_FOR_LOAD,  /// Resource not in memory, but has been placed in the load queue.
            LOAD_FAIL,         // not used??
            LOADED,            // also not used???
            WAITING_FOR_READY, /// Resource loaded in memory, but still needs some processing (like uploading to video memory).
            READY,             /// Resource is ready to be used.
            READY_BUT_FAIL     // don't need this
        };
    protected:
        Status status = UNLOADED;
        name_t name = 0;
        bool load_fail = false;
        size_t references = 0;
    public:
        Resource(){}
        Resource(name_t name) : name(name) {}
        inline Resource::Status GetStatus() const {return status;}
        inline uint64_t GetName() {return name;};
        inline size_t GetRef() {return references;}
        inline void AddRef(){references++;}
        inline void RemoveRef(){references--;}
        void virtual LoadFromDisk() = 0;
        void virtual LoadFromMemory() = 0;
    };


    namespace Async { void RequestResource (EntityComponent* requester, Resource* requested_resource); }

    template <typename T>
    class ResourceProxy {
    public:
        ResourceProxy(EntityComponent* parent) : parent(parent) {}
        ~ResourceProxy() { set(nullptr); }
        void set(Resource* res){
            if (resource) resource->RemoveRef();

            resource = res;
            
            if (resource) {
                resource->AddRef();
                
                if (resource->GetStatus() != Resource::READY) {
                    parent->resources_waiting++;
                    Async::RequestResource(parent, resource);
                }
            }
        }
        T* get() { return (T*)resource; }
        T* operator= (Resource* res) { set(res); return get(); }
        T* operator->() { return (T*)resource; }
        T& operator*() { return *((T*)resource); }
        explicit operator bool() { return resource != nullptr; }
    protected:
        EntityComponent* parent;
        Resource* resource = nullptr;
    };


    namespace UI {
        void SetText(const char* text, uint32_t x, uint32_t y, float scale, float width, bool justify, bool stretch, uint32_t font, const glm::vec3& color);

        void SetDebugText(const char* text, const glm::vec3& location, const glm::vec3& color);
    }

    // TODO: review the interface and improve if possible
    // also probably take this out into another header file
    namespace Stats {
        enum Type {
            FRAME,
            FRAME_NO_SWAP,
            RENDER,
            RENDERSORT,
            PHYSICS
        };

        extern double total_time[sizeof(Stats::Type)];
        extern double start_time[sizeof(Stats::Type)];
        extern float final_time[sizeof(Stats::Type)];
        extern float final_time_cum[sizeof(Stats::Type)];
        extern float final_time_avg[sizeof(Stats::Type)];

        extern uint64_t full_frame;
        extern size_t frame_passed;

        extern uint64_t renderobjects;
        extern uint64_t animationcomponents;
        extern uint64_t physicscomponents;

        void Start(Stats::Type stat);

        void Stop(Stats::Type stat);

        void Reset();

        void Update();

        class Counter {
        public:
            Counter(Stats::Type statToBeCounted){
                stat = statToBeCounted;
                Start(stat);
            }

            ~Counter(){
                Stop(stat);
            }
        private:
            Stats::Type stat;
        };

    }



}

#endif
