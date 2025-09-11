#pragma once

#include "Containers.h"
#include "View.h"

#include <memory>
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

    virtual Status execute (Containers& container) = 0;
};

class UIEvent {
public:
    UIEvent();
    virtual ~UIEvent();

    virtual Status execute (UI::UIController& controller) = 0;
};

bool pushEvent(std::unique_ptr<ModelEvent> event);
bool pushEvent(std::unique_ptr<UIEvent> event);

void pollEvent (std::unique_ptr<ModelEvent>& event);
void pollEvent (std::unique_ptr<UIEvent>& event);
    
} // End ShooterCentral namespace
