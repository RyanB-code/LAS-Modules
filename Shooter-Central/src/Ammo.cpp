#include "Ammo.h"

using namespace ShooterCentral;

// MARK: AMMO TYPE
bool AmmoType::operator==(const AmmoType& other) const {
    if(this->name == other.name && this->manufacturer == other.manufacturer && this->cartridge == other.cartridge && this->grainWeight == other.grainWeight)
        return true;
    else
        return false;
}

// MARK: AMMO TRACKER
AmmoTracker::AmmoTracker(LAS::Logging::LoggerPtr setLogger): logger { setLogger }
{

}
AmmoTracker::~AmmoTracker(){
    
}

void ShooterCentral::to_json (LAS::json& j, const AmmoType& ammoType){
    j = LAS::json {
        { "name",           ammoType.name },
        { "manufacturer",   ammoType.manufacturer },
        { "cartridge",      ammoType.cartridge },
        { "grain",          int{ammoType.grainWeight}}
    };
}
void ShooterCentral::from_json(const LAS::json& j, AmmoType& ammoType){
    j.at("name").get_to(ammoType.name);
    j.at("manufacturer").get_to(ammoType.manufacturer);
    j.at("cartridge").get_to(ammoType.cartridge);
    j.at("grain").get_to(ammoType.grainWeight);
}
void ShooterCentral::to_json (LAS::json& j, const TrackedAmmo& ammo){
    j = LAS::json {
        { "name",           ammo.ammoType.name },
        { "manufacturer",   ammo.ammoType.manufacturer },
        { "cartridge",      ammo.ammoType.cartridge },
        { "grain",          int{ammo.ammoType.grainWeight}},
        { "amountOnHand",   ammo.amount }
    };
}
void ShooterCentral::from_json(const LAS::json& j, TrackedAmmo& ammo){
    j.at("name").get_to(ammo.ammoType.name);
    j.at("manufacturer").get_to(ammo.ammoType.manufacturer);
    j.at("cartridge").get_to(ammo.ammoType.cartridge);
    j.at("grain").get_to(ammo.ammoType.grainWeight);
    j.at("amountOnHand").get_to(ammo.amount);
}

// MARK: STOCKPILE
bool AmmoTracker::addAmmoToStockpile (uint64_t amount, const AmmoType& ammoType){
    if(ammoStockpile.contains(ammoType)){
        ammoStockpile.at(ammoType)->amount += amount;
        return true;
    }
    
    TrackedAmmoPtr ammoBuf { std::make_shared<TrackedAmmo>(TrackedAmmo{ammoType, amount}) };
    ammoStockpile.try_emplace(ammoType, ammoBuf);

    if(ammoStockpile.contains(ammoType)){
        addCartridge(ammoType.cartridge);           // Add to cartridges, does not matter return
        return true;
    }
    else
        return false;
}
bool AmmoTracker::removeAmmoFromStockpile (uint64_t amountUsed, const AmmoType& ammoType){
    if(!ammoStockpile.contains(ammoType))
        return true;
    
    auto& ammo { ammoStockpile.at(ammoType) };

    if(ammo->amount - amountUsed >= 100000)
        return false;
    else
        ammo->amount -= amountUsed;

    return true;
}
void AmmoTracker::removeAllAmmoFromStockpile  (){
    if(ammoStockpile.empty())
        return;

    ammoStockpile.clear();
}

// MARK: GET INFO
void AmmoTracker::getAllAmmoNames(StringVector& names) const{
    if(!names.empty())
        names.erase(names.begin(), names.end());


    for(const auto& ammo : ammoStockpile){
        names.push_back(ammo.second->ammoType.name);
    }
}
void AmmoTracker::getAllAmmo (std::vector<TrackedAmmo>& list)   const{
     if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& ammo : ammoStockpile){
        list.push_back(*ammo.second);
    }
}

