#include "GUI/UIEvents.h"

namespace ShooterCentral::UI {

SetScreen::SetScreen(const Screen& setScreen) : screen { setScreen } {
        
}
SetScreen::~SetScreen(){

}
Status SetScreen::execute (UIController& controller){
    controller.setScreen(screen);
    return Status{true};
}
ShowPopup::ShowPopup(const char* msg) {

}
ShowPopup::~ShowPopup(){

}
Status ShowPopup::execute(UIController& controller) {
    return Status { false };
}


} // End UI namespace
