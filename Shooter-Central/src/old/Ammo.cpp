#include "Ammo.h"

using namespace ShooterCentral;
using namespace LAS;


// MARK: Manufacturer
Manufacturer::Manufacturer(std::string setName) : name { setName } {

}
Manufacturer::~Manufacturer(){

}
std::string Manufacturer::getName() const {
    return name;
}
Manufacturer::operator std::string() const {
    return name;
}
bool Manufacturer::operator==(const Manufacturer& other) const{
    if(this->name == other.getName())
        return true;
    else
        return false;
}

std::ostream& ShooterCentral::operator<<(std::ostream& os, const Manufacturer& manufactuerer){
    os << manufactuerer.getName();
    return os;
}

// MARK: Cartridge
Cartridge::Cartridge(std::string setName) : name { setName } {

}
Cartridge::~Cartridge(){

}
std::string Cartridge::getName() const {
    return name;
}
Cartridge::operator std::string() const {
    return name;
}
bool Cartridge::operator==(const Cartridge& other) const{
    if(this->name == other.getName())
        return true;
    else
        return false;
}
std::ostream& ShooterCentral::operator<<(std::ostream& os, const Cartridge& cartridge){
    os << cartridge.getName();
    return os;
}

// MARK: Ammo Type Operators
bool AmmoType::operator==(const AmmoType& other) const {
    if(name == other.name && manufacturer == other.manufacturer && cartridge == other.cartridge && grainWeight == other.grainWeight)
        return true;
    else
        return false;
}
bool AmmoType::operator<(const AmmoType& other) const{
   std::tuple<std::string, std::string, std::string, int> lhs {cartridge.getName(), manufacturer.getName(), name, grainWeight};
   std::tuple<std::string, std::string, std::string, int> rhs {other.cartridge.getName(), other.manufacturer.getName(), other.name, other.grainWeight};

   return lhs < rhs;
}



// MARK: Tracked Ammo
bool TrackedAmmo::operator==(const TrackedAmmo& other) const{
    if(this->ammoType == other.ammoType && this->amount == other.amount)
        return true;
    else
        return false;
}

// MARK:JSON Ammo Type
void ShooterCentral::to_json (LAS::json& j, const AmmoType& ammoType){
    j = LAS::json {
        { "name",           ammoType.name },
        { "manufacturer",   ammoType.manufacturer },
        { "cartridge",      ammoType.cartridge },
        { "grain",          int{ammoType.grainWeight}}
    };
}
void ShooterCentral::from_json(const LAS::json& j, AmmoType& ammoType){
    std::string manufacturerBuf { }, cartridgeBuf { };

    j.at("name").get_to(ammoType.name);
    j.at("manufacturer").get_to(manufacturerBuf);
    j.at("cartridge").get_to(cartridgeBuf);
    j.at("grain").get_to(ammoType.grainWeight);

    ammoType.manufacturer   = Manufacturer{manufacturerBuf};
    ammoType.cartridge      = Cartridge{cartridgeBuf};

}

// MARK: JSON Tracked Ammo
void ShooterCentral::to_json (LAS::json& j, const TrackedAmmo& ammo){
    j = LAS::json {
        { "name",           ammo.ammoType.name },
        { "manufacturer",   ammo.ammoType.manufacturer },
        { "cartridge",      ammo.ammoType.cartridge },
        { "grain",          int{ammo.ammoType.grainWeight}},
        { "amount",         ammo.amount }
    };
}
void ShooterCentral::from_json(const LAS::json& j, TrackedAmmo& ammo){
    std::string manufacturerBuf { }, cartridgeBuf { };

    j.at("name").get_to(ammo.ammoType.name);
    j.at("manufacturer").get_to(manufacturerBuf);
    j.at("cartridge").get_to(cartridgeBuf);
    j.at("grain").get_to(ammo.ammoType.grainWeight);
    j.at("amount").get_to(ammo.amount);

    ammo.ammoType.manufacturer   = Manufacturer{manufacturerBuf};
    ammo.ammoType.cartridge      = Cartridge{cartridgeBuf};

}

// MARK: AMMO TRACKER
AmmoTracker::AmmoTracker(){

}
AmmoTracker::~AmmoTracker(){
    
}



// MARK: Add Items
bool AmmoTracker::addCartridge (const std::string& cartridge){
    if(cartridges.contains(cartridge))
        return false;

    cartridges.try_emplace(cartridge, cartridge);
    return cartridges.contains(cartridge);
}
bool AmmoTracker::addManufacturer (const std::string& manufacturer){
    if(manufacturers.contains(manufacturer))
        return false;

    manufacturers.try_emplace(manufacturer, manufacturer);
    return manufacturers.contains(manufacturer);
}
bool AmmoTracker::addAmmoToStockpile (const TrackedAmmo& trackedAmmo){
    if(ammoStockpile.contains(trackedAmmo.ammoType)){
        ammoStockpile.at(trackedAmmo.ammoType)->amount += trackedAmmo.amount;
        return true;
    }
    

    if(ammoStockpile.try_emplace(trackedAmmo.ammoType, std::make_shared<TrackedAmmo>(trackedAmmo)).second){
        addCartridge(trackedAmmo.ammoType.cartridge);           // Add to cartridges, does not matter return
        addManufacturer(trackedAmmo.ammoType.manufacturer);

        return true;
    }
    else
        return false;
}

