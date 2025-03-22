// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_STACK_H
#define TRAM_SDK_TEMPLATES_STACK_H

#include <string>
#include <cstdint>
#include <iostream>

/* copy-pasting queue code might have not been the best idea. anyway, it would
 * be nice to yeet all of the old names of methods an keep only the new aliases.
 * 
 * the internal stuff should also be rewritten to be actually understandable
 */

namespace tram {
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
        void push(const T& value) { *AddNew() = value; }
        T pop() { T value = top(); Remove(); return value; }
        void reset() { Reset(); }
        size_t size() {return GetLength(); }
    };
}

#endif  // TRAM_SDK_TEMPLATES_STACK_H