#include <framework/message.h>
#include <framework/entity.h>

namespace Core {
    Queue<Message> Core::Message::queue("message queue", 500);
    
    /// Dispatches the messages from the message queue.
    void Message::Dispatch(){
        Message* message = Message::queue.GetFirstPtr();
        while(message){
            Entity* receiver = Entity::FindByID(message->receiverID);
            if (receiver) receiver->MessageHandler(*message);

            Message::queue.Remove();
            message = Message::queue.GetFirstPtr();
        }
    }
    
    /// Adds a message to the message queue.
    void Message::Send(Message &message){
        *(Message::queue.AddNew()) = message;
    }
}
