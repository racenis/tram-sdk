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
        std::string print_name; //name of queue for log messages etc.
        uint64_t current_size;  //how many elements are in queue right now
        uint64_t full_size;     //how many elements can be added to queue
        T* first;               //first element in queue
        T* last;                //one past last element in queue
        T* first_end;
        T* last_end;
    public:
        Stack(std::string name, uint64_t initial_size){
            print_name = name;
            full_size = initial_size;
            current_size = 0;

            char* newmemory = (char*)::operator new(initial_size * sizeof(T));
            first = (T*)newmemory;
            last = first;
            first_end = first;
            last_end = (T*)newmemory + (initial_size * sizeof(T));
        }
        
        T& top() {
            return *GetLastPtr();
        }
        
        void push(const T& value) {
            *AddNew() = value;
        }
        
        T pop() {
            T value = top();
            Remove();
            return value;
        }
        
        void reset() {
            current_size = 0;
            last = first;
        }
        
        size_t size() {
            return current_size;
        }
        
        // aliases, don't use for new code
        T* AddNew() { return AllocateTop(); }
        void Remove() { RemoveTop(); }
        T* GetLastPtr() { return GetTop(); }
        uint64_t GetLength() { return size(); }
        void Reset() { reset(); }
        
    protected:
        T* AllocateTop() {
            if (current_size == full_size) {
                std::cout << "Stack " << print_name << " out of space!" << std::endl;
                return nullptr;
            }

            T* newobj = last;
            new(newobj) T;
            last++;
            current_size++;
            return newobj;
        }
        
        void RemoveTop()  {
            if (last == first_end) {
                std::cout << "Stack " << print_name << " already empty!" << std::endl;
            };
            last--;
            current_size--;
        }
        
        T* GetTop() {
            if (current_size == 0) {
                return nullptr;
            } else {
                return last - 1;
            }
        }
    };
}

#endif  // TRAM_SDK_TEMPLATES_STACK_H