#include "Events.h"

namespace ShooterCentral {


static std::queue<std::unique_ptr<UIEvent>> eventQueue_UI               { };
static std::queue<std::unique_ptr<DatabaseEvent>> eventQueue_Database   { };

bool pushEvent(DatabaseEvent* rawEvent){
    if(!rawEvent)
        return false;

    std::unique_ptr<DatabaseEvent> event {rawEvent->clone()};

    eventQueue_Database.push(std::move(event));
    return true;
}
bool pushEvent(UIEvent* rawEvent){
    if(!rawEvent)
        return false;

    std::unique_ptr<UIEvent> event {rawEvent->clone()};

    eventQueue_UI.push(std::move(event));
    return true;
}


void pollEvent (std::unique_ptr<DatabaseEvent>& event){
    if(eventQueue_Database.empty()){
        event = nullptr;
        return;
    }

    event = std::move(eventQueue_Database.front());
    eventQueue_Database.pop();
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
