// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TEMPLATES_STACK_H
#define TEMPLATES_STACK_H

#include <string>
#include <iostream>

namespace Core {
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
}

#endif  // TEMPLATES_STACK_H