// MARK: Remove Items
bool AmmoTracker::removeAmmoFromStockpile (const TrackedAmmo& trackedAmmo){
    if(!ammoStockpile.contains(trackedAmmo.ammoType))
        return true;
    
    auto& ammo { ammoStockpile.at(trackedAmmo.ammoType) };

    if(ammo->amount - trackedAmmo.amount >= 100000)
        return false;
    else
        ammo->amount -= trackedAmmo.amount;

    return true;
}
void AmmoTracker::removeAllAmmoFromStockpile  (){
    if(ammoStockpile.empty())
        return;

    ammoStockpile.clear();
}

// MARK: Get Info
bool AmmoTracker::contains(const AmmoType& ammoType) const {
    return ammoStockpile.contains(ammoType);
}

void AmmoTracker::getAllAmmoNames(StringVector& names) const{
    if(!names.empty())
        names.erase(names.begin(), names.end());


    for(const auto& ammo : ammoStockpile){
        names.emplace_back(ammo.second->ammoType.name);
    }
}
void AmmoTracker::getAllAmmo (std::vector<ConstTrackedAmmoPtr>& list)   const{
     if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, ammo] : ammoStockpile){
        list.emplace_back(ammo);
    }
}
void AmmoTracker::getAmmoCountByCartridge (AmountPerCartridgeList& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    CartridgeList namesList;
    getAllCartridges(namesList); // Get all the names of cartridges

    // For every cartridge, add the amounts together
    for(const auto& name : namesList){
        int amountBuf { 0 };
        std::vector<TrackedAmmo> sameCartridgeList;
        getAllAmmoByCartridge(sameCartridgeList, name);

        for(const auto& ammo : sameCartridgeList)
            amountBuf += ammo.amount;

        list.emplace_back(std::pair{name, amountBuf});  // Add to list
    }
}
void AmmoTracker::getAllAmmoByCartridge(std::vector<TrackedAmmo>& list, const Cartridge& cartridge)   const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [ammoType, trackedAmmoPtr] : ammoStockpile){
        if(trackedAmmoPtr->ammoType.cartridge == cartridge)
            list.emplace_back(*trackedAmmoPtr);
    }
}
void AmmoTracker::getAllCartridges(CartridgeList& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());


    for(const auto& [key, cartridge] : cartridges){
        list.emplace_back(cartridge);
    }
}
void AmmoTracker::getAllManufacturers(ManufacturerList& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, manufacturer] : manufacturers){
        list.emplace_back(manufacturer);
    }
}
int AmmoTracker::getAmountForCartridge   (const Cartridge& cartridge)    const{
    if(!cartridges.contains(cartridge))
        return -1;

    int amountBuf { 0 };
    std::vector<TrackedAmmo> sameCartridgeList;
    getAllAmmoByCartridge(sameCartridgeList, cartridge);

    // For the cartridge, add the amounts together
    for(const auto& ammo : sameCartridgeList)
        amountBuf += ammo.amount;

    return amountBuf;
}


// MARK: Write
bool AmmoTracker::writeAllAmmo() const{
    if(ammoStockpile.empty())
        return true;

    if(!std::filesystem::exists(saveDirectory)){
        log_error("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Ammo objects.");
        return false;
    }

    int ammoNotSaved { 0 };
	for(const auto& pair : ammoStockpile) {
        const auto& ammo = *pair.second;

        try{
            if(!AmmoHelper::writeTrackedAmmo(saveDirectory, ammo)){
                log_error("Directory [" + saveDirectory + "] was not found. Ammo [" + ammo.ammoType.name + "] was not saved.");
                ++ammoNotSaved;
            }
        }
        catch(std::exception& e){
            log_error("Error creating JSON object. Code " + std::string{e.what()} + ". What: " + std::string{e.what()});
            ++ammoNotSaved;
        }
	}

    if(ammoNotSaved > 0)
        log_error("Could not save " + std::to_string(ammoNotSaved) + " ammo objects.");
    else
        log_info("Saved all ammo");

    return true;
}
bool AmmoTracker::writeAllCartridges() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += CARTRIDGES_FILENAME;

    std::vector<std::string> rawCartridges;

    for(auto& c : cartridges){
        rawCartridges.emplace_back(c.second);
    }

    if(AmmoHelper::writeAllCartridges(fullPath, rawCartridges)){
        log_info("Saved cartridges");
        return true;
    }
    else{
        log_error("Could not save cartridges");
        return false;
    }
}
bool AmmoTracker::writeAllManufacturers() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += MANUFACTURERS_FILENAME;

    StringVector rawManufacturers;

    for(auto& c : manufacturers){
        rawManufacturers.emplace_back(c.second);
    }

    if(AmmoHelper::writeAllManufacturers(fullPath, rawManufacturers)){
        log_info("Saved manufacturers");
        return true;
    }
    else{
        log_error("Could not save manufacturers");
        return false;
    }
}

