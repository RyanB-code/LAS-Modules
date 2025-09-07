#pragma once

namespace ShooterCentral {

struct EventStatus {
    bool didSucceed { false };
    char msg[256];
};


class Event {
public:
    Event::Event();
    virtual ~Event();

    virtual EventStatus execute   
    
private:

};


}
