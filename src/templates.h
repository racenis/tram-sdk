// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// TEMPLATES.H -- Templates
// Various custom container templates.

#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <string>
#include <iostream>
#include <atomic>

// these next ones are just for octree placeholder
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

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
}
#endif // TEMPLATES_H