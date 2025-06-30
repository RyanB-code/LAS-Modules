#include "Gun.h"

using namespace ShooterCentral;
using namespace LAS;

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
    else
        ammoTracker.at(trackedAmmo.ammoType).amount += trackedAmmo.amount;
    
    return true;
}
void Gun::getAllAmmoUsed(ConstTrackedAmmoPtrList& ammoUsed) const{
    if(!ammoUsed.empty())
        ammoUsed.erase(ammoUsed.begin(), ammoUsed.end());

    for(const auto& [ammoType, trackedAmmo] : ammoTracker){
        ammoUsed.emplace_back(std::make_shared<const TrackedAmmo>(trackedAmmo));
    }
}
// MARK: Operators
bool Gun::operator==(const Gun& other) const{
    if(this->name == other.name && this->weaponType == other.weaponType && this->cartridge == other.cartridge)
        return true;
    else
        return false;
}
bool Gun::operator<(const Gun& other) const{
    return std::tuple{getWeaponType().getName(), getCartridge().getName(), getName()} < std::tuple{other.getWeaponType().getName(), other.getCartridge().getName(), other.getName()};
}


// MARK: To/From JSON
void ShooterCentral::to_json(LAS::json& j, const Gun& gun){
    using LAS::json;

    // Write every ammo type used and amount
    ConstTrackedAmmoPtrList ammoUsed;
    gun.getAllAmmoUsed(ammoUsed);
    
	json trackedAmmoArray = json::array();
	for (const auto& ta : ammoUsed){
        trackedAmmoArray.emplace_back(*ta);
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
    using LAS::json;

    std::string nameBuf, weaponTypeBuf, cartBuf; 

    j.at("name").get_to(nameBuf);
    j.at("weaponType").get_to(weaponTypeBuf);
    j.at("cartridge").get_to(cartBuf);

    Gun gunBuf {nameBuf, WeaponType{weaponTypeBuf}, Cartridge{cartBuf} };
    
    json trackedAmmoList;
	j.at("trackedAmmo").get_to(trackedAmmoList);
	
    // Add for each element
	for (auto& trackedAmmoListElement : trackedAmmoList.items()) {
		json pair = trackedAmmoListElement.value();        
        TrackedAmmo taBuf {pair.template get<ShooterCentral::TrackedAmmo>()};
        
        gunBuf.addToRoundCount(taBuf);
	}

    gun = gunBuf;

}

// MARK: Gun Tracker
GunTracker::GunTracker(){

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

    // Retrieve shared ptr to gun to return if successfully instered
    std::pair  returnBuffer { guns.try_emplace(gun, std::make_shared<Gun>(gun)) };
    
    
    if(returnBuffer.second){
        // Add to known weapon types
        addWeaponType(gun.getWeaponType());

        return std::pair(guns.at(gun), true);
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

bool GunTracker::contains(const Gun& gun) const {
    return guns.contains(gun);
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
        log_info("Saved all Guns");
        return true;
    }

    if(!std::filesystem::exists(saveDirectory)){
        log_error("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Gun objects.");
        return false;
    }

    int gunsNotSaved { 0 };
	for(const auto& [key, gun] : guns) {
        if(!GunHelper::writeGun(saveDirectory, *gun)){
            log_error("Failed to save gun [" + gun->getName() + "]");
            ++gunsNotSaved;
        }
	}

    if(gunsNotSaved > 0)
        log_error("Could not save " + std::to_string(gunsNotSaved) + " guns.");
    else
        log_info("Saved guns");

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
        log_info("Saved weapon types");
        return true;
    }
    else{
        log_error("Could not save weapon types");
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
                log_error("Could not insert Gun from file [" + dirEntry.path().string() + ']');
		}
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != WEAPON_TYPES_FILENAME){  // Ignore the weaponTypes file
                log_error("Failed to create Gun object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
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
        log_error("Error reading weapon types. What: " + std::string{e.what()});
    }

    for(const auto& wt : rawWweaponTypes){
        if(!addWeaponType(wt))
            log_warn("Weapon Type named [" + wt.getName() + "] already exists.");
    }

    return true;
}


// MARK: GUN HELPER
std::string GunHelper::makeFileName    (std::string directory, const Gun& gun){

    std::ostringstream fileName;
    fileName << directory;

    for(const auto& c : gun.getWeaponType().getName()){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : gun.getCartridge().getName()){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << '_';

    for(const auto& c : gun.getName()){     // Remove spaces
        if(isalnum(c))
            fileName << c;
        else if(c == ' ' || c == '\t')
            fileName << '-';
    }

    fileName << ".json";

    return fileName.str();
}


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

    // Write to file
    std::ofstream file{makeFileName(directory, gun)};
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
