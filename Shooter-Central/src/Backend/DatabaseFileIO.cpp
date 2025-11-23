#include "DatabaseFileIO.h"

namespace ShooterCentral {

void to_json(LAS::json& j, const ShootingEventMetadata& data){
    std::ostringstream timeString;
    timeString << std::chrono::system_clock::time_point{std::chrono::sys_days{data.date}};

    j = LAS::json{
        { "location",   data.location.getName()    },
        { "eventType",  data.eventType.getName()   },
        { "notes",      data.notes       },
        { "date",       timeString.str() }
    };
}
void from_json(const LAS::json& j, ShootingEventMetadata& data){
    std::string locBuf, etBuf, dateBuf;

    j.at("notes").get_to(data.notes);
    j.at("location").get_to(locBuf);
    j.at("eventType").get_to(etBuf);
    j.at("date").get_to(dateBuf);

    data.location   = Location  { locBuf.c_str() };
    data.eventType  = ShootingEventType { etBuf.c_str() }; 
    data.date       = std::chrono::floor<std::chrono::days>(stringToTimepoint(dateBuf));
}

void to_json (LAS::json& j, const AmmoMetadata& data){
    j = LAS::json {
        { "name",           data.name },
        { "manufacturer",   data.manufacturer.getName() },
        { "cartridge",      data.cartridge.getName() },
        { "grain",          data.grainWeight},
    };
}
void from_json  (const LAS::json& j, AmmoMetadata& data){

}

void to_json(LAS::json& j, const GunMetadata& gun){
    j = LAS::json{
        { "name",           gun.name        },
        { "weaponType",     gun.weaponType.getName()  },
        { "cartridge",      gun.cartridge.getName()   },
    };
}

void to_json(LAS::json& j, const AmountOfAmmo& data){
    j = LAS::json {
        { "ammoInfo",   data.getAmmoInfo() },
        { "amount",     data.getAmount() }
    };
}

std::string makeFileName    (std::string directory, const GunMetadata& gun) {
    std::ostringstream fileName;
    fileName << directory;

    for(const auto& c : std::string{gun.weaponType.getName()}){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : std::string{gun.cartridge.getName()}){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : std::string{gun.name}){     // Remove spaces
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

    for(const auto& c : std::string{ammo.manufacturer.getName()} ){     // Remove spaces and make lowercase
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : std::string{ammo.cartridge.getName()} ){     // Remove spaces and make lowercase
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : std::string{ ammo.name } ){     // Remove spaces and make lowercase
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_' << ammo.grainWeight;

    fileName << ".json";

    return fileName.str();
}
std::string makeFileName(std::string directory, const ShootingEventMetadata& event){
    // Output fileName
    std::ostringstream fileName;
    fileName << directory << std::format("{:%Y-%m-%d}", event.date) << '_';

    for(const auto& c : std::string{event.eventType.getName()} ){     // Remove spaces, make sure alnum
        if(c == ' ' || c == '\t')
            fileName << '-';
        else if(isalnum(c))
            fileName << c;
    }

    fileName << '_';

    for(const auto& c : std::string{event.location.getName()} ){     // Remove spaces, make sure alnum
        if(c == ' ' || c == '\t')
            fileName << '-';
        else if(isalnum(c))
            fileName << c;
    }

    fileName << ".json";

    return fileName.str();
}

// TESTING WRITE
bool write (std::string directory, const ShootingEventMetadata& data){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;
    

    json eventJson = json{
        { "eventInfo",  data },
    };

    std::ofstream file{makeFileName(directory, data)};
    file << std::setw(1) << std::setfill('\t') << eventJson;
    file.close();
   
    return true;
}

/*
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
bool write (std::string directory, const ShootingEvent& data){
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
*/


}   // End SC namespace
