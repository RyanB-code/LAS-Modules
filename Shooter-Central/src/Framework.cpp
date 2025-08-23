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
    */

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
        const Event& event {*eventPtr};
        const EventMetadata& info { event.getInfo() };

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

            ObjectBuffers::EventMetadata eventInfoBuffer;
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
                EventType buffer { elm.value() };

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
        const Event& event { *eventPtr };

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
std::string makeFileName(std::string directory, const EventMetadata& event){
    // Output fileName
    std::ostringstream fileName;
    fileName << directory << std::format("{:%Y-%m-%d}", event.date) << '_';

    for(const auto& c : event.eventType.getName()){     // Remove spaces, make sure alnum
        if(c == ' ' || c == '\t')
            fileName << '-';
        else if(isalnum(c))
            fileName << c;
    }

    fileName << '_';

    for(const auto& c : event.location.getName()){     // Remove spaces, make sure alnum
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
    std::ofstream file{ makeFileName(directory, data.getAmmoInfo()) };
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
    for(const GunAndAmmo& gunUsed : data.getGunsUsed()){

        // Add ammo used to gun
        json ammoUsed = json::array();
        for(const auto& amountOfAmmo : gunUsed.getAmmoUsedList())
            ammoUsed.emplace_back(amountOfAmmo);
        
        // Make json for the gun
        json buffer;
        buffer = json {
            { "gunInfo",  gunUsed.getGunInfo() },
            { "ammoUsed", ammoUsed }
        };
        
        gunsArray.emplace_back(buffer);
    }
    
    std::ostringstream timeBuf;
    timeBuf << std::chrono::system_clock::time_point{std::chrono::sys_days{data.getInfo().date}};
    
    // Make JSON
    eventJson = json{
        { "eventInfo",  data.getInfo() },
        { "gunsUsed",   gunsArray      }
    };

    std::ofstream file{makeFileName(directory, data.getInfo())};
    file << std::setw(1) << std::setfill('\t') << eventJson;
    file.close();
   
    return true;

}
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
void read_EventMetadata (const json& j, ObjectBuffers::EventMetadata& buffer){
    j.at("date").get_to(buffer.date);
    j.at("eventType").get_to(buffer.eventType);
    j.at("location").get_to(buffer.location);
    j.at("notes").get_to(buffer.notes);
}

} // End FileIO namespace


