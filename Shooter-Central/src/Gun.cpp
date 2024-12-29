#include "Gun.h"

using namespace ShooterCentral;

// MARK: Location
WeaponType::WeaponType(std::string setName) : name { setName } {

}
WeaponType::~WeaponType(){

}
std::string WeaponType::getName() const {
    return name;
}
WeaponType::operator std::string() const {
    return name;
}
bool WeaponType::operator==(const WeaponType& other) const{
    if(this->name == other.getName())
        return true;
    else
        return false;
}

std::ostream& ShooterCentral::operator<<(std::ostream& os, const WeaponType& weaponType){
    os << weaponType.getName();
    return os;
}

// MARK: Gun
Gun::Gun(std::string setName, WeaponType setWeaponType, Cartridge setCartridge)
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
int Gun::getRoundCount() const{
    uint64_t rounds { 0 };

    for(const auto& [key, ta] : ammoTracker)
        rounds += ta.amount;

    return rounds;
}
WeaponType Gun::getWeaponType() const{
    return weaponType;
}
Cartridge Gun::getCartridge () const{
    return cartridge;
}
bool Gun::addToRoundCount(TrackedAmmo trackedAmmo){
    if(!ammoTracker.contains(trackedAmmo.ammoType))
        return ammoTracker.try_emplace(trackedAmmo.ammoType, trackedAmmo).second;
    
    return true;
}
void Gun::getAllAmmoUsed(ConstTrackedAmmoPtrList& ammoUsed) const{
    if(!ammoUsed.empty())
        ammoUsed.erase(ammoUsed.begin(), ammoUsed.end());

    for(const auto& [ammoType, trackedAmmo] : ammoTracker){
        ammoUsed.emplace_back(std::make_shared<const TrackedAmmo>(trackedAmmo));
    }
}
bool Gun::operator==(const Gun& other) const{
    if(this->name == other.name && this->weaponType == other.weaponType && this->cartridge == other.cartridge)
        return true;
    else
        return false;
}
// MARK: To/From JSON
void ShooterCentral::to_json(LAS::json& j, const Gun& gun){
    using LAS::json;

    // Write every ammo type used and amount
    ConstTrackedAmmoPtrList ammoUsed;
    gun.getAllAmmoUsed(ammoUsed);
    
	nlohmann::json trackedAmmoArray = nlohmann::json::array();
	for (const auto& ta : ammoUsed){
        json trackedAmmoJson { *ta };       
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

    Gun gunBuf {nameBuf, WeaponType{weaponTypeBuf}, Cartridge{cartBuf} };
    
    nlohmann::json trackedAmmoList;
	j.at("trackedAmmo").get_to(trackedAmmoList);
	
    // Add for each element
	for (auto& trackedAmmoListElement : trackedAmmoList.items()) {
		nlohmann::json pair = trackedAmmoListElement.value();        
        TrackedAmmo taBuf {pair.at(0).template get<ShooterCentral::TrackedAmmo>()};
        
        gunBuf.addToRoundCount(taBuf);
	}

    gun = gunBuf;

}

// MARK: Gun Tracker
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
GunPtr  GunTracker::createGun(const std::string& name, const WeaponType& weaponType, const Cartridge& cartridge){
    Gun gunBuf { name, weaponType, cartridge};

    if(addGun(gunBuf).second)
        return guns.at(gunBuf);
    else
        return nullptr;
}

// MARK: Add Items
bool GunTracker::addWeaponType (const WeaponType& type){
    if(weaponTypes.contains(type))
        return false;

    weaponTypes.try_emplace(type, type);
    return weaponTypes.contains(type);
}
std::pair<GunPtr, bool> GunTracker::addGun(Gun& gun){
    if(guns.contains(gun))
        return std::pair(guns.at(gun), false);  // Return shared ptr to gun if it already exists, but false since item wasnt inserted

    // Add to known weapon types
    addWeaponType(gun.getWeaponType());

    // Retrieve shared ptr to gun to return if successfully instered
    GunPtr returnBuffer { nullptr };
    if(guns.try_emplace(gun, std::make_shared<Gun>(gun)).second) {
        returnBuffer = guns.at(gun);
        return std::pair(returnBuffer, true);
    }
    else
         return std::pair(nullptr, false);
}
std::pair<GunPtr, bool> GunTracker::addGun(GunPtr gun){
    if(!gun)
        return std::pair(nullptr, false);

    if(guns.contains(*gun))
        return std::pair(guns.at(*gun), false);

    // Add to known weapon types
    addWeaponType(gun->getWeaponType());

    // Retrieve shared ptr to gun to return if successfully inserted
    GunPtr returnBuffer { nullptr };

    if(guns.try_emplace(*gun, gun).second) {
        returnBuffer = guns.at(*gun);
        return std::pair(returnBuffer, true);
    }
    else
         return std::pair(nullptr, false);
}

// MARK: Remove Items
bool GunTracker::removeGun(const Gun& gun){
    if(!guns.contains(gun))
        return true;

    guns.erase(gun);
    return !guns.contains(gun); // Return the inverse of contain()
}

// MARK: Get Gun
GunPtr  GunTracker::getGun (const Gun& gun){
    if(!guns.contains(gun))
        return nullptr;

    return guns.at(gun);
}


// MARK: Get Info
void    GunTracker::getRoundsShotPerCartridge(std::unordered_map<std::string, uint64_t>& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, gun] : guns){
        list[gun->getCartridge()] = gun->getRoundCount();
    }
}
int GunTracker::getGunTotal() const{
    return guns.size();
}
void GunTracker::getAllGuns(std::vector<ConstGunPtr>& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, gun] : guns)
        list.push_back(gun);
}
void GunTracker::getAllWeaponTypeNames   (WeaponTypeList& names) const{
    if(!names.empty())
        names.erase(names.begin(), names.end());

    for(const auto& [key, wt] : weaponTypes){
        names.emplace_back(wt);
    }
}

