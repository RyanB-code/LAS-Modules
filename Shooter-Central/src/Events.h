#pragma once

#include <memory>
#include <queue>

#define MODEL_EVENT_CLONE(type) virtual std::unique_ptr<ModelEvent> clone() const override { return std::make_unique<type>(*this); };
#define UI_EVENT_CLONE(type)    virtual std::unique_ptr<UIEvent>    clone() const override { return std::make_unique<type>(*this); };


namespace ShooterCentral {

// Forward Declarations
class Containers;

namespace UI {
    class UIController;
}


struct Status {
    bool didSucceed { false };
    char msg[256] = { };
};


class ModelEvent {
public:
    ModelEvent() = default;
    virtual ~ModelEvent() = default;

    virtual Status execute (Containers& container) = 0;
    virtual std::unique_ptr<ModelEvent> clone() const = 0;
};

class UIEvent {
public:
    UIEvent() = default;
    virtual ~UIEvent() = default;

    virtual Status execute (UI::UIController& controller) = 0;
    virtual std::unique_ptr<UIEvent> clone() const = 0;
};

bool pushEvent(ModelEvent* event);
bool pushEvent(UIEvent* event);

void pollEvent (std::unique_ptr<ModelEvent>& event);
void pollEvent (std::unique_ptr<UIEvent>& event);
    
} // End ShooterCentral namespace