void AmmoTracker::getAllCartridgeNames(StringVector& names) const{
    if(!names.empty())
        names.erase(names.begin(), names.end());


    for(const auto& pair : cartridges){
        names.push_back(pair.second);
    }
}
void AmmoTracker::getAmmoCountByCartridge (std::vector<std::pair<std::string, uint64_t>>& count) const{
    if(!count.empty())
        count.erase(count.begin(), count.end());

    StringVector namesList;
    getAllCartridgeNames(namesList); // Get all the names of cartridges

    // For every cartridge, add the amounts together
    for(const auto& name : namesList){
        int amountBuf { 0 };
        std::vector<TrackedAmmo> sameCartridgeList;
        getAllAmmoByCartridge(sameCartridgeList, name);

        for(const auto& ammo : sameCartridgeList)
            amountBuf += ammo.amount;

        count.push_back(std::pair{name, amountBuf});           // Add to list
    }
}
void AmmoTracker::getAllAmmoByCartridge(std::vector<TrackedAmmo>& list, const std::string& cartridgeName)   const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& pair : ammoStockpile){
        if(pair.second->ammoType.cartridge == cartridgeName)
            list.push_back(*pair.second);
    }
}
bool AmmoTracker::addCartridge (const std::string& cartridge){
    if(cartridges.contains(cartridge))
        return false;

    cartridges.try_emplace(cartridge, cartridge);
    return cartridges.contains(cartridge);
}

// MARK: R/W AMMO
bool AmmoTracker::writeAllAmmo() const{
    using LAS::json;

    if(ammoStockpile.empty())
        return true;

    if(!std::filesystem::exists(saveDirectory)){
        logger->log("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Ammo objects.", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }


	for(const auto& pair : ammoStockpile) {
        const auto& ammo = *pair.second;

        try{
            if(!AmmoHelper::writeTrackedAmmo(saveDirectory, ammo)) 
                logger->log("Directory [" + saveDirectory + "] was not found. Ammo [" + ammo.ammoType.name + "] was not saved.", LAS::Logging::Tags{"ERROR", "SC"});
        }
        catch(json::exception& e){
            logger->log("Error creating JSON object. Code " + std::string{e.what()} + ". What: " + std::string{e.what()}, LAS::Logging::Tags{"ERROR", "SC"});
        }
	}

    return true;
}
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
            if(!addAmmoToStockpile(ammoBuf.amount, ammoBuf.ammoType))
                logger->log("Ammo object already exists from file [" + dirEntry.path().string() + ']', LAS::Logging::Tags{"ROUTINE", "SC"});
		}
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != CARTRIDGES_FILENAME)  // Ignore the cartridges file
			    logger->log("Failed to create Ammo object from file [" + dirEntry.path().string() + ']', LAS::Logging::Tags{"ERROR", "SC"});
                logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
		}
	}

	return true;
}
// MARK: R/W CARTRIDGE
bool AmmoTracker::writeAllCartridges() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += CARTRIDGES_FILENAME;

    std::vector<std::string> rawCartridges;

    for(auto& c : cartridges){
        rawCartridges.push_back(c.second);
    }

    return AmmoHelper::writeAllCartridges(fullPath, rawCartridges);
}
bool AmmoTracker::readCartridges(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += CARTRIDGES_FILENAME;

    StringVector cartridgeNames;
    AmmoHelper::readCartridges(fullPath, cartridgeNames);

    for(const auto& s : cartridgeNames){
        if(!addCartridge(s))
            logger->log("Cartridge named [" + s + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
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
bool AmmoHelper::writeTrackedAmmo(std::string directory, const TrackedAmmo& ammo){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;
    

    // Make JSON
    json j = ammo;

    // Create JSON file name
    std::string fileName;
    for(const auto& c : ammo.ammoType.name){     // Remove spaces and make lowercase
        if(isalpha(c))
            fileName += tolower(c);
        else if(c == ' ' || c == '\t')
            fileName += '-';
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
TrackedAmmo AmmoHelper::readTrackedAmmo(const std::string& path){
    using LAS::json;

    if(!std::filesystem::exists(path))
        throw std::filesystem::filesystem_error::runtime_error{"File does not exist"};

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    return TrackedAmmo{j.template get<ShooterCentral::TrackedAmmo>()};
}
// MARK: R/W CARTRIDGES
bool AmmoHelper::writeAllCartridges(std::string path, const std::vector<std::string>& cartridges){
    using LAS::json;

    if(cartridges.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& cartridge : cartridges) 
        entries.push_back(cartridge);
    
    j["cartridges"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
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

        cartridgeNames.push_back(nameBuf);
	}

    return true;
}
