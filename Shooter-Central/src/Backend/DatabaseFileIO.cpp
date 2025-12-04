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

    j.at("date").get_to(dateBuf);
    j.at("eventType").get_to(etBuf);
    j.at("location").get_to(locBuf);
    j.at("notes").get_to(data.notes);

    data.location   = Location  { locBuf.c_str() };
    data.eventType  = ShootingEventType { etBuf.c_str() }; 
    data.date       = std::chrono::floor<std::chrono::days>(stringToTimepoint(dateBuf));
}


void to_json (LAS::json& j, const AmmoMetadata& data){
    j = LAS::json {
        { "name",           data.name },
        { "manufacturer",   data.manufacturer.getName() },
        { "cartridge",      data.cartridge.getName() },
        { "grainWeight",    data.grainWeight},
    };
}
void from_json  (const LAS::json& j, AmmoMetadata& data){
    std::string textBuf;

    j.at("name").get_to(data.name);

    j.at("manufacturer").get_to(textBuf);
    data.manufacturer = Manufacturer { textBuf.c_str() };

    j.at("cartridge").get_to(textBuf);
    data.cartridge = Cartridge { textBuf.c_str() };

    j.at("grainWeight").get_to(data.grainWeight);
}


void to_json(LAS::json& j, const GunMetadata& data){
    j = LAS::json{
        { "name",           data.name        },
        { "weaponType",     data.weaponType.getName()  },
        { "cartridge",      data.cartridge.getName()   },
    };
}
void from_json  (const LAS::json& j, GunMetadata& data){
    std::string textBuf;

    j.at("name").get_to(data.name);

    j.at("weaponType").get_to(textBuf);
    data.weaponType = WeaponType { textBuf.c_str() };

    j.at("cartridge").get_to(textBuf);
    data.cartridge = Cartridge { textBuf.c_str() };
}


void to_json(LAS::json& j, const AmountOfAmmo& data){
    j = LAS::json {
        { "ammoInfo",   data.getAmmoInfo() },
        { "amount",     data.getAmount() }
    };
}
void from_json (const LAS::json& j, AmountOfAmmo& data){
    AmmoMetadata infoBuffer;
    int amountBuf { 0 };

    j.at("ammoInfo").get_to(infoBuffer);
    j.at("amount").get_to(amountBuf);
    
    data = AmountOfAmmo { infoBuffer, amountBuf };
}


void to_json(LAS::json& j, const StockpileAmmo& data){
    j["ammoInfo"]       = data.getAmmoInfo();
    j["amountOnHand"]   = data.getAmountOnHand();
    j["isActive"]       = data.isActive();
}
void from_json(const LAS::json& j, StockpileAmmo& data){
    AmmoMetadata ammoInfo { j.at("ammoInfo").get<AmmoMetadata>() };
    data = StockpileAmmo { ammoInfo };
    data.setActive(j.at("isActive").get<bool>());
    data.addAmount(j.at("amountOnHand").get<int>());
}


void to_json(LAS::json& j, const GunTrackingAmmoUsed& data){
    LAS::json ammoUsedArray = LAS::json::array();

    for(const auto& amountOfAmmo : data.getAmmoUsed() )
        ammoUsedArray.emplace_back(amountOfAmmo);

    j["gunInfo"] = data.getGunInfo();
    j["ammoUsed"] = ammoUsedArray;
}
void from_json(const LAS::json& j, GunTrackingAmmoUsed& data){
    GunMetadata gunInfo { j.at("gunInfo").get<GunMetadata>() };
        
    GunTrackingAmmoUsed buf { gunInfo };
    
    for (const auto& amountOfAmmoJson : j.at("ammoUsed").items()){
        AmountOfAmmo amountOfAmmo = amountOfAmmoJson.value().get<AmountOfAmmo>();
        buf.addAmmoUsed( amountOfAmmo );
    }

    data = buf;
}


void to_json(LAS::json& j , const ArmoryGun& data){
    j["gunInfo"]    = data.getGunInfo();
    j["isActive"]   = data.isActive();
}
void from_json(const LAS::json& j, ArmoryGun& data ){
    GunMetadata gunInfo { j.at("gunInfo").get<GunMetadata>() };
    data = ArmoryGun { gunInfo };
    data.setActive(j.at("isActive").get<bool>());
}



