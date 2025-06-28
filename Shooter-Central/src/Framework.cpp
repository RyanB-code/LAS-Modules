#include "Framework.h"

using namespace ShooterCentral;
using namespace LAS;

Framework::Framework() {
    window = std::make_shared<ShooterCentralWindow>(ShooterCentralWindow{});
}
Framework::~Framework(){

}

// MARK: Public Functions
bool Framework::setup(const std::string& directory){
    Filesystem filesystem {directory};

    if(!setupFilesystem(filesystem)){
        log_critical("Failed to setup filesystem");
        return false;
    }
    if(!setupAmmoTracker(filesystem.ammoDir)){
        log_critical("Failed to setup Ammo Tracker");
        return false;
    }
    if(!setupGunTracker(filesystem.gunsDir)){
        log_critical("Failed to setup Gun Tracker");
        return false;
    }
    if(!setupEventTracker(filesystem.eventsDir)){
        log_critical("Failed to setup Event Tracker");
        return false;
    }
    if(!setupWindow()){
        log_critical("Failed to setup window");
        return false;
    }
   
    log_info("Setup sucessful");
    return true;
}
SCWindowPtr Framework::getWindow() const {
    return window;
}
std::string Framework::getCommandGroupName() const {
    return commandGroupName;
}


// MARK: PRIVATE FUNCTIONS
bool Framework::setupFilesystem(Filesystem& filesystem){
    if(filesystem.parentDir.empty())
        return false;

    filesystem.parentDir = LAS::TextManip::ensureSlash(filesystem.parentDir);

    filesystem.ammoDir         = filesystem.parentDir + "Ammo/";
    filesystem.drillsDir       = filesystem.parentDir + "Drills/";
    filesystem.eventsDir       = filesystem.parentDir + "Events/";
    filesystem.gunsDir         = filesystem.parentDir + "Guns/";

    // Check paths are good
    if(!LAS::ensureDirectory(filesystem.ammoDir)){
        log_error(std::format("Error finding or creating directory [{}]", filesystem.ammoDir));
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.drillsDir)){
        log_error("Error finding or creating directory [" + filesystem.drillsDir + "]");
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.eventsDir)){
        log_error("Error finding or creating directory [" + filesystem.eventsDir + "]");
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.gunsDir)){
        log_error("Error finding or creating directory [" + filesystem.gunsDir + "]");
        return false;
    }
    return true;
}
bool Framework::setupAmmoTracker(std::string directory){
    if(!ammoTracker)
        ammoTracker = std::make_shared<AmmoTracker>();

    if(!ammoTracker->setDirectory(directory))
        return false;
    
    if(!ammoTracker->readCartridges())
        return false;
    
    if(!ammoTracker->readManufacturers())
        return false;

    if(!ammoTracker->readAllAmmo())
        return false;

    return true;
}
bool Framework::setupGunTracker    (std::string directory){
    if(!gunTracker)
        gunTracker = std::make_shared<GunTracker>();

    if(!gunTracker->setDirectory(directory))
        return false;
    
    if(!gunTracker->readGuns())
        return false;
    
    if(!gunTracker->readWeaponTypes())
        return false;

    // Add all ammo types to AmmoTracker
    std::vector<ConstGunPtr> gunList;
    gunTracker->getAllGuns(gunList);

    for(const auto& gun : gunList){
        ConstTrackedAmmoPtrList ammoUsed;
        gun->getAllAmmoUsed(ammoUsed);

        for(const auto& trackedAmmo : ammoUsed)
            ammoTracker->addAmmoToStockpile(TrackedAmmo{trackedAmmo->ammoType, 0 });
            // 0 as amount since ammoUsed tracks rounds shot through gun and isn't what is in stockpile
    }

    return true;
}
bool Framework::setupEventTracker    (std::string directory){
    if(!eventTracker)
        eventTracker = std::make_shared<EventTracker>();

    if(!eventTracker->setDirectory(directory))
        return false;

    if(!eventTracker->readEvents())
        return false;
    
    if(!eventTracker->readEventTypes())
        return false;

    if(!eventTracker->readLocations())
        return false;
    
    return true;
}
bool Framework::setupWindow(){
    if(!window->setAmmoTracker(ammoTracker)){
        log_critical("Could not add Ammo Tracker to window");
        return false;
    }
    if(!window->setGunTracker(gunTracker)){
        log_critical("Could not add Gun Tracker to window");
        return false;
    }
    if(!window->setEventTracker(eventTracker)){
        log_critical("Could not add Event Tracker to window");
        return false;
    }

    return true;
}

