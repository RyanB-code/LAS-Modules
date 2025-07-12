#include "Framework.h"

using namespace ShooterCentral;
using namespace LAS;

Framework::Framework() : view {
    ContainerItrs {
        model.knownAmmo_cbegin(),
        model.knownAmmo_cend(),

        model.knownGuns_cbegin(),
        model.knownGuns_cend(),

        model.ammoStockpile_cbegin(),
        model.ammoStockpile_cend(),

        model.gunsInArmory_cbegin(),
        model.gunsInArmory_cend(),

        model.events_cbegin(),
        model.events_cend()
    }

}
{

}
Framework::~Framework(){

}
bool Framework::setup(const std::string& directory, std::shared_ptr<bool> setShown){
    using namespace ShooterCentral::Setup;

    Filepaths paths {directory};

    if(!setupFilesystem(paths)){
        log_critical("Failed to setup filesystem");
        return false;
    }

    shown = setShown;
   
    log_info("SC Setup sucessful");
    return true;
}
void Framework::update() {

}
void Framework::draw() {
   
    if(!ImGui::Begin(TITLE, &*shown, ImGuiWindowFlags_MenuBar)){
        ImGui::End();
        return;
    }

    ContainerItrs itrs {
        model.knownAmmo_cbegin(),
        model.knownAmmo_cend(),

        model.knownGuns_cbegin(),
        model.knownGuns_cend(),

        model.ammoStockpile_cbegin(),
        model.ammoStockpile_cend(),

        model.gunsInArmory_cbegin(),
        model.gunsInArmory_cend(),

        model.events_cbegin(),
        model.events_cend()
    };

    view.draw(itrs, unsavedChanges);

    ImGui::End();
}


bool Setup::setupFilesystem(Framework::Filepaths& paths){
    if(paths.parentDir.empty())
        return false;

    paths.parentDir = LAS::TextManip::ensureSlash(paths.parentDir);

    paths.ammoDir         = paths.parentDir + "Ammo/";
    paths.eventsDir       = paths.parentDir + "Events/";
    paths.gunsDir         = paths.parentDir + "Guns/";

    // Check paths are good
    if(!LAS::ensureDirectory(paths.ammoDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.ammoDir));
        return false;
    }
    if(!LAS::ensureDirectory(paths.eventsDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.eventsDir));
        return false;
    }
    if(!LAS::ensureDirectory(paths.gunsDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.gunsDir));
        return false;
    }

    return true;
}
 