// MARK: Write
bool GunTracker::writeAllGuns() const{
    if(guns.empty()){
        logger->log("Saved all Guns", LAS::Logging::Tags{"ROUTINE", "SC"});
        return true;
    }

    if(!std::filesystem::exists(saveDirectory)){
        logger->log("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Gun objects.", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }

    int gunsNotSaved { 0 };
	for(const auto& [key, gun] : guns) {
        if(!GunHelper::writeGun(saveDirectory, *gun)){
            logger->log("Failed to save gun [" + gun->getName() + "]", LAS::Logging::Tags{"ERROR", "SC"});
            ++gunsNotSaved;
        }
	}

    if(gunsNotSaved > 0)
        logger->log("Could not save " + std::to_string(gunsNotSaved) + " guns.", LAS::Logging::Tags{"ERROR", "SC"});
    else
        logger->log("Saved guns", LAS::Logging::Tags{"ROUTINE", "SC"});

    return true;
}
bool GunTracker::writeAllWeaponTypes () const {    
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += WEAPON_TYPES_FILENAME;

    WeaponTypeList rawWeaponTypes;

    for(auto& c : weaponTypes)
        rawWeaponTypes.emplace_back(c.second);

    if(GunHelper::writeAllWeaponTypes(fullPath, rawWeaponTypes)){
        logger->log("Saved weapon types", LAS::Logging::Tags{"ROUTINE", "SC"});
        return true;
    }
    else{
        logger->log("Could not save weapon types", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
}
// MARK: Read
bool GunTracker::readGuns(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    const std::filesystem::path workingDirectory{saveDirectory};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
			Gun gunBuf {GunHelper::readGun(dirEntry.path().string())};

            if(!addGun(gunBuf).first)
                logger->log("Could not insert Gun from file [" + dirEntry.path().string() + ']', LAS::Logging::Tags{"ERROR", "SC"});
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
bool GunTracker::readWeaponTypes  () {
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += WEAPON_TYPES_FILENAME;

    WeaponTypeList rawWweaponTypes;
    try{
        GunHelper::readWeaponTypes(fullPath, rawWweaponTypes);
    }
    catch(std::exception& e){
        logger->log("Error reading Weapon Types", LAS::Logging::Tags{"ERROR", "SC"});
        logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
    }

    for(const auto& wt : rawWweaponTypes){
        if(!addWeaponType(wt))
            logger->log("Weapon Type named [" + wt.getName() + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
    }

    return true;
}


// MARK: GUN HELPER


// MARK: R/W Gun
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
// MARK: R/W Cartridges
bool GunHelper::writeAllWeaponTypes(std::string path, const WeaponTypeList& list){
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
bool GunHelper::readWeaponTypes (std::string path, WeaponTypeList& list){
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