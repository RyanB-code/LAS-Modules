#include "Gun.h"

using namespace ShooterCentral;


// MARK: GUN
Gun::Gun() : name {"N/A"}, weaponType {"N/A"}, cartridge {"N/A"}
{

}
Gun::Gun(std::string setName, std::string setWeaponType, std::string setCartridge)
        :   name        { setName },
            weaponType  { setWeaponType },
            cartridge   { setCartridge }
{

}
Gun::~Gun(){

}
std::string Gun::getName() const{
    return name;
}
uint64_t Gun::getRoundCount() const{
    uint64_t rounds { 0 };
    for(const auto& pair : ammoTracker){
        rounds += pair.second->amount;
    }

    return rounds;
}
std::string Gun::getWeaponType() const{
    return weaponType;
}
std::string Gun::getCartridge () const{
    return cartridge;
}
bool Gun::addToRoundCount(uint64_t amount, const AmmoType& ammoType){
    if(!ammoTracker.contains(ammoType))
        return ammoTracker.try_emplace(ammoType, std::make_shared<TrackedAmmo>(TrackedAmmo{ammoType, amount})).second;
    
    ammoTracker.at(ammoType)->amount += amount;
    return true;
}
void Gun::getAllAmmoUsed(std::vector<TrackedAmmo>& ammoUsed) const{
    if(!ammoUsed.empty())
        ammoUsed.erase(ammoUsed.begin(), ammoUsed.end());

    for(const auto& pair : ammoTracker){
        ammoUsed.push_back(*pair.second);
    }
}
bool Gun::operator==(const Gun& other) const{
    if(this->name == other.name && this->weaponType == other.weaponType && this->cartridge == other.cartridge)
        return true;
    else
        return false;
}

void ShooterCentral::to_json(LAS::json& j, const Gun& gun){
    using LAS::json;

    // Write every ammo type used and amount
    std::vector<TrackedAmmo> ammoUsed;
    gun.getAllAmmoUsed(ammoUsed);
    
	nlohmann::json trackedAmmoArray = nlohmann::json::array();
	for (const auto& pair : ammoUsed){
        json trackedAmmoJson { pair.ammoType, pair.amount };       
        trackedAmmoArray.emplace_back(trackedAmmoJson);
    }
    
    // Make JSON
    j = LAS::json{
        { "name",           gun.getName()       },
        { "weaponType",     gun.getWeaponType() },
        { "cartridge",      gun.getCartridge()  },
        { "trackedAmmo",    trackedAmmoArray    }
    };
}
void ShooterCentral::from_json(const LAS::json& j, Gun& gun){
    std::string nameBuf, weaponTypeBuf, cartBuf; 

    j.at("name").get_to(nameBuf);
    j.at("weaponType").get_to(weaponTypeBuf);
    j.at("cartridge").get_to(cartBuf);

    Gun gunBuf {nameBuf, weaponTypeBuf, cartBuf };
    
    nlohmann::json trackedAmmoList;
	j.at("trackedAmmo").get_to(trackedAmmoList);
	
    // Add for each element
	for (auto& trackedAmmoListElement : trackedAmmoList.items()) {
		nlohmann::json pair = trackedAmmoListElement.value();

        uint64_t amountBuf { 0 };
		AmmoType ammoTypeBuf {pair.at(0).template get<ShooterCentral::AmmoType>()};
		pair.at(1).get_to(amountBuf);
        
        gunBuf.addToRoundCount(amountBuf, ammoTypeBuf);
	}

    gun = gunBuf;

}

// MARK: GUN TRACKER
GunTracker::GunTracker(LAS::Logging::LoggerPtr setLogger): logger { setLogger }
{

}
GunTracker::~GunTracker(){
    
}
bool GunTracker::setDirectory(std::string path) {
    path = LAS::TextManip::ensureSlash(path);

    if(std::filesystem::exists(path)){
        saveDirectory = path;
        return true;
    }
    else
        return false;
}
std::string GunTracker::getDirectory() const{
    return saveDirectory;
}
GunPtr  GunTracker::createGun(const std::string& name, const std::string& weaponType, const std::string& cartridge){
    Gun gunBuf { name, weaponType, cartridge};

    if(addGun(gunBuf))
        return guns.at(gunBuf);
    else
        return nullptr;
}

