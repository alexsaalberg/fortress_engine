//
//  Messaging.h
//  CastleSim
//

//  https://gamedev.stackexchange.com/questions/47476/tips-for-component-based-entity-system-message-handling/47483#47483
//

#include <vector>

#ifndef Messaging_h
#define Messaging_h

//RECEIVER
class BaseReceiver {
protected:
    static size_t nextId;
};

template <typename EventDataType>
class Receiver : public BaseReceiver {
public:
    virtual void receive(const EventDataType&) = 0;
    
    static size_t GetId() {
        return receiver_id;
    }
private:
    static size_t receiver_id;
};

template <typename T>
size_t Receiver<T>::receiver_id = BaseReceiver::nextId++;

//EVENT
struct BaseEvent {
protected:
    static size_t nextId;
};


template <typename T>
struct Event : public BaseEvent {
public:
    static size_t GetId() {
        return event_id;
    }
private:
    static size_t event_id;
};

template <typename T>
size_t Event<T>::event_id = BaseEvent::nextId++;

//HANDLER
class EventHandler {
private:
    std::vector< std::vector<BaseReceiver*> > subscribers;
public:
    template <typename EventType>
    void emit(const EventType& event) {
        std::vector<BaseReceiver*> receivers = subscribers[EventType::GetId()];
        for(int i = 0; i < receivers.size(); i++) {
            BaseReceiver* receiver = receivers[i];
            static_cast<Receiver<EventType>&>( *receiver ).receive(event);
        }
    }
    
    template <typename EventType, typename ReceiverType>
    void subscribe(ReceiverType* receiver) {
        size_t eventId = EventType::GetId();
        size_t receiverId = ReceiverType::GetId();
        
        if(subscribers.size() <= eventId) {
            subscribers.resize(eventId + 1);
        }
    
        std::vector<BaseReceiver*>& receivers = subscribers[eventId];
        
        if(receivers.size() <= receiverId) {
            receivers.resize(receiverId + 1);
        }
        
        receivers[receiverId] = receiver;
    }
    
    template < typename EventType, typename ReceiverType>
    void unsubscribe(ReceiverType* receiver) {
        subscribers[EventType::GetId()][ReceiverType::GetId()] = NULL;
    }
};

//Test Event
struct MouseClickEvent : Event<MouseClickEvent> {
    double x;
    double y;
    int button;
    
    MouseClickEvent(double x, double y, int button) : x(x), y(y), button(button) {}
};

#endif /* Messaging_h */
