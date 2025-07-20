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

    // TESTING
    if(!readGuns(paths.gunsDir))
        log_error("Failed reading Guns");
    if(!readAmmo(paths.ammoDir))
        log_error("Failed reading Ammo");
    if(!readEvents(paths.eventsDir))
        log_error("Failed reading Events");


    /*
    const std::chrono::zoned_time now {std::chrono::current_zone(), std::chrono::system_clock::now( ) };
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now.get_local_time())};

    Event testEvent1 { Location{"Test Location"}, EventType{"Test EventType"}, "no notes here fuckwad", ymd} ;

    GunAndAmmo testGAA { model.knownGuns_cbegin()->second };
    testGAA.addAmmoUsed(AmountOfAmmo{model.knownAmmo_cbegin()->second, 15});

    testEvent1.addGun(testGAA);
    if(!FileIO::write(paths.eventsDir, testEvent1))
        log_error("Could not write event");
    */

 

    std::cout << "Guns:\n";
    for(auto itr {model.knownGuns_cbegin()}; itr != model.knownGuns_cend(); ++itr) {
        const auto& [key, value] {*itr};
        std::cout << std::format("  Name: {}, WT: {}, Cart: {}\n", value->name, value->weaponType.getName(), value->cartridge.getName());
        std::cout << "    Gun Addr: " << &*value << "\n";
    }
    std::cout << "\n\nAmmo:\n";
    for(auto itr {model.ammoStockpile_cbegin()}; itr != model.ammoStockpile_cend(); ++itr) {
        const auto& [key, value] {*itr};
        const AmmoMetadata& data {value.getAmountOfAmmo().getAmmo() };
        std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n",  data.name, data.manufacturer.getName(), data.cartridge.getName(), data.grainWeight, value.getAmountOfAmmo().getAmount());
        std::cout << "    Ammo Addr: " << &value.getAmountOfAmmo().getAmmo() << "\n";
    }
    std::cout << "\n\nEvents:\n";
    for(auto itr{model.events_cbegin()}; itr != model.events_cend(); ++itr){
        const Event& event {*itr->second};
        std::cout << std::format("  Event: Location: {}, EventType: {}, Notes: {}, Date: {}\n", event.getLocation().getName(), event.getEventType().getName(), event.getNotes(), event.printDate());

        for(auto itrGun { event.cbegin() }; itrGun != event.cend(); ++itrGun){
            std::cout << "    Gun Addr: " << &itrGun->getGun() << "\n";
                
            for(auto itrAmmo { itrGun->cbegin() }; itrAmmo != itrGun->cend(); ++itrAmmo){
                std::cout << "      AmmoAddr: " << &itrAmmo->getAmmo() << "\n";
            }
        }
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
bool Framework::readGuns(const std::string& dir) {
    if(!std::filesystem::exists(dir))
        return false;

    const std::filesystem::path workingDirectory{dir};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
            std::ifstream inputFile{ dirEntry.path(), std::ios::in };
            json j = json::parse(inputFile);

            GunMetadata gunBuf {j.template get<ShooterCentral::GunMetadata>()};

            if(!model.knownGuns_add(std::make_shared<GunMetadata>(gunBuf))){
                log_error("Could not insert GunMetadata from file [" + dirEntry.path().string() + ']');
                continue;
            }

            if(!model.gunsInArmory_add(AssociatedGun{model.knownGuns_at(gunBuf)})){ 
                log_error("Could not add AssociatedGun to armory from file [" + dirEntry.path().string() + ']');
                continue;
            }

		}
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != FILENAME_WEAPON_TYPES){  // Ignore the weaponTypes file
                log_error("Failed to create GunMetadata object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
            }
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

            if(!model.knownAmmo_add(std::make_shared<AmmoMetadata>(ammoInfo))){
                log_error("Could not insert AmmoMetadata from file [" + dirEntry.path().string() + ']');
                continue;
            }

            // Make the AmountOfAmmo object and add to stockpile
            int amountBuf { 0 };
            j.at("amount").get_to(amountBuf);
            if(!model.ammoStockpile_add(AmountOfAmmo{model.knownAmmo_at(ammoInfo), amountBuf})){ // Implicit conversion to AssociatedAmmo
                log_error("Could not add AssociatedAmmo to stockpile from file [" + dirEntry.path().string() + ']');
                continue;
            }
		}
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != FILENAME_WEAPON_TYPES){  // Ignore the weaponTypes file
                log_error("Failed to create Ammo objects from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
            }
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
                if(!model.knownGuns_contains(gunInfoBuf)){
                    if(!model.knownGuns_add(std::make_shared<GunMetadata>(gunInfoBuf))){
                        log_error("Could not insert GunMetadata when reading Event from file [" + dirEntry.path().string() + ']');
                        continue;
                    }
                }

                // Get all ammo used for the gun
                GunAndAmmo gunAndAmmoBuf {model.knownGuns_at(gunInfoBuf)};
                for(auto& ammoElm : gunElm.value().at("ammoUsed").items()){
                    AmmoMetadata infoBuf {ammoElm.value().at("ammoInfo").template get<ShooterCentral::AmmoMetadata>() };

                    if(!model.knownAmmo_contains(infoBuf)){
                        if(!model.knownAmmo_add(std::make_shared<AmmoMetadata>(infoBuf))){
                            log_error("Could not insert AmmoMetadata when reading Event from file [" + dirEntry.path().string() + ']');
                            continue;
                        }
                    }

                    int amountBuf { 0 };
                    ammoElm.value().at("amount").get_to(amountBuf);

                    if(!gunAndAmmoBuf.addAmmoUsed(AmountOfAmmo{model.knownAmmo_at(infoBuf), amountBuf})){
                        log_error(std::format("Could not add AmountOfAmmo [{}] to Gun [{}] when reading Event from file [{}]", infoBuf.name, gunInfoBuf.name, dirEntry.path().string()));
                        continue;
                    }
                } // End of all Ammo elements

                if(!eventBuf.addGun(gunAndAmmoBuf)){
                    log_error(std::format("Could not add Gun [{}] when reading Event from file [{}]", gunInfoBuf.name, dirEntry.path().string()));
                    continue;
                }
	        } // End of all Gun elements

            if(!model.events_add(std::make_shared<Event>(eventBuf))){
                log_error(std::format("Could not add Event to model. Event file [{}]", dirEntry.path().string()));
                continue;
            }
		} // End of all file elements
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != FILENAME_WEAPON_TYPES){  // Ignore the weaponTypes file
                log_error("Failed to create Event object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
            }
		}
	}
    
	return true;
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





























