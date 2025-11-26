#pragma once

#include <memory>
#include <queue>

// Macros for each type of event to use in the object declaration of derived Events
#define DATABASE_EVENT_FUNCTIONS(type)  virtual Status execute (Database& ) override; \
                                        virtual std::unique_ptr<DatabaseEvent> clone() const override { return std::make_unique<type>(*this); };

#define UI_EVENT_FUNCTIONS(type)        virtual Status execute (UIController& ) override; \
                                        virtual std::unique_ptr<UIEvent>    clone() const override { return std::make_unique<type>(*this); };


namespace ShooterCentral {

// Forward Declarations
class Database;

namespace UI {
    class UIController;
}


struct Status {
    bool didSucceed { false };
    char msg[256] = { };
};


class DatabaseEvent {
public:
    DatabaseEvent() = default;
    virtual ~DatabaseEvent() = default;

    virtual Status execute (Database& ) = 0;
    virtual std::unique_ptr<DatabaseEvent> clone() const = 0;
};

class UIEvent {
public:
    UIEvent() = default;
    virtual ~UIEvent() = default;

    virtual Status execute (UI::UIController& ) = 0;
    virtual std::unique_ptr<UIEvent> clone() const = 0;
};

bool pushEvent(DatabaseEvent* event);
bool pushEvent(UIEvent* event);

void pollEvent (std::unique_ptr<DatabaseEvent>& event);
void pollEvent (std::unique_ptr<UIEvent>& event);
    
} // End ShooterCentral namespace
