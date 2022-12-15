#include <framework/message.h>
#include <framework/entity.h>

namespace Core {
    Queue<Message> Core::Message::queue("message queue", 500);

    void Message::Dispatch(){
        Message* message = Message::queue.GetFirstPtr();
        while(message){
            Entity* receiver = Entity::Find(message->receiverID);
            if (receiver) receiver->MessageHandler(*message);

            Message::queue.Remove();
            message = Message::queue.GetFirstPtr();
        }
    }
    
    void Message::Send(Message &message){
        *(Message::queue.AddNew()) = message;
    }
}
