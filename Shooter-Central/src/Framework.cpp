#include "Framework.h"

using namespace ShooterCentral;
using namespace LAS;

Framework::Framework() {

}
Framework::~Framework() {

}

// FOR TESTING
void printAmmoMetadata(const AmmoMetadata& info) {
    std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}\n",  info.name, info.manufacturer.getName(), info.cartridge.getName(), info.grainWeight );
}
void printGunMetadata(const GunMetadata& info) {
    std::cout << std::format("  Name: {}, WT: {}, Cart: {}\n", info.name, info.weaponType.getName(), info.cartridge.getName());
}
void printEventMetadata(const ShootingEventMetadata& info){
    std::cout << std::format("  Event: Location: {}, EventType: {}, Notes: {}, Date: {}\n", info.location.getName(), info.eventType.getName(), info.notes, std::format("{:%Od %b %Y}", info.date) );
}
void printStockpileAmmo(const StockpileAmmo& ammo){
    printAmmoMetadata(ammo.getAmmoInfo());
    std::cout << "  Amount: " << ammo.getAmountOnHand() << "\n";
    std::cout << "    isActive: " << std::boolalpha << ammo.isActive() << "\n";

    std::cout << "  ";

    for(const auto& gunInfo : ammo.getGunsUsed() )
        printGunMetadata(gunInfo);
}
void printArmoryGun(const ArmoryGun& gun){
    printGunMetadata(gun.getGunInfo());
    std::cout << "    isActive: " << std::boolalpha << gun.isActive() << "\n";
    std::cout << "    roundCount: " << gun.getRoundCount() << "\n";

    for(const auto& eventInfo : gun.getEventsUsed() ){
        std::cout << "  ";
        printEventMetadata(eventInfo);
    }
    std::cout << "   -\n";
    for(const auto& [info, amountOfAmmo] : gun.getAmmoUsed()) {
        std::cout << "  ";
        printAmmoMetadata(amountOfAmmo.getAmmoInfo());
        std::cout << "    Amount: " << amountOfAmmo.getAmount() << '\n';
    }
}
void printAmountOfAmmo(const AmountOfAmmo& ammo){
    printAmmoMetadata(ammo.getAmmoInfo());
    std::cout << "  Amount: " << ammo.getAmount() << "\n";
}
void printEvent(const ShootingEvent& event) {
    const auto& info { event.getInfo() };
    printEventMetadata(info); 

    for(const auto& gunTrackingAmmo : event.getGunsUsed() ){
        std::cout << "  ";
        printGunMetadata(gunTrackingAmmo.getGunInfo());
        for(const auto& amountOfAmmo : gunTrackingAmmo.getAmmoUsed()) {
            std::cout << "    ";
            printAmmoMetadata(amountOfAmmo.getAmmoInfo());
            std::cout << "      Amount: " << amountOfAmmo.getAmount() << '\n';
        }
    }
}


bool Framework::setup(const std::string& directory, std::shared_ptr<bool> setShown){
    // REAL SETUP DATA
    shown = setShown;

    using namespace ShooterCentral::Setup;

    Filepaths paths {directory};

    if(!setupFilesystem(paths)){
        log_critical("Failed to setup filesystem");
        return false;
    }


    if(!readGuns(database, std::filesystem::path(paths.gunsDir)))
        log_error("Failed reading Guns");
    if(!readAmmo(database, std::filesystem::path(paths.ammoDir)))
        log_error("Failed reading Ammo");
    if(!readEvents(database, std::filesystem::path(paths.eventsDir)))
        log_error("Failed reading Events");

    addAllMetadataInfo(database);
    associateEvents(database);

    log_info("SC Setup sucessful");
    // END REAL SETUP DATA


    // Below is for testing display
    std::cout << "\nEvents: \n";
    for(const auto& [info, e] : database.getEvents())
        printEvent(e);

    std::cout << "\nStockpile: \n";
    for(const auto& [key, cartMap] : database.getStockpile()){
        for(const auto& [info, stockpileAmmo] : cartMap)
            printStockpileAmmo(stockpileAmmo);
    }

    std::cout << "\nStockpile by cartridge: \n";
    for(const auto& c : database.getCartridges() ) {
        try {
            for(const auto& [info, stockpileAmmo] : database.getStockpile(c) )
                printAmountOfAmmo(stockpileAmmo.getAmountOfAmmo());
        }
        catch(...){
            continue;
        }  
    }

    std::cout << "\nArmory: \n";
    for(const auto& [key, cartMap] : database.getArmory()){
        for(const auto& [info, armoryGun] : cartMap)
            printArmoryGun(armoryGun);
    }

    std::cout << "\nArmory by cartridge: \n";
    for(const auto& c : database.getCartridges() ) {
        try { 
            for(const auto& [info, gun] : database.getArmory(c) )
                printGunMetadata(gun.getGunInfo());
        }
        catch(...){
            continue;
        }
    }


    std::cout << "\n\nManufacturers:\n";
    for(const auto& manufacturer : database.getManufacturers() ){
        std::cout << "  " << manufacturer.getName() << "\n";
    }
    std::cout << "\n\nLocations:\n";
    for(const auto& location : database.getLocations() ){
        std::cout << "  " << location.getName() << "\n";
    }
    std::cout << "\n\nEventTypes:\n";
    for(const auto& et : database.getEventTypes() ){
        std::cout << "  " << et.getName() << "\n";
    }
    std::cout << "\n\nCartridges:\n";
    for(const auto& c : database.getCartridges() ){
        std::cout << "  " << c.getName() << "\n";
    }
    std::cout << "\n\nWeaponTypes:\n";
    for(const auto& wt : database.getWeaponTypes() ){
        std::cout << "  " << wt.getName() << "\n";
    }


    return true;
}
void Framework::update() {
    /*
    std::unique_ptr<ModelEvent> modelEvent;
    std::unique_ptr<UIEvent> uiEvent;
    pollEvent(modelEvent);
    pollEvent(uiEvent);

    // Must check bc if no event is queued, pollEvent returns nullptr
    if(modelEvent){
        Status s {modelEvent->execute(containers)};

        if (!s.didSucceed){
            Popup_CommandFailed p {s.msg};
            UI::UIEvents::ShowPopup e {std::make_shared<Popup_CommandFailed>(p)};
            pushEvent(&e);
        }
    }
    if(uiEvent){
        Status s {uiEvent->execute(view)};

        // In future have it open a pop-up msg in the UI too
        if (!s.didSucceed)
            log_error(std::string{s.msg});
    }
    */

}
void Framework::draw() {
    if(!ImGui::Begin(TITLE, &*shown, 0)){
        ImGui::End();
        return;
    }

    view.draw(database, unsavedChanges);

    ImGui::End();
}
bool Setup::setupFilesystem(Framework::Filepaths& paths){
    if(paths.parentDir.empty())
        return false;

    paths.parentDir = LAS::TextManip::ensureSlash(paths.parentDir);

    paths.ammoDir         = paths.parentDir + "Ammo/";
    paths.eventsDir       = paths.parentDir + "Events/";
    paths.gunsDir         = paths.parentDir + "Guns/";
    paths.miscDir         = paths.parentDir + "Misc/";

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
    if(!LAS::ensureDirectory(paths.miscDir)){
        log_error(std::format("Error finding or creating directory [{}]", paths.miscDir));
        return false;
    }
    return true;
}