// MARK: Read
bool AmmoTracker::readAllAmmo(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    // Clear all ammo to not add amount to existing ammo
    removeAllAmmoFromStockpile();

	const std::filesystem::path workingDirectory{saveDirectory};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
			TrackedAmmo ammoBuf {AmmoHelper::readTrackedAmmo(dirEntry.path().string())};

            // Attempt adding to stockpile
            if(!addAmmoToStockpile(ammoBuf))
                log_warn("Ammo object already exists from file [" + dirEntry.path().string() + ']');
		}
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != CARTRIDGES_FILENAME && dirEntry.path().filename().string() != MANUFACTURERS_FILENAME){  // Ignore the cartridges file
                log_error("Failed to create Ammo object from file [" + dirEntry.path().string() + "]. What: " + std::string{e.what()});
            }
		}
	}

	return true;
}
bool AmmoTracker::readCartridges(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += CARTRIDGES_FILENAME;

    StringVector cartridgeNames;
    try{
        AmmoHelper::readCartridges(fullPath, cartridgeNames);
    }
    catch(std::exception& e){
        log_error("Error reading cartridges. What: " + std::string{e.what()});
    }

    for(const auto& s : cartridgeNames){
        if(!addCartridge(s))
            log_warn("Cartridge named [" + s + "] already exists.");
    }

    return true;
}
bool AmmoTracker::readManufacturers(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += MANUFACTURERS_FILENAME;

    StringVector rawManufacturers;
    try{
        AmmoHelper::readManufacturers(fullPath, rawManufacturers);
    }
    catch(std::exception& e){
        log_error("Error reading manufacturers. What: " + std::string{e.what()});
    }

    for(const auto& s : rawManufacturers){
        if(!addManufacturer(s))
            log_warn("Manufacturer named [" + s + "] already exists.");
    }

    return true;
}


bool AmmoTracker::setDirectory(std::string path) {
    path = LAS::TextManip::ensureSlash(path);

    if(std::filesystem::exists(path)){
        saveDirectory = path;
        return true;
    }
    else
        return false;
}
std::string AmmoTracker::getDirectory() const{
    return saveDirectory;
}

// MARK: AMMO HELPER
std::string AmmoHelper::makeFileName (std::string directory, const AmmoType& ammo){
    // Create JSON file name
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




// MARK: Write
bool AmmoHelper::writeTrackedAmmo(std::string directory, const TrackedAmmo& ammo){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;

    // Make JSON
    json j = ammo;

    // Write to file
    std::ofstream file{makeFileName(directory, ammo.ammoType)};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
bool AmmoHelper::writeAllCartridges(std::string path, const StringVector& cartridges){
    using LAS::json;

    if(cartridges.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& cartridge : cartridges) 
        entries.emplace_back(cartridge);
    
    j["cartridges"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
bool AmmoHelper::writeAllManufacturers(std::string path, const StringVector& manufacturers){
    using LAS::json;

    if(manufacturers.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& manufacturer : manufacturers) 
        entries.emplace_back(manufacturer);
    
    j["manufacturers"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}

// MARK: Read
TrackedAmmo AmmoHelper::readTrackedAmmo(const std::string& path){
    using LAS::json;

    if(!std::filesystem::exists(path))
        throw std::filesystem::filesystem_error::runtime_error{"File does not exist"};

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    return TrackedAmmo{j.template get<ShooterCentral::TrackedAmmo>()};
}

bool AmmoHelper::readCartridges (std::string path, StringVector& cartridgeNames){
    using LAS::json;

    if(!std::filesystem::exists(path))
        return false;

    if(!cartridgeNames.empty())
        cartridgeNames.erase(cartridgeNames.begin(), cartridgeNames.end());

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    json cartridges;
	j.at("cartridges").get_to(cartridges);

	for (auto& elment : cartridges.items()) {
		json obj = elment.value();
        std::string nameBuf;

		obj.get_to(nameBuf);

        cartridgeNames.emplace_back(nameBuf);
	}

    return true;
}
bool AmmoHelper::readManufacturers (std::string path, StringVector& manufacturers){
    using LAS::json;

    if(!std::filesystem::exists(path))
        return false;

    if(!manufacturers.empty())
        manufacturers.erase(manufacturers.begin(), manufacturers.end());

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    json manufacturersJson;
	j.at("manufacturers").get_to(manufacturersJson);

	for (auto& elment : manufacturersJson.items()) {
		json obj = elment.value();
        std::string nameBuf;

		obj.get_to(nameBuf);

        manufacturers.emplace_back(nameBuf);
	}

    return true;
}
