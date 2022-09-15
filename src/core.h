// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CORE.H -- Engine core definitons
// Basic engine datatypes, templates, useful functions.

// to-do list:
// - Add a component proxy
// - Streamable animations
// - Custom fonts without modifying the engine
// - Make ResourceProxy check if template instantiaties with resource subclass
// - Research vulkan, directx and other rendering APIs and update the rendering abstractions

#ifndef TRAM_CORE_H
#define TRAM_CORE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <atomic>
#include <algorithm>

namespace Core {
    template <typename T>
    class Pool {
    protected:
        std::string poolName;
        uint64_t poolSize;
        uint64_t maxSize;
        T* first;
        T* last;
        T* lastfree;
    public:
        struct iterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            
            iterator(T* ptr) : ptr(ptr) {}
            
            T& operator*() const { return *ptr; }
            T* operator->() { return ptr; }

            iterator& operator++() { do ptr++; while (*((uint64_t*)ptr) == 0 && *(((uint64_t*)ptr) + 1) != 0); return *this; }  

            iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

            friend bool operator== (const iterator& a, const iterator& b) { return a.ptr == b.ptr; };
            friend bool operator!= (const iterator& a, const iterator& b) { return a.ptr != b.ptr; };  
            
            T* ptr;
        };
    
        Pool(std::string name, uint64_t initialSize, bool skipFirst = false){
            poolName = name;
            maxSize = initialSize;
            poolSize = skipFirst ? 1 : 0;

            first = static_cast<T*>(::operator new((initialSize * sizeof(T)) + sizeof(uint64_t) * 2));
            last = first;
            lastfree = first;

            if(skipFirst){
                new(first) T;
                last++;
                lastfree++;
            }
            
            *((uint64_t*)last) = 0;
            *(((uint64_t*)last) + 1) = 0;

        };
        
        template <typename... Args>
        T* AddNew(Args&&... args){
            if(poolSize == maxSize){
                std::cout << "Pool " << poolName << " out of space!" << std::endl;
                abort();
            }

            T* newobj;

            if(lastfree != last){
                newobj = lastfree;
                uint64_t* skip = reinterpret_cast<uint64_t*>(lastfree);
                skip++;
                T** skip2 = reinterpret_cast<T**>(skip);
                lastfree = *skip2;
            } else {
                newobj = lastfree;
                poolSize++;
                last++;
                lastfree++;
                
                *((uint64_t*)last) = 0;
                *(((uint64_t*)last) + 1) = 0;
            }
            
            new(newobj) T(std::forward<Args>(args)...);

            return newobj;
        };
        
        void Remove(T* removeptr){
            assert(removeptr >= first && removeptr < last); // pointer is in pool
            removeptr->~T(); // destruct
            uint64_t* skip = reinterpret_cast<uint64_t*>(removeptr);
            *skip = 0; // mark as empty
            skip++;
            T** nextfree = reinterpret_cast<T**>(skip);
            *nextfree = lastfree; // add pointer to previous free place
            lastfree = removeptr;
        };
        T& operator[](size_t index) { return *(first + index); } // note that there is no checking for whether the index is valid
        T* GetFirst(){return first;};   // yeet?
        T* GetLast(){return last;};     // also yeet?
        iterator begin(){return first;};
        iterator end(){return last;};
        size_t GetSize(){return poolSize;}; // yeet too?
        size_t size(){return poolSize;};