void to_json(LAS::json& j, const ShootingEvent& event){
    using LAS::json;

    json gunsUsedArray = json::array();
    for(const auto& gunAmmoUsed : event.getGunsUsed() ){
        json gunAmmoUsedJson = gunAmmoUsed;
        gunsUsedArray.emplace_back(gunAmmoUsedJson);
    }

    j["eventInfo"]  = event.getInfo();
    j["gunsUsed"]   = gunsUsedArray;

}
void from_json(const LAS::json& j, ShootingEvent& event){
    ShootingEventMetadata eventInfo;

    j.at("eventInfo").get_to(eventInfo);

    ShootingEvent buf { eventInfo };

    for (const auto& gunJson : j.at("gunsUsed").items()) {
        GunTrackingAmmoUsed gunTrackingAmmo = gunJson.value().get<GunTrackingAmmoUsed>();
        buf.addGun(gunTrackingAmmo);
    }

    event = buf;
}






bool write (std::string directory, const StockpileAmmo& data){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;
    
    json j = data;

    std::ofstream file{makeFileName(directory, data.getAmmoInfo())};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
bool write (std::string directory, const ArmoryGun& data){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;
    
    json j = data;

    std::ofstream file{makeFileName(directory, data.getGunInfo())};
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

    json j = data;

    std::ofstream file{makeFileName(directory, data.getInfo())};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}






bool read (std::ifstream& file, ShootingEvent& data){
    try{
        data = LAS::json::parse(file).get<ShootingEvent>();
    }
    catch(std::exception& e){
        LAS::log_error(std::format("Failed to parse JSON. What: {}", e.what()) );
        return false;
    }

    return true;
}
bool read (std::ifstream& file, ArmoryGun& data){
    try{
        data = LAS::json::parse(file).get<ArmoryGun>();
    }
    catch(std::exception& e){
        LAS::log_error(std::format("Failed to parse JSON. What: {}", e.what()) );
        return false;
    }

    return true;
}
bool read (std::ifstream& file, StockpileAmmo& data){
    try{
        data = LAS::json::parse(file).get<StockpileAmmo>();
    }
    catch(std::exception& e){
        LAS::log_error(std::format("Failed to parse JSON. What: {}", e.what()) );
        return false;
    }

    return true;
}








bool readEvents(Database& db, const std::filesystem::path& workingDirectory) {
    using namespace LAS;

    if(!std::filesystem::exists(workingDirectory))
        return false;

	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
        std::ifstream inputFile{ dirEntry.path(), std::ios::in };
        ShootingEvent event { };

        if(!read(inputFile, event)){
            LAS::log_error(std::format("Failed to create Event object from file [{}]", dirEntry.path().string()));
            continue;
        }

        if(!db.addEvent(event).wasAdded)
            LAS::log_error(std::format("Failed to add Event on {}", event.printDate()));
	}
    
	return true;
}
bool readGuns(Database& db, const std::filesystem::path& workingDirectory) {
    using namespace LAS;

    if(!std::filesystem::exists(workingDirectory))
        return false;

	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
        std::ifstream inputFile{ dirEntry.path(), std::ios::in };
        ArmoryGun data { };

        if(!read(inputFile, data)){
            LAS::log_error(std::format("Failed to create ArmoryGun object from file [{}]", dirEntry.path().string()));
            continue;
        }

        const GunMetadata& gunInfo { data.getGunInfo() };

        // If already exists, ensure isActive status is updated
        if(db.armoryContains(gunInfo))
            db.getGun(gunInfo) = data;
        else {
            if(!db.addToArmory(data))
                LAS::log_error(std::format("Failed to add ArmoryGun named [{}]", gunInfo.name));
        }
	}
    
	return true;
}
bool readAmmo(Database& db, const std::filesystem::path& workingDirectory) {
    using namespace LAS;

    if(!std::filesystem::exists(workingDirectory))
        return false;

	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
        std::ifstream inputFile{ dirEntry.path(), std::ios::in };
        StockpileAmmo data { };

        if(!read(inputFile, data)){
            LAS::log_error(std::format("Failed to create StockpileAmmo object from file [{}]", dirEntry.path().string()));
            continue;
        }

        const AmmoMetadata& ammoInfo { data.getAmmoInfo() };

        // If already exists, replace with read object
        if(db.stockpileContains(ammoInfo))
            db.getAmmo(ammoInfo) = data;
        else{
            if(!db.addToStockpile(data))
                LAS::log_error(std::format("Failed to add StockpileAmmo named [{}]", ammoInfo.name));
        }
	}
    
	return true;
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

}   // End SC namespace
