#include "Events.h"

namespace ShooterCentral{


static std::queue<std::unique_ptr<UIEvent>> eventQueue_UI           { };
static std::queue<std::unique_ptr<ModelEvent>> eventQueue_Model     { };

bool pushEvent(ModelEvent* rawEvent){
    if(!rawEvent)
        return false;

    std::unique_ptr<ModelEvent> event {rawEvent->clone()};

    eventQueue_Model.push(std::move(event));
    return true;
}
bool pushEvent(UIEvent* rawEvent){
    if(!rawEvent)
        return false;

    std::unique_ptr<UIEvent> event {rawEvent->clone()};

    eventQueue_UI.push(std::move(event));
    return true;
}


void pollEvent (std::unique_ptr<ModelEvent>& event){
    if(eventQueue_Model.empty()){
        event = nullptr;
        return;
    }

    event = std::move(eventQueue_Model.front());
    eventQueue_Model.pop();
}
void pollEvent (std::unique_ptr<UIEvent>& event){
    if(eventQueue_UI.empty()){
        event = nullptr;
        return;
    }

    event = std::move(eventQueue_UI.front());
    eventQueue_UI.pop();
}


} // End ShooterCentral namespace
