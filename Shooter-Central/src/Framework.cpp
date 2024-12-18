#include "Framework.h"

using namespace ShooterCentral;
using namespace LAS::Logging;

Framework::Framework() {
    window = std::make_shared<ShooterCentralWindow>(ShooterCentralWindow{});
}
Framework::~Framework(){

}

// MARK: Public Functions
bool Framework::setup(LAS::Logging::LoggerPtr setLoggerPtr, const std::string& directory){
    if(!setLogger(setLoggerPtr))
        return false;

    // -------------------------------
    // USE LOGGING ONCE LOGGER IS VERIFIED
    // -------------------------------

    Filesystem filesystem {directory};

    if(!setupFilesystem(filesystem)){
        logger->log("Failed to setup filesystem", Tags{"FATAL", "SC"});
        return false;
    }
    if(!setupAmmoTracker(filesystem.ammoDir)){
        logger->log("Failed to setup Ammo Tracker", Tags{"FATAL", "SC"});
        return false;
    }
    if(!setupGunTracker(filesystem.gunsDir)){
        logger->log("Failed to setup Gun Tracker", Tags{"FATAL", "SC"});
        return false;
    }
    if(!setupEventTracker(filesystem.eventsDir)){
        logger->log("Failed to setup Event Tracker", Tags{"FATAL", "SC"});
        return false;
    }
    if(!setupWindow()){
        logger->log("Failed to setup window", Tags{"FATAL", "SC"});
        return false;
    }

   
    logger->log("Setup sucessful", Tags{"ROUTINE", "SC"});
    return true;
}
SCWindowPtr Framework::getWindow() const {
    return window;
}
std::string Framework::getCommandGroupName() const {
    return commandGroupName;
}
bool Framework::setLogger(LAS::Logging::LoggerPtr setLogger){
    if(!setLogger)
        return false;
    
    logger = setLogger;
    return true;
}
LAS::Logging::LoggerPtr Framework::getLogger() const {
    return logger;
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
        logger->log("Error finding or creating directory [" + filesystem.ammoDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.drillsDir)){
        logger->log("Error finding or creating directory [" + filesystem.drillsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.eventsDir)){
        logger->log("Error finding or creating directory [" + filesystem.eventsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    if(!LAS::ensureDirectory(filesystem.gunsDir)){
        logger->log("Error finding or creating directory [" + filesystem.gunsDir + "]", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
    return true;
}
bool Framework::setupAmmoTracker(std::string directory){
    if(!ammoTracker)
        ammoTracker = std::make_shared<AmmoTracker>(logger);

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
        gunTracker = std::make_shared<GunTracker>(logger);

    if(!gunTracker->setDirectory(directory))
        return false;
    
    if(!gunTracker->readGuns())
        return false;
    
    if(!gunTracker->readWeaponTypes())
        return false;

    // Add gun cartridges to ammo tracker
    std::vector<GunPtr> gunList;
    gunTracker->getAllGuns(gunList);

    StringVector cartridges;
    for(const auto& gun : gunList){
        ammoTracker->addCartridge(gun->getCartridge());
    }

    return true;
}
bool Framework::setupEventTracker    (std::string directory){
    if(!eventTracker)
        eventTracker = std::make_shared<EventTracker>(logger);

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
        logger->log("Could not add Ammo Tracker to window", LAS::Logging::Tags{"FATAL", "SC"});
        return false;
    }
    if(!window->setGunTracker(gunTracker)){
        logger->log("Could not add Gun Tracker to window", LAS::Logging::Tags{"FATAL", "SC"});
        return false;
    }
    if(!window->setEventTracker(eventTracker)){
        logger->log("Could not add Event Tracker to window", LAS::Logging::Tags{"FATAL", "SC"});
        return false;
    }

    return true;
}

