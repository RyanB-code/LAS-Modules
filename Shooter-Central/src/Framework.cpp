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
    std::cout << "  ";

    for(const auto& gunInfo : ammo.getGunsUsed() )
        printGunMetadata(gunInfo);
}
void printArmoryGun(const ArmoryGun& gun){
    printGunMetadata(gun.getGunInfo());
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
    using namespace ShooterCentral::Setup;

    Filepaths paths {directory};

    if(!setupFilesystem(paths)){
        log_critical("Failed to setup filesystem");
        return false;
    }

    shown = setShown;

    // END REAL SETUP DATA

    Cartridge testCart1 { "testCart1" };
    Cartridge testCart2 { "testCart2" };
    Manufacturer testMan1  { "testMan1" };
    Manufacturer testMan2  { "testMan2" };

    AmmoMetadata amMet1 {
        "testAmmo1",
        testCart1,
        testMan1,
        3
    };

    AmmoMetadata amMet2 {
        "testAmmo2",
        testCart1,
        testMan1,
        5
    };

    AmmoMetadata amMet3 {
        "testAmmo3",
        testCart2,
        testMan2,
        60
    };



    GunMetadata gunMet1 {
        "testGun1",
        testCart1,
        WeaponType {"testWT1"}
    };

    GunMetadata gunMet2 {
        "testGun2",
        testCart2,
        WeaponType {"testWT1"}
    };

    Database database { };

    const std::chrono::zoned_time now {std::chrono::current_zone(), std::chrono::system_clock::now( ) };
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now.get_local_time())};

    ShootingEventMetadata eventMet1 {
        .notes = "",
        .location = Location { "testLoc1" },
        .eventType = ShootingEventType { "testET1" },
        .date = ymd
    };

    GunTrackingAmmoUsed gunUsed1 {gunMet1};
    gunUsed1.addAmmoUsed( AmountOfAmmo  { amMet1, 50 } );
    gunUsed1.addAmmoUsed( AmountOfAmmo  { amMet2, 60 } );
    gunUsed1.addAmmoUsed( AmountOfAmmo  { amMet1, 3 } );
    
    GunTrackingAmmoUsed gunUsed2 {gunMet2};
    gunUsed2.addAmmoUsed( AmountOfAmmo { amMet3, 100 } );

    database.addEvent(ShootingEvent {eventMet1} );
    database.getEvent(eventMet1).addGun( gunUsed1 );
    database.getEvent(eventMet1).addGun( gunUsed2 );

    database.addToStockpile( AmountOfAmmo { amMet1, 100 } );
    database.addToStockpile( AmountOfAmmo { amMet2, 69 } );

    associateEvents(database);

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



    write(paths.eventsDir, database.getEvent(eventMet1).getInfo());



    log_info("SC Setup sucessful");
    return true;
}


