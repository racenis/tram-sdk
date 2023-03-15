// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TEMPLATES_QUEUE_H
#define TEMPLATES_QUEUE_H

#include <string>
#include <iostream>
#include <atomic>

namespace tram {
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
}

#endif  // TEMPLATES_QUEUE_H