// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TEMPLATES_STACKPOOL_H
#define TEMPLATES_STACKPOOL_H

#include <string>
#include <iostream>

namespace Core {
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
}

#endif  // TEMPLATES_STACKPOOL_H