        // make sure that there will be enough room for the empty place marker and free list pointer
        static_assert(sizeof(T) >= sizeof(T*) + sizeof(uint64_t));
    };

    template <typename T>
    class StackPool {
    protected:
        std::string poolName;
        size_t poolSize;
        size_t maxSize;
        T* first;
        T* last;
    public:
        StackPool(){};
        StackPool(std::string name, size_t initialSize){
            poolName = name;
            maxSize = initialSize;
            poolSize = 0;

            first = static_cast<T*>(::operator new(initialSize * sizeof(T)));
            last = first;
        };
        T* AddNew(size_t units){
            if(poolSize + units > maxSize){
                std::cout << "Pool " << poolName << " out of space!" << std::endl;
                return nullptr;
            }

            T* newobj;

            newobj = last;
            poolSize += units;
            last += units;

            return newobj;
        };
        void Reset(){
            //return;
            poolSize = 0;
            last = first;
        };
        size_t GetSize(){return poolSize;};
        T* GetFirst(){return first;};
        T* GetLast(){return last;};
        T* begin(){return first;};
        T* end(){return last;};
    };

    template <typename T>
    class Queue{
    protected:
        std::string queueName;  //name of queue for log messages etc.
        uint64_t queueLength;   //how many elements are in queue right now
        uint64_t queueSize;     //how many elements can be added to queue
        T* first;               //first element in queue
        T* last;                //one past last element in queue
        T* firstend;
        T* lastend;
        std::atomic<bool> spinlock = {false};
    public:
        Queue(std::string name, uint64_t initialSize){
            queueName = name;
            queueSize = initialSize;
            queueLength = 0;

            char* newmemory = (char*)::operator new(initialSize * sizeof(T));
            first = (T*)newmemory;
            last = first;
            firstend = first;
            lastend = (T*)(newmemory + (initialSize * sizeof(T)));
        };
        T* AddNew(){
            if(queueLength == queueSize){
                std::cout << "Queue " << queueName << " out of space!" << std::endl;
                return nullptr;
            }

            T* newobj = last;
            new(newobj) T;
            last++;
            queueLength++;
            if(last == lastend) last = firstend;
            return newobj;
        };
        void Remove(){
            first++;
            queueLength--;
            if(first == lastend) first = firstend;
        };
        T* GetFirstPtr(){
            if(first == last)
                return nullptr;
            else
                return first;
        };
        uint64_t GetLength(){return queueLength;};

        // TODO: add a class to do locking/unlocking with RAII
        void Lock(){while(spinlock.exchange(true));}
        void Unlock(){spinlock.store(false);}
    };

    template <typename T>
    class Stack{
    protected:
        // TODO: figure out what is going on in here???
        std::string stackName;  //name of queue for log messages etc.
        uint64_t stackLength;   //how many elements are in queue right now
        uint64_t stackSize;     //how many elements can be added to queue
        T* first;               //first element in queue
        T* last;                //one past last element in queue
        T* firstend;
        T* lastend;
    public:
        Stack(std::string name, uint64_t initialSize){
            stackName = name;
            stackSize = initialSize;
            stackLength = 0;

            char* newmemory = (char*)::operator new(initialSize * sizeof(T));
            first = (T*)newmemory;
            last = first;
            firstend = first;
            lastend = (T*)newmemory + (initialSize * sizeof(T));
        };
        T* AddNew(){
            if(stackLength == stackSize){
                std::cout << "Stack " << stackName << " out of space!" << std::endl;
                return nullptr;
            }

            T* newobj = last;
            new(newobj) T;
            last++;
            stackLength++;
            return newobj;
        };
        void Remove(){
            if(last == firstend){
                std::cout << "Stack " << stackName << " already empty!" << std::endl;
            };
            last--;
            stackLength--;
        };
        T* GetLastPtr(){
            if (stackLength == 0)
                return nullptr;
            else
                return last - 1;
        };
        uint64_t GetLength(){return stackLength;};
        void Reset(){
            stackLength = 0;
            last = first;
        }
        T& top() { return *GetLastPtr(); }
    };

    template <typename T>
    class Octree {
        // TODO: actually implement the octree
        struct Node {
            uint32_t id;
            float x, y, z;
            T type;
        };

        std::vector<Node> nodevec;
        uint32_t last_id = 1;
    public:

        uint32_t AddLeaf(T type, float x, float y, float z){
            //Node n = {last_id, x, y, z, type};
            nodevec.push_back(Node{last_id, x, y, z, type});
            last_id++;
            return last_id - 1;
        }

        void RemoveLeaf(uint32_t leaf_id){
            nodevec.erase(std::find_if(nodevec.begin(), nodevec.end(), [=](Node& n){return n.id == leaf_id;}));
        }

        size_t FindNearest(T result[], float x, float y, float z){
            std::sort(nodevec.begin(), nodevec.end(), [=](const Node& a, const Node& b){return glm::distance(glm::vec3(x, y, z), glm::vec3(a.x, a.y, a.z)) > glm::distance(glm::vec3(x, y, z), glm::vec3(b.x, b.y, b.z));});
            for (size_t i = 0; i < 4 && i < nodevec.size(); i++){
                result[i] = nodevec[i].type;
            }
            return nodevec.size() > 4 ? 4 : nodevec.size();
        }


    };

    template <typename T>
    class PoolProxy {
    public:
        static T* New(){return pool.AddNew();}
        static void Delete(T* obj){pool.Remove(obj);}
        static Pool<T>& GetPool(){return pool;}
    protected:
        static Pool<T> pool;
    };

    template <typename T> using PoopProxy = PoolProxy<T>;
    typedef uint64_t name_t;


    extern bool SHOULD_CLOSE;
    extern float FRAME_TIME;
    
    extern bool DRAW_PHYSICS_DEBUG;
    extern bool DRAW_RENDER_DEBUG;
    extern bool DRAW_PATH_DEBUG;

    // forward declarations

    // TODO: check which ones can be yeeted
    class WorldCell;
    class EntityComponent;

    uint64_t UID(std::string name);
    uint64_t UID(std::string_view name, size_t length);
    name_t FindLangStr(name_t name);
    const char* ReverseUID(uint64_t uid);
    
    void LoadText(const char* filename);
    void LoadPath(const char* filename);

    void Init();

    void QuatLookAt(glm::quat& quaternion, const glm::vec3& from, const glm::vec3& to);
    void ProjectLine(glm::vec3& point, const glm::vec3& from, const glm::vec3& to);

    uint64_t GenerateID();


    struct Event {
        enum Type : uint16_t {
            KEYPRESS,
            KEYDOWN,
            KEYUP,
            CURSORPOS
        };

        struct Listener {
            EntityComponent* ent = nullptr; // TODO: rename this
            uint64_t poster = 0;
            glm::vec3 location;
            float dist = 0.0f;
            uint16_t typemask = 0;
        };

        static void Post (Event &event);
        static void Dispatch();
        static Event::Listener* AddListener(Event::Type type);
        static void RemoveListener(Event::Type type, Event::Listener* listener);

        Event::Type type;
        uint16_t subtype;
        glm::vec3 location;
        uint64_t posterID;
        void* data;

        // there's probably a way better way to do this, but this works...
        static Pool<Event::Listener> listeners[10];
        static Queue<Event> queue;
    };

    struct Message {
        enum Type : uint64_t {
            PING,
            HELLO,
            ACTIVATE,
            GET_IN,
            GET_OUT
        };

        static void Send (Message &message);
        static void Dispatch();

        Message::Type type;
        uint64_t receiverID;
        uint64_t senderID;
        void* data = nullptr;

        static Queue<Message> queue;
    };

    struct Path {
        struct Node {
            Node* next;
            Node* prev;
            Node* left;
            Node* right;
            glm::vec3 p1;
            glm::vec3 p2;
            glm::vec3 p3;
            glm::vec3 p4;
            float lens[10];
            
            void ProducePoint(glm::vec3& p, const float& t);
            void CalculateLenghts();
            void Render();
            
            struct Follower {
                Node* current_node;
                float t = 0.0f;
                
                void GoForth(float ammount);
                void Render();
                void GetPosition(glm::vec3& pos);
            };
        };
        
        name_t name = 0;
        std::vector<Node> nodes;
    };
    
    
    struct Navmesh {
        struct Node {
            Node* next;
            Node* prev;
            Node* left;
            Node* right;
            glm::vec3 location;
        };
        
        name_t name = 0;
        std::vector<Node> nodes;
    };


    //fwd decl
    class SerializedEntityData; // TODO: move

    /// Base class for all entities.
    class Entity {
    public:
        /// Load the entity's components.
        virtual void Load() = 0;

        /// Unload the entity's components.
        virtual void Unload() = 0;

        /// Pack the entity's components' data into serialized form.
        virtual void Serialize() = 0;

        /// Creates to create this entity.
        void SerializeString(std::string& str);

        Entity() = default;

        /// Generates ID's for the entity and registers it into the rest of the systems.
        inline void Init(){
            if(!id){
                id = GenerateID();
                Register();
            }
        }

        /// Prepares the entity for removal.
        void Yeet();

        [[nodiscard]] inline name_t GetName() const { return name; }
        [[nodiscard]] inline uint64_t GetID() const { return id; }
        [[nodiscard]] inline bool IsLoaded() const { return isloaded; }
        [[nodiscard]] inline bool IsAutoLoad() const { return auto_load; }
        inline bool IsInInterior() { return in_interior; }
        inline bool IsPersistent() { return  persistent; }
        inline bool IsChanged() { return changed; }

        /// Parses the default parameters that every entity should have.
        void SetParameters (std::string_view& params);

        /// Like SetParameters, but in reverse.
        void ParametersString(std::string& str);

        void virtual UpdateParameters() = 0;
        
        void virtual SetParameters() = 0;

        /// Sets the translation of the entity.
        void SetLocation(float x, float y, float z){ location = glm::vec3(x, y, z); SetParameters(); CheckTransition(); }
        void SetLocation(const glm::vec3& loc) { location = loc; SetParameters(); CheckTransition();}

        /// Sets the rotation of the entity.
        void SetRotation(float x, float y, float z){ rotation = glm::quat(glm::vec3(x, y, z)); SetParameters(); }
        void SetRotation(glm::quat& rot){ rotation = rot; SetParameters(); }

        /// Sets both the rotation and the translation of the entity.
        inline void UpdateTransform(const glm::vec3& loc, const glm::quat& rot){
            location = loc;
            rotation = rot;
            UpdateParameters();
            CheckTransition();
        }

        /// Translates the entity by a vector.
        inline void Move(const glm::vec3& move_by){
            SetLocation(location + move_by);
        }

        inline void GetLocation(glm::vec3& loc){ loc = location; }
        inline const glm::vec3& GetLocation(){ return location; }

        inline void GetRotation(glm::quat& rot){ rot = rotation; }
        inline const glm::quat& GetRotation(){ return rotation; }

        /// Each entity type implements this so that they can respond to messages.
        virtual void MessageHandler(Message& msg) = 0;

        void CheckTransition();

        void RemoveSerializedData();

        void Activate (Message& msg){
            std::cout << ReverseUID(name) << " got activated" << std::endl;
            return;
        }

        /// Creates an entity from an initialization string and returns a pointer to it.
        static Entity* Make (std::string_view& params);

        /// Registers an entity type for the Make method.
        static void Register (const char* name, Entity* (*constr_func)(std::string_view& params));

        /// Registers an entity into the system.
        void Register();

        /// Unregisters an entity from the system.
        void Deregister();

        /// Finds a registered entity from its ID.
        static Entity* Find (uint64_t entityID);

        /// Finds a registered entity from its name.
        static Entity* FindName(name_t entityName);
    protected:
        /// Unique ID number.
        uint64_t id = 0;

        /// Name of the entity. Set to 0 if entity has no name.
        name_t name = 0;

        /// Command list that will be executed if entity is activated. Set to 0 if no command list is to be executed.
        name_t action = 0;

        // TODO: rename this one to is_loaded
        bool isloaded = false;

        /// If the entity will be loaded with the cell it is in.
        bool auto_load = true;

        /// If the cell the entity is in is an interior.
        bool in_interior = false;

        /// Will the entity be deleted when it is unloaded.
        bool persistent = true;

        /// Will the entity be saved.
        bool changed = false;

        /// Cell the entity is in.
        WorldCell* cell = nullptr;

        /// Rotation of the entity around its origin in world space.
        glm::quat rotation = {0.0f, 0.0f, 0.0f, 1.0f};

        /// Translation of the entity in world space.
        glm::vec3 location = {0.0f, 0.0f, 0.0f};

        /// Pointer to serialized entity data. Will actually be of whichever type the entity has.
        SerializedEntityData* serialized_data = nullptr; // TODO: yeet this

        static std::unordered_map<uint64_t, Entity*> List;
        static std::unordered_map<name_t, Entity*> NameList;
        static std::unordered_map<std::string, Entity* (*)(std::string_view& params)> entityConstructors;
        friend class WorldCell;
    };


    /// Base class of Entity components.
    class EntityComponent {
    public:
        EntityComponent(){ id = GenerateID(); }

        virtual void Init() = 0;

        virtual void Uninit() = 0;

        virtual void EventHandler(Event &event) = 0;

        inline bool IsReady(){ return is_ready; }

        inline uint64_t GetID(){ return id; }

        void ResourceReady(){ resources_waiting--; if (resources_waiting == 0) Start();}
    protected:
        uint64_t id;
        size_t resources_waiting = 0;
        bool is_ready = false;

        virtual void Start() = 0;
        template <typename T> friend class ResourceProxy;
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
        bool interior = false;                  /// Is the cell an interior.
        bool interiorLights = false;            /// Does the cell have interior lighting.
        bool loaded = false;                    /// Is the cell loaded.
        bool draw = false;
        std::vector<Entity*> entities;          /// List of pointers to entities that are in this cell.
        std::vector<Transition*> trans_in;      /// List of pointers to transitions into the cell.
        std::vector<Transition*> trans_out;     /// List of pointers to transitions out of the cell into other cells.
        std::vector<Path*> paths;
        std::vector<Navmesh*> navmeshes;
        static std::unordered_map<uint64_t, WorldCell*> List;
        friend void LoadCells();
    public:
        WorldCell(){};
        //WorldCell(uint64_t cellName, bool isInterior, bool hasInteriorLighting);

        bool IsLoaded() {return loaded;};

        bool IsInterior() {return interior;};
        bool HasInteriorLighting() {return interiorLights;};
        bool IsDrawn() {return draw;};
        
        
        name_t GetName(){return name;};   

        void SetName(name_t new_name);
        
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

    // rather inelegant, this forward declaration
    namespace Async {void RequestResource (EntityComponent* requester, Resource* requested_resource);}

    template <typename T>
    class ResourceProxy {
    public:
        ResourceProxy(EntityComponent* parent) : parent(parent) {}
        void SetResource(Resource* res){
            if(resource)resource->RemoveRef();

            resource = res;
            resource->AddRef();

            parent->resources_waiting++;


            Async::RequestResource(parent, resource);
        }
        T* GetResource() { return (T*)resource; }
        T* operator->() { return (T*)resource; }
    protected:
        EntityComponent* parent;
        Resource* resource = nullptr;
    };


    namespace UI {
        void SetText(const char* text, uint32_t x, uint32_t y, float scale, float width, bool justify, bool stretch, uint32_t font, const glm::vec3& color);

        void SetDebugText(const char* text, const glm::vec3& location, const glm::vec3& color);
    }

    // TODO: review the interface and improve if possible
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
