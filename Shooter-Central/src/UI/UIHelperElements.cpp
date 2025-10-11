#include "UI/UIHelperElements.h"

namespace ShooterCentral::UI {

void centerNextItemX(float x){
    float windowWidth { ImGui::GetContentRegionAvail().x };
    if(x < windowWidth)
        ImGui::SetCursorPosX((windowWidth - x) * 0.5f);

    return;
}
void centerNextItemY(float y){
    float windowHeight { ImGui::GetContentRegionAvail().y };
    if(y < windowHeight)
        ImGui::SetCursorPosY((windowHeight - y) * 0.5f);

    return;

}
void centerNextComboBoxX(const std::string& text, float comboBoxWidth){
    centerNextItemX(ImGui::CalcTextSize(text.c_str()).x + comboBoxWidth );
    ImGui::Text("%s", text.c_str()); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboBoxWidth);
}

void centerText(const std::string& text){
    float windowWidth { ImGui::GetWindowSize().x };
    float textWidth   { ImGui::CalcTextSize(text.c_str()).x };

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text.c_str());
}
void centerTextDisabled(const std::string& text){
    float windowWidth { ImGui::GetWindowSize().x };
    float textWidth   { ImGui::CalcTextSize(text.c_str()).x };

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextDisabled("%s", text.c_str());
}
bool centerButton(const std::string& text, ImVec2 size){
    float windowWidth = ImGui::GetWindowSize().x;

    ImGui::SetCursorPosX((windowWidth - size.x) * 0.5f);
    return ImGui::Button(text.c_str(), size);
}
std::string categoryToString(const Category& category, const std::string& noneStr){
    switch(category){
        case Category::NONE:
            if(noneStr.empty())
                return std::string {"None"};
            else
                return noneStr;
            break;
        case Category::GUNS:
            return std::string {"Guns"};
            break;
        case Category::EVENTS:
            return std::string {"Events"};
            break;
        case Category::STOCKPILE:
            return std::string {"Stockpile"};
            break;
        default:
            return std::string {"NOT HANDLED"};
            break;
    }
}
std::string subItemToString (const SubItem& item, const std::string& noneStr){
    switch(item){
        case SubItem::NONE:
            if(noneStr.empty())
                return std::string {"None"};
            else
                return noneStr;
            break;
        case SubItem::EVENT_EVENT:
            return std::string {"Event"};
            break;
        case SubItem::EVENT_TYPE:
            return std::string {"Event Type"};
            break;
        case SubItem::EVENT_LOCATION:
            return std::string {"Event Location"};
            break;
        case SubItem::AMMO_AMMO:
            return std::string {"Ammo"};
            break;
        case SubItem::AMMO_MANUFACTURER:
            return std::string {"Manufacuter"};
            break;
        case SubItem::GUN_AMMO_CARTRIDGE:
            return std::string {"Cartridge"};
            break;
        case SubItem::GUN_GUN:
            return  std::string {"Gun"};
            break;
        case SubItem::GUN_WEAPON_TYPE:
            return std::string {"Weapon Type"};
            break;
        default:
            return std::string {"NOT HANDLED"}; 
            break;
    }
}

void resetText (char* dest, size_t size, const char* replacementText) {
    if(!dest){
        LAS::log_warn("UI::resetText() - destination is null");
        return;
    }
    
    if(strlen(dest) > size) {
        LAS::log_warn("UI::resetText() - buffer > allowed size");
        return;
    }

    if(strlen(replacementText) > size) {
        LAS::log_warn("UI::resetText() - replacement text > allowed size");
        return;
    }

    memset(dest, '\0', size);
    std::strcpy(dest, replacementText);

    return;
}


Popup::Popup(const char* set) {
    strncpy(title, set, sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';            // Manually null-terminate
}
Popup::~Popup()  {

}
void Popup::close() const {
    ImGui::CloseCurrentPopup();

    UIEvents::ClosePopup close{ };
    pushEvent(&close);
}
const char* Popup::getTitle() const {
    return title;
}
void Popup::buttons_Close() const {
    if (centerButton("Close", ImVec2{120, 0}))
        close();
}
bool Popup::buttons_YesOrNo() const{ 
    float windowWidth { ImGui::GetWindowSize().x };
    float buttonWidth { 100 };

    ImGui::SetCursorPosX( (windowWidth * 0.5f) - (buttonWidth) - 3 ); // Subtracting 3 makes it seem more centered

    if(ImGui::Button("Yes", ImVec2 {buttonWidth, 0}))
        return true;

    ImGui::SameLine();

    if(ImGui::Button("No", ImVec2 {buttonWidth, 0})) 
       close(); 

    return false;
}

namespace UIEvents {

    ShowPopup::ShowPopup(std::shared_ptr<Popup> set) : popup { set } {

    }
    Status ShowPopup::execute(UIController& controller) {
        if(controller.setPopup(popup))
            return Status { true };

        return Status { false, "UIController::setPopup() failed" };
    }

    Status ClosePopup::execute(UIController& controller) {
        controller.closePopup();
        return Status { true };
    }


}   // End UIEvents namespace


}   // End UI namespace
