#pragma once

#include "UI/UIData.h"
#include "UI/UIControl.h"
#include "Events.h"

#include <imgui/imgui.h>
#include <LAS/Logging.h>

namespace ShooterCentral::UI {

void centerNextItemX        (float x);
void centerNextItemY        (float y);
void centerNextComboBoxX    (const std::string& text, float comboBoxWidth);

void centerText             (const std::string& text);
void centerTextDisabled     (const std::string& text);
bool centerButton           (const std::string& text, ImVec2 size);

std::string categoryToString    (const Category& category,  const std::string& noneText=""); // noneText - Choose what to display when none is selected
std::string subItemToString     (const SubItem& item,       const std::string& noneText=""); // noneText - Choose what to display when none is selected
                                                                                             
void resetText (char* dest, size_t size, const char* replacementText=""); // Does nothing if paremeters are invalid for any reason


class Popup {
public:
    Popup(const char* title);
    virtual ~Popup();

    virtual void show() = 0;
    void close() const;

    const char* getTitle() const;

    void buttons_Close()   const;
    bool buttons_YesOrNo() const; // Returns true if YES is chosen, false if not chosen yet

private:
    char title[32] = "";
};

namespace UIEvents {
    class ShowPopup : public UIEvent {
    public: 
        ShowPopup(std::shared_ptr<Popup>);
        ~ShowPopup() = default;

        UI_EVENT_FUNCTIONS(ShowPopup)
    private:
        std::shared_ptr<Popup> popup; 
    };

    class ClosePopup : public UIEvent {
    public:
        ClosePopup() = default;
        ~ClosePopup() = default;


        UI_EVENT_FUNCTIONS(ClosePopup)
    };

}   // End UIEvents namespace


}   // End UI namespace
