#include "Framework.h"

using namespace ShooterCentral;
using namespace LAS;

Framework::Framework() {

}
Framework::~Framework() {

}
bool Framework::setup(const std::string& directory, std::shared_ptr<bool> setShown){
    using namespace ShooterCentral::Setup;

    Filepaths paths {directory};

    if(!setupFilesystem(paths)){
        log_critical("Failed to setup filesystem");
        return false;
    }

    shown = setShown;

    // TESTING
    if(!readGuns(paths.gunsDir))
        log_error("Failed reading Guns");
    if(!readAmmo(paths.ammoDir))
        log_error("Failed reading Ammo");
    if(!readEvents(paths.eventsDir))
        log_error("Failed reading Events");
    if(!readDescriptors(paths.miscDir))
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
    */

    buildAssociations();
    addItemDescriptors();


    std::cout << "Known Guns:\n";
    for(const auto& [gunMetadata, value] : containers.getKnownGuns() ) {
        std::cout << std::format("  Name: {}, WT: {}, Cart: {}\n", value->name, value->weaponType.getName(), value->cartridge.getName());
        std::cout << "    Gun Addr: " << &*value << "\n";
    }
    std::cout << "\n\nKnown Ammo:\n";
    for(const auto& [data, value ] : containers.getKnownAmmo() ) {
        std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}\n",  data.name, data.manufacturer.getName(), data.cartridge.getName(), data.grainWeight );
        std::cout << "    AmmoAddr: " << &*value << "\n";
    }
    std::cout << "\n\nAmmo Stockpile (Assoc Ammo) :\n";
    for(const auto& [cartridge, map] : containers.getAmmoStockpile() ) {
        
        for(auto mapItr {map.cbegin()}; mapItr != map.cend(); ++mapItr){
            const auto& [key, value] { *mapItr };

            const AmmoMetadata& data {value->getAmountOfAmmo().getAmmo() };
            std::cout << "    Ammo Addr: " << &value->getAmountOfAmmo().getAmmo() << "  Amount: " << value->getAmountOfAmmo().getAmount() << "\n";

            for(auto itr2 {value->cbegin() }; itr2 != value->cend(); ++itr2){
                std::cout << "      Gun Addr: " << &*itr2->second << "\n";
            }
        }
        
    }
    std::cout << "\n\nEvents:\n";
    for(const auto& [evntMetadata, eventPtr] :containers.getEvents() ) {
        const Event& event {*eventPtr};

        std::cout << std::format("  Event: Location: {}, EventType: {}, Notes: {}, Date: {}\n", event.getLocation().getName(), event.getEventType().getName(), event.getNotes(), event.printDate());
        std::cout << "      Event Addr: " << &event << "\n";

        for(auto itrGun { event.cbegin() }; itrGun != event.cend(); ++itrGun){
            std::cout << "    Gun Addr: " << &itrGun->getGun() << "\n";
                
            for(auto itrAmmo { itrGun->cbegin() }; itrAmmo != itrGun->cend(); ++itrAmmo){
                const AmmoMetadata& data { itrAmmo->getAmmo() };
                std::cout << "      AmmoAddr: " << &data << "  Amount: " << itrAmmo->getAmount() << "\n";

            }
        }
    }


    std::cout << "\n\nAssoc Guns:\n";
    for(auto itr{containers.getGunsInArmory().cbegin()}; itr != containers.getGunsInArmory().cend(); ++itr){
        const auto& [cartridge, map] {*itr};

        for(auto mapItr {map.cbegin()}; mapItr != map.cend(); ++mapItr){
            const AssociatedGun& gun {*mapItr->second};

            std::cout << "  Gun Addr: " << &gun.getGun() << "\n";
        
            for(const auto& [info, eventPtr] : gun.getEventsUsed())
                std::cout << "    Event Addr: " << &eventPtr->getInfo() << "\n";

            for(const auto& [info, amountOfAmmo] : gun.getAmmoUsed() ){
                const AmmoMetadata& data { amountOfAmmo.getAmmo() };
                std::cout << "    AmmoAddr: " << &data << "\n";
                std::cout << std::format("      Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n",  data.name, data.manufacturer.getName(), data.cartridge.getName(), data.grainWeight, amountOfAmmo.getAmount());
            }
        }
    }

    std::cout << "\n\nManufacturers:\n";
    for(auto itr{containers.getManufacturers().cbegin()}; itr != containers.getManufacturers().cend(); ++itr){
        std::cout << "  " << itr->getName() << "\n";
    }

    std::cout << "\n\nLocations:\n";
    for(auto itr{containers.getLocations().cbegin()}; itr != containers.getLocations().cend(); ++itr){
        std::cout << "  " << itr->getName() << "\n";
    }

    std::cout << "\n\nEventTypes:\n";
    for(auto itr{containers.getEventTypes().cbegin()}; itr != containers.getEventTypes().cend(); ++itr){
        std::cout << "  " << itr->getName() << "\n";
    }

    std::cout << "\n\nCartridges:\n";
    for(auto itr{containers.getCartridges().cbegin()}; itr != containers.getCartridges().cend(); ++itr){
        std::cout << "  " << itr->getName() << "\n";
    }

    std::cout << "\n\nWeaponTypes:\n";
    for(auto itr{containers.getWeaponTypes().cbegin()}; itr != containers.getWeaponTypes().cend(); ++itr){
        std::cout << "  " << itr->getName() << "\n";
    }

    // DONE TESTING
   
    log_info("SC Setup sucessful");
    return true;
}
void Framework::update() {

}
void Framework::draw() {
   
    if(!ImGui::Begin(TITLE, &*shown, 0)){
        ImGui::End();
        return;
    }

    view.draw(containers, unsavedChanges);

    ImGui::End();
}
bool Framework::readGuns(const std::string& dir) {
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            GunMetadata gunBuf {j.template get<ShooterCentral::GunMetadata>()};

            if(!containers.getKnownGuns().contains(gunBuf)){
                if(!containers.knownGuns_add(std::make_shared<GunMetadata>(gunBuf))){
                    log_error("Could not insert GunMetadata from file [" + dirEntry.path().string() + ']');
                    continue;
                }
            }            
		}
		catch(std::exception& e){
            log_error("Failed to create GunMetadata object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::readAmmo(const std::string& dir) {
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            AmmoMetadata ammoInfo {j.at("ammoInfo").template get<ShooterCentral::AmmoMetadata>()};

            if(!containers.getKnownAmmo().contains(ammoInfo)){
                if(!containers.knownAmmo_add(std::make_shared<AmmoMetadata>(ammoInfo))){
                    log_error("Could not insert AmmoMetadata from file [" + dirEntry.path().string() + ']');
                    continue;
                }
            }

            // Make the AmountOfAmmo object and add to stockpile ONLY if it is set to active
            if(!ammoInfo.isActive)
                continue;

            int amountBuf { 0 };
            j.at("amount").get_to(amountBuf);
            if(!containers.ammoStockpile_contains(ammoInfo)){
                if(!containers.ammoStockpile_add(
                                std::make_shared<AssociatedAmmo>(AmountOfAmmo{containers.knownAmmo_at(ammoInfo), amountBuf}) ) )
                { 
                    log_error("Could not add AssociatedAmmo to stockpile from file [" + dirEntry.path().string() + ']');
                    continue;
                }
            }
		}
		catch(std::exception& e){
            log_error("Failed to create Ammo objects from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::readEvents(const std::string& dir) {
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            Event eventBuf { EventMetadata {j.at("eventInfo").template get<ShooterCentral::EventMetadata>() } };

            // Go over all guns used
	        for (auto& gunElm : j.at("gunsUsed").items()) {
                GunMetadata gunInfoBuf  {gunElm.value().at("gunInfo").template get<ShooterCentral::GunMetadata>() };
                
                // Add to known Guns
                if(!containers.getKnownGuns().contains(gunInfoBuf)){
                    if(!containers.knownGuns_add(std::make_shared<GunMetadata>(gunInfoBuf))){
                        log_error("Could not insert GunMetadata when reading Event from file [" + dirEntry.path().string() + ']');
                        continue;
                    }
                }

                // Get all ammo used for the gun
                GunAndAmmo gunAndAmmoBuf {containers.knownGuns_at(gunInfoBuf)};
                for(auto& ammoElm : gunElm.value().at("ammoUsed").items()){
                    AmmoMetadata infoBuf {ammoElm.value().at("ammoInfo").template get<ShooterCentral::AmmoMetadata>() };

                    if(!containers.getKnownAmmo().contains(infoBuf)){
                        if(!containers.knownAmmo_add(std::make_shared<AmmoMetadata>(infoBuf))){
                            log_error("Could not insert AmmoMetadata when reading Event from file [" + dirEntry.path().string() + ']');
                            continue;
                        }
                    }

                    int amountBuf { 0 };
                    ammoElm.value().at("amount").get_to(amountBuf);

                    if(!gunAndAmmoBuf.addAmmoUsed(AmountOfAmmo{containers.knownAmmo_at(infoBuf), amountBuf})) {
                        log_error(std::format("Could not add AmountOfAmmo [{}] to Gun [{}] when reading Event from file [{}]", infoBuf.name, gunInfoBuf.name, dirEntry.path().string()));
                        continue;
                    }
                } // End of all Ammo elements

                if(!eventBuf.addGun(gunAndAmmoBuf)){
                    log_error(std::format("Could not add Gun [{}] when reading Event from file [{}]", gunInfoBuf.name, dirEntry.path().string()));
                    continue;
                }
	        } // End of all Gun elements

            if(!containers.events_add(std::make_shared<Event>(eventBuf))){
                log_error(std::format("Could not add Event to containers. Event file [{}]", dirEntry.path().string()));
                continue;
            }
		} // End of all file elements
		catch(std::exception& e){
            log_error("Failed to create Event object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::readDescriptors(const std::string& dir){
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
                std::string nameBuf { elm.value() };
                if(!containers.manufacturers_add( Manufacturer{nameBuf} ))
                    log_error(std::format("Could not add Manufacturer [{}]", nameBuf));
            }

	        for (auto& elm : j.at("cartridges").items()) {
                std::string nameBuf { elm.value() };
                if(!containers.cartridges_add( Cartridge {nameBuf} ))
                    log_error(std::format("Could not add Cartridge [{}]", nameBuf));
            }

            for (auto& elm : j.at("locations").items()) {
                std::string nameBuf { elm.value() };
                if(!containers.locations_add( Location {nameBuf} ))
                    log_error(std::format("Could not add Location [{}]", nameBuf));
            }

            for (auto& elm : j.at("eventTypes").items()) {
                std::string nameBuf { elm.value() };
                if(!containers.eventTypes_add( EventType {nameBuf} ))
                    log_error(std::format("Could not add EventType [{}]", nameBuf));
            }

            for (auto& elm : j.at("weaponTypes").items()) {
                std::string nameBuf { elm.value() };
                if(!containers.weaponTypes_add( WeaponType {nameBuf} ))
                    log_error(std::format("Could not add WeaponType [{}]", nameBuf));
            }
          
		} // End of all file elements
		catch(std::exception& e){
            log_error("Failed to create Event object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
		}
	}
    
	return true;
}
bool Framework::writeDescriptors(std::string directory) {
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;


    json manufacturersArray = json::array();
    for(const auto& man : containers.getManufacturers() )
        manufacturersArray.emplace_back(man);

    json cartridgesArray = json::array();
    for(const auto cart : containers.getCartridges() )
        cartridgesArray.emplace_back(cart);

    json locationsArray = json::array();
    for(const auto loc : containers.getLocations() )
        locationsArray.emplace_back(loc);

    json eventTypesArray = json::array();
    for(const auto& et : containers.getEventTypes() )
        eventTypesArray.emplace_back(et);

    json weaponTypesArray = json::array();
    for(const auto& wt : containers.getWeaponTypes() )
        weaponTypesArray.emplace_back(wt);


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
        const Event& event { *eventPtr };

        // Go through every GunAndAmmo in the event
        for(auto gunAndAmmoItr { event.cbegin() }; gunAndAmmoItr != event.cend(); ++gunAndAmmoItr){
            const GunMetadata& gunInfo { gunAndAmmoItr->getGun() };

            // Add AssociatedGun object
            if(!containers.gunsInArmory_contains(gunInfo)){
                if(!containers.gunsInArmory_add(std::make_shared<AssociatedGun>(containers.knownGuns_at(gunInfo)))){ 
                    log_error("Could not add AssociatedGun to armory when associating Event");
                    continue;
                }
            }

            // Add Event to AssociatedGun object
            AssociatedGun& assocGun { *containers.gunsInArmory_at(gunInfo) };
            assocGun.addEvent(eventPtr);
 
            // Go through every Ammo used for the Gun
            for(auto amountOfAmmoItr { gunAndAmmoItr->cbegin() }; amountOfAmmoItr != gunAndAmmoItr->cend(); ++amountOfAmmoItr){
                const AmmoMetadata& ammoInfo { amountOfAmmoItr->getAmmo()};

                // Add AssociatedAmmo object ONLY IF it is set to active
                if(!ammoInfo.isActive)
                    continue;

                if(!containers.ammoStockpile_contains(ammoInfo)){
                    if(!containers.ammoStockpile_add(
                                std::make_shared<AssociatedAmmo>(AssociatedAmmo{AmountOfAmmo{containers.knownAmmo_at(ammoInfo), 0} }))) // Do not add ammo amount since the amount here is for what was used during the Event
                    { 
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
void Framework::addItemDescriptors() {

    for(const auto& [eventMetadata, eventPtr] : containers.getEvents() ){
        if(!containers.getLocations().contains(eventMetadata.location))
            containers.locations_add(eventMetadata.location);

        if(!containers.getEventTypes().contains(eventMetadata.eventType))
            containers.eventTypes_add(eventMetadata.eventType);
    }

    for(const auto& [gunMetadata, gunPtr ] : containers.getKnownGuns() ){
        if(!containers.getWeaponTypes().contains(gunMetadata.weaponType))
            containers.weaponTypes_add(gunMetadata.weaponType);
    }

    for(const auto& [ammoMetadata, ammoPtr] : containers.getKnownAmmo() ) {
        if(!containers.getManufacturers().contains(ammoMetadata.manufacturer))
            containers.manufacturers_add(ammoMetadata.manufacturer);

        if(!containers.getCartridges().contains(ammoMetadata.cartridge))
            containers.cartridges_add(ammoMetadata.cartridge);
    }
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
 

namespace ShooterCentral::FileIO {

std::string makeFileName    (std::string directory, const GunMetadata& gun) {
    std::ostringstream fileName;
    fileName << directory;

    for(const auto& c : gun.weaponType.getName()){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : gun.cartridge.getName()){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : gun.name){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << ".json";

    return fileName.str();
}
std::string makeFileName (std::string directory, const AmmoMetadata& ammo) {
    std::ostringstream fileName;
    fileName << directory;

    for(const auto& c : ammo.manufacturer.getName()){     // Remove spaces and make lowercase
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : ammo.cartridge.getName()){     // Remove spaces and make lowercase
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : ammo.name){     // Remove spaces and make lowercase
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_' << ammo.grainWeight;

    fileName << ".json";

    return fileName.str();
}
std::string makeFileName(std::string directory, const Event& event){
    // Output fileName
    std::ostringstream fileName;
    fileName << directory << std::format("{:%Y-%m-%d}", event.getDate()) << '_';

    for(const auto& c : event.getEventType().getName()){     // Remove spaces, make sure alnum
        if(c == ' ' || c == '\t')
            fileName << '-';
        else if(isalnum(c))
            fileName << c;
    }

    fileName << '_';

    for(const auto& c : event.getLocation().getName()){     // Remove spaces, make sure alnum
        if(c == ' ' || c == '\t')
            fileName << '-';
        else if(isalnum(c))
            fileName << c;
    }

    fileName << ".json";

    return fileName.str();
}
bool write(std::string directory, const GunMetadata& data) {
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;

    json j = data;

    // Write to file
    std::ofstream file{ makeFileName(directory, data) };
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
bool write(std::string directory, const AmountOfAmmo& data) {
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;

    LAS::json j = data;

    // Write to file
    std::ofstream file{ makeFileName(directory, data.getAmmo()) };
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
bool write (std::string directory, const Event& data){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;
    
    json eventJson;
	json gunsArray = json::array();

    // Write every gun and ammo used
    for(auto itr {data.cbegin()}; itr != data.cend(); ++itr){

        // Add ammo used to gun
        json ammoUsed = json::array();
        for(auto itr2 { itr->cbegin() }; itr2 != itr->cend(); ++itr2)
            ammoUsed.emplace_back(*itr2);
        
        // Make json for the gun
        json buffer;
        buffer = json {
            { "gunInfo",  itr->getGun() },
            { "ammoUsed", ammoUsed }
        };
        
        gunsArray.emplace_back(buffer);
    }
    
    std::ostringstream timeBuf;
    timeBuf << std::chrono::system_clock::time_point{std::chrono::sys_days{data.getDate()}};
    
    // Make JSON
    eventJson = json{
        { "eventInfo",  data.getInfo() },
        { "gunsUsed",   gunsArray      }
    };

    std::ofstream file{makeFileName(directory, data)};
    file << std::setw(1) << std::setfill('\t') << eventJson;
    file.close();
   
    return true;

}

} // End FileIO namespace