/*
    // TESTING
    if(!readDir_Guns(paths.gunsDir))
        log_error("Failed reading Guns");
    if(!readDir_Ammo(paths.ammoDir))
        log_error("Failed reading Ammo");
    if(!readDir_Events(paths.eventsDir))
        log_error("Failed reading Events");
    if(!readFile_Descriptors(paths.miscDir))
        log_error("Failed reading Events");

    /*
    const std::chrono::zoned_time now {std::chrono::current_zone(), std::chrono::system_clock::now( ) };
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now.get_local_time())};

    Event testEvent1 { Location{"Test Location"}, EventType{"Test EventType"}, "no notes here fuckwad", ymd} ;

    GunAndAmmo testGAA { containers.knownGuns_cbegin()->second };
    testGAA.addAmmoUsed(AmountOfAmmo{containers.knownAmmo_cbegin()->second, 15});

    testEvent1.addGun(testGAA);
    if(!FileIO::write(paths.eventsDir, testEvent1))
        log_error("Could not write event");
    // /

    buildAssociations();

    std::cout << "Known Guns:\n";
    for(const auto& [gunMetadata, value] : containers.getKnownGuns() ) {
        std::cout << std::format("  Name: {}, WT: {}, Cart: {}\n", value->name, value->weaponType.getName(), value->cartridge.getName());
        std::cout << "    Gun Addr: " << &*value << "\n";
        std::cout << "    WT Addr: " << &value->weaponType << "\n";
        std::cout << "    Cart Addr: " << &value->cartridge << "\n";
    }
    std::cout << "\n\nKnown Ammo:\n";
    for(const auto& [ammoMetadata, value ] : containers.getKnownAmmo() ) {
        std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}\n",  ammoMetadata.name, ammoMetadata.manufacturer.getName(), ammoMetadata.cartridge.getName(), ammoMetadata.grainWeight );
        std::cout << "    AmmoAddr: " << &*value << "\n";
        std::cout << "    Man Addr: " << &value->manufacturer << "\n";
        std::cout << "    Cart Addr: " << &value->cartridge << "\n";
    }
    std::cout << "\n\nAmmo Stockpile (Assoc Ammo) :\n";
    for(const auto& [cartridge, map] : containers.getAmmoStockpile() ) {
        
        for(const auto& [ammoMetadata, assocAmmoPtr] : map){
            const AmmoMetadata& data {assocAmmoPtr->getAmountOfAmmo().getAmmoInfo() };
            std::cout << "    Ammo Addr: " << &data << "  Amount: " << assocAmmoPtr->getAmountOfAmmo().getAmount() << "\n";

            for(const auto& [gunMetadata, gunPtr] : assocAmmoPtr->getGunsUsed()){
                std::cout << "      Gun Addr: " << &gunPtr << "\n";
            }
        }
        
    }
    std::cout << "\n\nEvents:\n";
    for(const auto& [eventMetadata, eventPtr] : containers.getEvents() ) {
        const ShootingEvent& event {*eventPtr};
        const ShootingEventMetadata& info { event.getInfo() };

        std::cout << std::format("  Event: Location: {}, EventType: {}, Notes: {}, Date: {}\n", info.location.getName(), info.eventType.getName(), info.notes, event.printDate());
        std::cout << "      Event Addr: " << &event << "\n";

        for(const GunAndAmmo& gunAndAmmo : event.getGunsUsed() ){
            std::cout << "    Gun Addr: " << &gunAndAmmo.getGunInfo() << "\n";
                
            for(const auto& amountOfAmmo : gunAndAmmo.getAmmoUsedList() ){
                const AmmoMetadata& data { amountOfAmmo.getAmmoInfo() };
                std::cout << "      AmmoAddr: " << &data << "  Amount: " << amountOfAmmo.getAmount() << "\n";

            }
        }
    }


    std::cout << "\n\nAssoc Guns:\n";
    for(auto itr{containers.getGunsInArmory().cbegin()}; itr != containers.getGunsInArmory().cend(); ++itr){
        const auto& [cartridge, map] {*itr};

        for(auto mapItr {map.cbegin()}; mapItr != map.cend(); ++mapItr){
            const AssociatedGun& gun {*mapItr->second};

            std::cout << "  Gun Addr: " << &gun.getGunInfo() << "\n";
        
            for(const auto& [info, eventPtr] : gun.getEventsUsed())
                std::cout << "    Event Addr: " << &eventPtr->getInfo() << "\n";

            for(const auto& [info, amountOfAmmo] : gun.getAmmoUsed() ){
                const AmmoMetadata& data { amountOfAmmo.getAmmoInfo() };
                std::cout << "    AmmoAddr: " << &data << "\n";
                std::cout << std::format("      Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n",  data.name, data.manufacturer.getName(), data.cartridge.getName(), data.grainWeight, amountOfAmmo.getAmount());
            }
        }
    }

    std::cout << "\n\nManufacturers:\n";
    for(const auto& [manufacturer, ptr] : containers.getManufacturers() ){
        std::cout << "  Addr: " << &*ptr << "  " << manufacturer.getName() << "\n";
    }
    std::cout << "\n\nLocations:\n";
    for(const auto& [location, ptr] : containers.getLocations() ){
        std::cout << "  Addr: " << &*ptr << "  " << location.getName() << "\n";
    }
    std::cout << "\n\nEventTypes:\n";
    for(const auto& [et, ptr] : containers.getEventTypes() ){
        std::cout << "  Addr: " << &*ptr << "  " << et.getName() << "\n";
    }
    std::cout << "\n\nCartridges:\n";
    for(const auto& [c, ptr] : containers.getCartridges() ){
        std::cout << "  Addr: " << &*ptr << "  " << c.getName() << "\n";
    }
    std::cout << "\n\nWeaponTypes:\n";
    for(const auto& [wt, ptr] : containers.getWeaponTypes() ){
        std::cout << "  Addr: " << &*ptr << "  " << wt.getName() << "\n";
    }

    // DONE TESTING   
 
        return true;

}
*/
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
   
    /*
    if(!ImGui::Begin(TITLE, &*shown, 0)){
        ImGui::End();
        return;
    }

    view.draw(containers, unsavedChanges);

    ImGui::End();
    */

}
/*
bool Framework::readDir_Guns(const std::string& dir) {
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            ObjectBuffers::GunMetadata gunInfoBuffer;
            FileIO::read_GunMetadata(j, gunInfoBuffer);

            if(!containers.knownGuns_create(gunInfoBuffer).second){
                log_error("Could not add GunMetadata from file [" + dirEntry.path().string() + ']');
                continue;
            }
		}
		catch(std::exception& e){
            log_error("Failed to create GunMetadata object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::readDir_Ammo(const std::string& dir) {
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            ObjectBuffers::AmmoMetadata ammoInfoBuffer;
            FileIO::read_AmmoMetadata(j.at("ammoInfo"), ammoInfoBuffer);

            const auto& [ammoMetadataPtr, addSuccessful] { containers.knownAmmo_create(ammoInfoBuffer) };

            if(!addSuccessful){
                log_error("Could not add AmmoMetadata from file [" + dirEntry.path().string() + ']');
                continue;
            }

            const AmmoMetadata& ammoInfo { *ammoMetadataPtr };

            // Make the AmountOfAmmo object and add to stockpile ONLY if it is set to active
            if(!ammoInfoBuffer.isActive)
                continue;


            int amountBuf { 0 };
            j.at("amount").get_to(amountBuf);

            AmountOfAmmo amountOfAmmoBuffer(*ammoMetadataPtr);
            amountOfAmmoBuffer.addAmount(amountBuf);

            if(!containers.ammoStockpile_add(amountOfAmmoBuffer)) { 
                log_error("Could not add AssociatedAmmo to stockpile from file [" + dirEntry.path().string() + ']');
                continue;
            }
		}
		catch(std::exception& e){
            log_error("Failed to create Ammo objects from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::readDir_Events(const std::string& dir) {
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            ObjectBuffers::ShootingEventMetadata eventInfoBuffer;
            FileIO::read_EventMetadata(j.at("eventInfo"), eventInfoBuffer);

            const auto& [eventPtr, addSuccessful] { containers.events_create(eventInfoBuffer) };
                
            if(!addSuccessful){
                log_error(std::format("Could not add Event to containers. Event file [{}]", dirEntry.path().string()));
                continue;
            }

            // Add all guns used
	        for (auto& gunElm : j.at("gunsUsed").items()) {

                ObjectBuffers::GunMetadata gunInfoBuffer;
                FileIO::read_GunMetadata(gunElm.value().at("gunInfo"), gunInfoBuffer);
                
                // Add to known Guns
                const auto& [gunMetadataPtr, addResult] {containers.knownGuns_create(gunInfoBuffer)};

                if(!addResult){
                    log_error("Could not insert GunMetadata when reading Event from file [" + dirEntry.path().string() + ']');
                    continue;
                }

                // Add all ammo used for the gun
                GunAndAmmo gunAndAmmoBuf {*gunMetadataPtr};

                for(auto& ammoElm : gunElm.value().at("ammoUsed").items()){
                    ObjectBuffers::AmmoMetadata ammoInfoBuffer;
                    FileIO::read_AmmoMetadata(ammoElm.value().at("ammoInfo"), ammoInfoBuffer);

                    const auto& [ammoMetadataPtr, addSuccessful] { containers.knownAmmo_create(ammoInfoBuffer) };

                    if(!addSuccessful){
                        log_error("Could not add AmmoMetadata when reading Event from file [" + dirEntry.path().string() + ']');
                        continue;
                    }

                    // Add amounts to the Gun
                    int amountBuf { 0 };
                    ammoElm.value().at("amount").get_to(amountBuf);

                    AmountOfAmmo amountOfAmmoBuffer{*ammoMetadataPtr};
                    amountOfAmmoBuffer.addAmount(amountBuf);

                    if(!gunAndAmmoBuf.addAmmoUsed(amountOfAmmoBuffer)) {
                        log_error(std::format("Could not add AmountOfAmmo [{}] to Gun [{}] when reading Event from file [{}]", ammoMetadataPtr->name, gunMetadataPtr->name, dirEntry.path().string()));
                        continue;
                    }
                } 

                if(!eventPtr->addGun(gunAndAmmoBuf)){
                    log_error(std::format("Could not add Gun [{}] when reading Event from file [{}]", gunMetadataPtr->name, dirEntry.path().string()));
                    continue;
                }
	        } // End of all Gun elements

		} // End of all file elements
		catch(std::exception& e){
            log_error("Failed to create Event object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::readFile_Descriptors(const std::string& dir){
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){

        if(dirEntry.path().filename().string() != FILENAME_DESCRIPTORS)
            continue;

		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            // Parse all items here
	        for (auto& elm : j.at("manufacturers").items()) {
                Manufacturer buffer { elm.value() };

                if(containers.getManufacturers().contains(buffer))
                    continue;

                if(!containers.manufacturers_add( buffer ))
                    log_error(std::format("Could not add Manufacturer [{}]", buffer.getName()));
            }

	        for (auto& elm : j.at("cartridges").items()) {
                Cartridge buffer { elm.value() };

                if(containers.getCartridges().contains(buffer))
                    continue;

                if(!containers.cartridges_add(buffer))
                    log_error(std::format("Could not add Cartridge [{}]", buffer.getName()));
            }

            for (auto& elm : j.at("locations").items()) {
                Location buffer { elm.value() };

                if(containers.getLocations().contains(buffer))
                    continue;

                if(!containers.locations_add(buffer))
                    log_error(std::format("Could not add Location [{}]", buffer.getName()));
            }

            for (auto& elm : j.at("eventTypes").items()) {
                ShootingEventType buffer { elm.value() };

                if(containers.getEventTypes().contains(buffer))
                    continue;

                if(!containers.eventTypes_add(buffer))
                    log_error(std::format("Could not add EventType [{}]", buffer.getName()));
            }

            for (auto& elm : j.at("weaponTypes").items()) {
                WeaponType buffer { elm.value() };

                if(containers.getWeaponTypes().contains(buffer))
                    continue;

                if(!containers.weaponTypes_add(buffer))
                    log_error(std::format("Could not add WeaponType [{}]", buffer.getName()));
            }
          
		} // End of all file elements
		catch(std::exception& e){
            log_error("Failed to create Event object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::writeFile_Descriptors(std::string directory) {
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;


    json manufacturersArray = json::array();
    for(const auto& [info, ptr] : containers.getManufacturers() )
        manufacturersArray.emplace_back(*ptr);

    json cartridgesArray = json::array();
    for(const auto& [info, ptr] : containers.getCartridges() )
        cartridgesArray.emplace_back(*ptr);

    json locationsArray = json::array();
    for(const auto& [info, ptr] : containers.getLocations() )
        locationsArray.emplace_back(*ptr);

    json eventTypesArray = json::array();
    for(const auto& [info, ptr] : containers.getEventTypes() )
        eventTypesArray.emplace_back(*ptr);

    json weaponTypesArray = json::array();
    for(const auto& [info, ptr] : containers.getWeaponTypes() )
        weaponTypesArray.emplace_back(*ptr);


    json j;
    j = json {
        { "manufacturers",  manufacturersArray },
        { "cartridges",     cartridgesArray },
        { "locations",      locationsArray },
        { "eventTypes",     eventTypesArray },
        { "weaponTypes",    weaponTypesArray }
    };

    // Write to file
    std::ofstream file{ directory + std::string{FILENAME_DESCRIPTORS} };
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
void Framework::buildAssociations() {
    // Go through every event
    for(const auto& [eventMetadata, eventPtr] : containers.getEvents() ){
        const ShootingEvent& event { *eventPtr };

        // Go through every GunAndAmmo in the event
        for(const GunAndAmmo& gunAndAmmo : event.getGunsUsed() ){
            const GunMetadata& gunInfo { gunAndAmmo.getGunInfo() };

            // Add AssociatedGun object
            if(!containers.gunsInArmory_contains(gunInfo)){
                if(!containers.gunsInArmory_add(AssociatedGun{*containers.knownGuns_at(gunInfo)})){ 
                    log_error("Could not add AssociatedGun to armory when associating Event");
                    continue;
                }
            }

            // Add Event to AssociatedGun object
            AssociatedGun& assocGun { *containers.gunsInArmory_at(gunInfo) };
            assocGun.addEvent(eventPtr);
 
            // Go through every Ammo used for the Gun
            for(const auto& amountOfAmmo : gunAndAmmo.getAmmoUsedList() ){
                const AmmoMetadata& ammoInfo { amountOfAmmo.getAmmoInfo()};

                if(!ammoInfo.isActive)
                    continue;

                if(!containers.ammoStockpile_contains(ammoInfo)){

                    // Do not add ammo amount since the amount here is for what was used during the Event
                    AmountOfAmmo amountBuf {*containers.knownAmmo_at(ammoInfo), 0};
                    AssociatedAmmo assocAmmoBuffer {amountBuf};

                    if(!containers.ammoStockpile_add(assocAmmoBuffer)) {
                        log_error("Could not add AssociatedAmmo to stockpile when associating Event");
                        continue;
                    }
                }

                // Add Gun to AssociatedAmmo object
                AssociatedAmmo& assocAmmo { *containers.ammoStockpile_at(ammoInfo) };
                assocAmmo.addGun(containers.knownGuns_at(gunInfo));

            } // End every AmountOfAmmo for the gun
        } // End every GunAndAmmo in the event
    } // End every event
}
*/
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

 
/*
namespace ShooterCentral::FileIO {


void read_GunMetadata   (const json& j, ObjectBuffers::GunMetadata& buffer){
    using LAS::json;

    j.at("name").get_to(buffer.name);
    j.at("weaponType").get_to(buffer.weaponType);
    j.at("cartridge").get_to(buffer.cartridge);
    j.at("isActive").get_to(buffer.isActive);
}
void read_AmmoMetadata  (const json& j, ObjectBuffers::AmmoMetadata& buffer){
    j.at("name").get_to(buffer.name);
    j.at("manufacturer").get_to(buffer.manufacturer);
    j.at("cartridge").get_to(buffer.cartridge);
    j.at("grain").get_to(buffer.grainWeight);
    j.at("isActive").get_to(buffer.isActive);
}
void read_EventMetadata (const json& j, ObjectBuffers::ShootingEventMetadata& buffer){
    j.at("date").get_to(buffer.date);
    j.at("eventType").get_to(buffer.eventType);
    j.at("location").get_to(buffer.location);
    j.at("notes").get_to(buffer.notes);
}

} // End FileIO namespace


Popup_CommandFailed::Popup_CommandFailed(const char* set) : Popup("Event Failed") {
    strncpy(text, set, sizeof(text) - 1);
    text[sizeof(text) - 1] = '\0';            // Manually null-terminate
}
void Popup_CommandFailed::show() {
    UI::centerText(text);

    buttons_Close();
}
*/