bool GunTracker::removeGun(const Gun& gun){
    if(!guns.contains(gun))
        return true;

    guns.erase(gun);
    return !guns.contains(gun); // Return the inverse of contain()
}
// MARK: GET INFO
void    GunTracker::getRoundsShotPerCartridge(std::unordered_map<std::string, uint64_t>& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, gun] : guns){
        list[gun->getCartridge()] = gun->getRoundCount();
    }
}
uint64_t GunTracker::getGunTotal() const{
    return guns.size();
}
void GunTracker::getAllGuns(std::vector<Gun>& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, gun] : guns)
        list.push_back(*gun);
}
void GunTracker::getAllWeaponTypeNames   (StringVector& names) const{
    if(!names.empty())
        names.erase(names.begin(), names.end());

    for(const auto& [key, wt] : weaponTypes){
        names.emplace_back(wt);
    }
}
bool GunTracker::addWeaponType           (const std::string& type){
    if(weaponTypes.contains(type))
        return false;

    weaponTypes.try_emplace(type, type);
    return weaponTypes.contains(type);
}
// MARK: R/W Guns
bool GunTracker::writeAllGuns() const{
    if(guns.empty())
        return true;

    if(!std::filesystem::exists(saveDirectory)){
        logger->log("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Ammo objects.", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }

	for(const auto& [key, gun] : guns) {
        if(!GunHelper::writeGun(saveDirectory, *gun)) 
            logger->log("Directory [" + saveDirectory + "] was not found. Ammo [" + gun->getName() + "] was not saved.", LAS::Logging::Tags{"ERROR", "SC"});
	}

    return true;
}
bool GunTracker::readGuns(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    const std::filesystem::path workingDirectory{saveDirectory};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
			Gun gunBuf {GunHelper::readGun(dirEntry.path().string())};

            if(!addGun(gunBuf))
                logger->log("Gun object already exists from file [" + dirEntry.path().string() + ']', LAS::Logging::Tags{"ROUTINE", "SC"});
		}
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != WEAPON_TYPES_FILENAME){  // Ignore the weaponTypes file
                logger->log("Failed to create Gun object from file [" + dirEntry.path().string() + ']', LAS::Logging::Tags{"ERROR", "SC"});
                logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
            }
		}
	}
    
	return true;
}
// MARK: R/W Weapon Types
bool GunTracker::writeAllWeaponTypes () const {    
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += WEAPON_TYPES_FILENAME;

    StringVector rawWeaponTypes;

    for(auto& c : weaponTypes){
        rawWeaponTypes.emplace_back(c.second);
    }

    return GunHelper::writeAllWeaponTypes(fullPath, rawWeaponTypes);
}
bool GunTracker::readWeaponTypes  () {
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += WEAPON_TYPES_FILENAME;

    StringVector rawWweaponTypes;
    try{
        GunHelper::readWeaponTypes(fullPath, rawWweaponTypes);
    }
    catch(std::exception& e){
        logger->log("Error reading Weapon Types", LAS::Logging::Tags{"ERROR", "SC"});
        logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
    }

    for(const auto& s : rawWweaponTypes){
        if(!addWeaponType(s))
            logger->log("Weapon Type named [" + s + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
    }

    return true;
}

// MARK: PRIVATE FUNCTIONS
bool GunTracker::addGun(Gun& gun){
    if(guns.contains(gun))
        return false;

    // Add to known weapon types
    addWeaponType(gun.getWeaponType());

    return guns.try_emplace(gun, std::make_shared<Gun>(gun)).second;
}


// MARK: GUN HELPER


// MARK: R/W GUN
bool GunHelper::writeGun(std::string directory, const Gun& gun){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;

    // Make JSON
    json j = gun;

    // Create JSON file name
    std::string fileName;
    for(const auto& c : gun.getName()){     // Remove spaces and make lowercase
        if(isalpha(c))
            fileName += tolower(c);
        else if(c == ' ' || c == '\t')
            fileName += '_';
        else if(isalnum(c))
            fileName += c;
    }
    fileName += ".json";

    // Make fully qualified path
    std::string filePath;
    filePath = directory + fileName;

    // Write to file
    std::ofstream file{filePath};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
Gun GunHelper::readGun(const std::string& path){
    using LAS::json;

    if(!std::filesystem::exists(path))
        throw std::filesystem::filesystem_error::runtime_error{"File does not exist"};

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    return Gun{j.template get<ShooterCentral::Gun>()};
}
// MARK: R/W CARTRIDGES
bool GunHelper::writeAllWeaponTypes(std::string path, const std::vector<std::string>& list){
    using LAS::json;

    if(list.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& weaponType : list) 
        entries.emplace_back(weaponType);
    
    j["weaponTypes"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
bool GunHelper::readWeaponTypes (std::string path, StringVector& list){
    using LAS::json;

    if(!std::filesystem::exists(path))
        return false;

    if(!list.empty())
        list.erase(list.begin(), list.end());

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    json cartridges;
	j.at("weaponTypes").get_to(cartridges);

	for (auto& elment : cartridges.items()) {
		json obj = elment.value();
        std::string nameBuf;

		obj.get_to(nameBuf);

        list.emplace_back(nameBuf);
	}

    return true;
}