/*
#pragma once

#include "Containers.h"

#include <queue>

namespace ShooterCentral {

struct Status {
    bool didSucceed { false };
    char msg[256] = { };
};


class ModelEvent {
public:
    ModelEvent();
    virtual ~ModelEvent();

    virtual Status execute (Container& container) = 0;
};

class UIEvent {
public:
    UIEvent();
    virtual ~UIEvent();

    virtual Status execute (UIController& controller) = 0;
};


Status pushEvent    (std::unique_ptr<ModelEvent> event);
Status pushEvent    (std::unique_ptr<UIEvent> event);

void pollEvent      (std::unique_ptr<ModelEvent>& event);
void pollEvent      (std::unique_ptr<UI>& event);

}
*/
