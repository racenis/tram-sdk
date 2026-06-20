// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_STACK_H
#define TRAM_SDK_TEMPLATES_STACK_H

#include <string>
#include <cstdint>
#include <framework/logging.h>

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
        
        T& top() const {
            return *GetTop();
        }
        
        void push(const T& value) {
            *AllocateTop() = value;
        }
        
        T pop() {
            T value = top();
            RemoveTop();
            return value;
        }
        
        void reset() {
            current_size = 0;
            last = first;
        }
        
        size_t size() const {
            return current_size;
        }
        
        // aliases, don't use for new code
        [[deprecated]] T* AddNew() { return AllocateTop(); }
        [[deprecated]] void Remove() { RemoveTop(); }
        [[deprecated]] T* GetLastPtr() const { return GetTop(); }
        [[deprecated]] uint64_t GetLength() { return size(); }
        [[deprecated]] void Reset() { reset(); }
        
    protected:
        T* AllocateTop() {
            if (current_size == full_size) {
                Log(Severity::CRITICAL_ERROR, System::CORE, "Stack {} out of space!", print_name);
                return nullptr;
            }

            T* newobj = last;
            new(newobj) T;
            last++;
            current_size++;
            return newobj;
        }
        
        void RemoveTop() {
            if (last == first_end) {
                Log(Severity::CRITICAL_ERROR, System::CORE, "Stack {} already empty!", print_name);
            };
            last--;
            current_size--;
        }
        
        T* GetTop() const {
            if (current_size == 0) {
                return nullptr;
            } else {
                return last - 1;
            }
        }
    };
}

#endif  // TRAM_SDK_TEMPLATES_STACK_H