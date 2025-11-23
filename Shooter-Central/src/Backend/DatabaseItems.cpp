#include "DatabaseItems.h"

using namespace ShooterCentral;


Manufacturer::Manufacturer(const char* set)  {
    if(!set)
        return;

    if(!setName(set))
        throw std::invalid_argument("Manufacturer::Manufacturer(const char*), expected const char* but received nullptr");

}
const char* Manufacturer::getName() const {
    return name;
}
bool Manufacturer::setName(const char* set){
    if(!set)
       return false; 

    std::strncpy(name, set, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';            // Manually null-terminate

    return true;
}
bool Manufacturer::operator==(const Manufacturer& other) const{
    if(!other.getName())
        throw std::invalid_argument("Manufacturer::operator==(const Manufacturer& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("Manufacturer::operator==(const Manufacturer& other), this->name == nullptr. Expected char*");

    if(std::strcmp(other.getName(), name) == 0)
        return true;

    return false;
}
bool Manufacturer::operator< (const Manufacturer& other) const{
    if(!other.getName())
        throw std::invalid_argument("Manufacturer::operator<(const Manufacturer& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("Manufacturer::operator<(const Manufacturer& other), this->name == nullptr. Expected char*");


    if( std::strcmp(name, other.getName()) < 0)
        return true;

    return false;
}


Cartridge::Cartridge(const char* set)  {
    if(!set)
        return;

    if(!setName(set))
        throw std::invalid_argument("Cartridge::Cartridge(const char*), expected const char* but received nullptr");

}
const char* Cartridge::getName() const {
    return name;
}
bool Cartridge::setName(const char* set){
    if(!set)
       return false; 

    std::strncpy(name, set, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';            // Manually null-terminate

    return true;
}
bool Cartridge::operator==(const Cartridge& other) const{
    if(!other.getName())
        throw std::invalid_argument("Cartridge::operator==(const Cartridge& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("Cartridge::operator==(const Cartridge& other), this->name == nullptr. Expected char*");

    if(std::strcmp(other.getName(), name) == 0)
        return true;

    return false;
}
bool Cartridge::operator< (const Cartridge& other) const{
    if(!other.getName())
        throw std::invalid_argument("Cartridge::operator<(const Cartridge& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("Cartridge::operator<(const Cartridge& other), this->name == nullptr. Expected char*");


    if( std::strcmp(name, other.getName()) < 0)
        return true;

    return false;
}


WeaponType::WeaponType(const char* set)  {
    if(!set)
        return;

    if(!setName(set))
        throw std::invalid_argument("WeaponType::WeaponType(const char*), expected const char* but received nullptr");

}
const char* WeaponType::getName() const {
    return name;
}
bool WeaponType::setName(const char* set){
    if(!set)
       return false; 

    std::strncpy(name, set, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';            // Manually null-terminate

    return true;
}
bool WeaponType::operator==(const WeaponType& other) const{
    if(!other.getName())
        throw std::invalid_argument("WeaponType::operator==(const WeaponType& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("WeaponType::operator==(const WeaponType& other), this->name == nullptr. Expected char*");

    if(std::strcmp(other.getName(), name) == 0)
        return true;

    return false;
}
bool WeaponType::operator< (const WeaponType& other) const{
    if(!other.getName())
        throw std::invalid_argument("WeaponType::operator<(const WeaponType& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("WeaponType::operator<(const WeaponType& other), this->name == nullptr. Expected char*");


    if( std::strcmp(name, other.getName()) < 0)
        return true;

    return false;
}


Location::Location(const char* set)  {
    if(!set)
        return;

    if(!setName(set))
        throw std::invalid_argument("Location::Location(const char*), expected const char* but received nullptr");

}
const char* Location::getName() const {
    return name;
}
bool Location::setName(const char* set){
    if(!set)
       return false; 

    std::strncpy(name, set, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';            // Manually null-terminate

    return true;
}
bool Location::operator==(const Location& other) const{
    if(!other.getName())
        throw std::invalid_argument("Location::operator==(const Location& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("Location::operator==(const Location& other), this->name == nullptr. Expected char*");

    if(std::strcmp(other.getName(), name) == 0)
        return true;

    return false;
}
bool Location::operator< (const Location& other) const{
    if(!other.getName())
        throw std::invalid_argument("Location::operator<(const Location& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("Location::operator<(const Location& other), this->name == nullptr. Expected char*");


    if( std::strcmp(name, other.getName()) < 0)
        return true;

    return false;
}


ShootingEventType::ShootingEventType(const char* set)  {
    if(!set)
        return;

    if(!setName(set))
        throw std::invalid_argument("ShootingEventType::ShootingEventType(const char*), expected const char* but received nullptr");

}
const char* ShootingEventType::getName() const {
    return name;
}
bool ShootingEventType::setName(const char* set){
    if(!set)
       return false; 

    std::strncpy(name, set, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';            // Manually null-terminate

    return true;
}
bool ShootingEventType::operator==(const ShootingEventType& other) const{
    if(!other.getName())
        throw std::invalid_argument("ShootingEventType::operator==(const ShootingEventType& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("ShootingEventType::operator==(const ShootingEventType& other), this->name == nullptr. Expected char*");

    if(std::strcmp(other.getName(), name) == 0)
        return true;

    return false;
}
bool ShootingEventType::operator< (const ShootingEventType& other) const{
    if(!other.getName())
        throw std::invalid_argument("ShootingEventType::operator<(const ShootingEventType& other), other.getName() == nullptr. Expected char*");
            
    if(!name)
        throw std::invalid_argument("ShootingEventType::operator<(const ShootingEventType& other), this->name == nullptr. Expected char*");


    if( std::strcmp(name, other.getName()) < 0)
        return true;

    return false;
}




bool ShootingEventMetadata::operator==(const ShootingEventMetadata& other) const{
    if(date != other.date)
        return false;

    if(eventType != other.eventType)
        return false;

    if(location != other.location)
        return false;

    return true;
}
bool ShootingEventMetadata::operator<(const ShootingEventMetadata& other) const{
    return std::tuple(other.date, other.eventType, other.location) < std::tuple(date, eventType, location);
}




bool AmmoMetadata::operator==(const AmmoMetadata& other) const {
    if(name == other.name && manufacturer == other.manufacturer && cartridge == other.cartridge && grainWeight == other.grainWeight)
        return true;
    else
        return false;
}
bool AmmoMetadata::operator<(const AmmoMetadata& other) const{
    std::tuple lhs {cartridge, manufacturer, name, grainWeight};
    std::tuple rhs {other.cartridge, other.manufacturer, other.name, other.grainWeight};

    return lhs < rhs;
}



bool GunMetadata::operator==(const GunMetadata& other) const{
    if(this->name == other.name && this->weaponType == other.weaponType && this->cartridge == other.cartridge)
        return true;
    else
        return false;
}
bool GunMetadata::operator<(const GunMetadata& other) const{
    return std::tuple{weaponType, cartridge, name} < std::tuple{other.weaponType, other.cartridge, other.name}; 
}




AmountOfAmmo::AmountOfAmmo(const AmmoMetadata& setAmmo, int setAmount) : ammoInfo { setAmmo }, amount {setAmount} {

}
int AmountOfAmmo::getAmount() const {
    return amount;
}
void AmountOfAmmo::addAmount(int add){
    amount += add;
}
const AmmoMetadata& AmountOfAmmo::getAmmoInfo() const {
    return ammoInfo;
}


StockpileAmmo::StockpileAmmo(const AmountOfAmmo& setAmountOfAmmo) :
    ammoInfo        { setAmountOfAmmo.getAmmoInfo() }, 
    amountOnHand    { setAmountOfAmmo.getAmount() }
{

}
StockpileAmmo::StockpileAmmo(const AmmoMetadata& setAmmoInfo) : 
    ammoInfo { setAmmoInfo }
{

}
int StockpileAmmo::getAmountOnHand() const {
    return amountOnHand;
}
void StockpileAmmo::addAmount      (int amount){
   amountOnHand += amount; 
}
void StockpileAmmo::removeAmount   (int amount){
    amountOnHand += amount;
}
bool StockpileAmmo::addGun(const GunMetadata& gun) {
    if(gunsUsed.contains(gun))
        return false;

    return gunsUsed.emplace(gun).second;
}
bool StockpileAmmo::removeGun(const GunMetadata& gun){
    if(!gunsUsed.contains(gun))
        return true;

    gunsUsed.erase(gun);
    return !gunsUsed.contains(gun); // Return the inverse of contains()
}
bool StockpileAmmo::hasGun(const GunMetadata& gun) const {
    return gunsUsed.contains(gun);
}
int StockpileAmmo::totalGuns() const {
    return gunsUsed.size();
}



GunTrackingAmmoUsed::GunTrackingAmmoUsed(const GunMetadata& setGun) : gun {setGun} {
    ammoUsed.reserve(5);
}
bool GunTrackingAmmoUsed::addAmmoUsed(const AmountOfAmmo& ammo) {
    // Check if ammo already in list then add
    for(auto& entry : ammoUsed){
        if(ammo.getAmmoInfo() == entry.getAmmoInfo()){
            entry.addAmount(ammo.getAmount());
            roundCount += ammo.getAmount();
            return true;
        }
    }

    ammoUsed.emplace_back(ammo);
    roundCount += ammo.getAmount();

    return true;
}
bool GunTrackingAmmoUsed::hasUsedAmmo(const AmmoMetadata& ammo) const {
    for(const auto& entry : ammoUsed){
        if(ammo == entry.getAmmoInfo())
            return true;
    }
    
    return false;
}
int GunTrackingAmmoUsed::totalAmmoUsed() const {
    return ammoUsed.size();
}
int GunTrackingAmmoUsed::totalRoundsShot() const{
    return roundCount;
}
const GunMetadata& GunTrackingAmmoUsed::getGunInfo() const {
    return gun;
}


ArmoryGun::ArmoryGun(const GunMetadata& setGun) : gun {setGun} {

}
const GunMetadata& ArmoryGun::getGunInfo() const {
    return gun;
}
int ArmoryGun::getRoundCount() const {
    return roundCount;
}
bool ArmoryGun::addAmmoUsed(const AmountOfAmmo& addAmmo) {
    const AmmoMetadata& ammoInfo {addAmmo.getAmmoInfo()};

    if(ammoUsed.contains(ammoInfo)){
        ammoUsed.at(ammoInfo).addAmount(addAmmo.getAmount());
        roundCount += addAmmo.getAmount();
        return true;
    }

    if(ammoUsed.try_emplace(ammoInfo, addAmmo).second){
        roundCount += addAmmo.getAmount();
        return true;
    }
     
    return false;
}
/* May not acutally need this so commenting out
 *
        bool ArmoryGun::removeAmmoUsed(const AmmoMetadata& ammo){
            if(!ammoUsed.contains(ammo))
                return true;

            roundcount -= ammoUsed.at(ammo).getAmount();    // Remove from roundcount
            ammoUsedt.erase(ammo);
            return !ammoUsedList.contains(ammo); // Return the inverse of contains()
        }
 *
 *
*/
bool ArmoryGun::hasUsedAmmo(const AmmoMetadata& ammo) const {
    return ammoUsed.contains(ammo);
}
bool ArmoryGun::addEvent(const ShootingEvent& event) {
    if(eventsUsed.contains(event.getInfo()))
        return false;

    if(!eventsUsed.emplace(event.getInfo()).second)
        return false;

    // Go through the event and add all the ammo used for the gun
    bool gunFound { false };
    for(const auto& gunAndAmmo : event.getGunsUsed()){
        // Verify the GunAndAmmo is pointing to a valid gun object
        if(gunAndAmmo.getGunInfo() == gun){
            gunFound = true;
            break;
        }
    }

    if(gunFound)
        return true;
    else{
        // If the gun was not found in the Event's used gun list, then it should not be associated with this gun
        // Therefore, remove event from the list and exit
        eventsUsed.erase(event.getInfo());
        return false;
    }
}
bool ArmoryGun::wasUsedInEvent(const ShootingEventMetadata& data){
    return eventsUsed.contains(data);
}
int ArmoryGun::totalEventsUsed() const {
    return eventsUsed.size();
}
int ArmoryGun::totalAmmoTypesUsed() const {
    return ammoUsed.size();
}




ShootingEvent::ShootingEvent(const ShootingEventMetadata& info) : eventInfo {info} {
    gunsUsed.reserve(5);
}
bool ShootingEvent::addGun(const GunTrackingAmmoUsed& gun) {
    for(const auto& entry : gunsUsed){
        if(gun.getGunInfo() == entry.getGunInfo())
            return false;
    }

    gunsUsed.emplace_back(gun);
    return true;
}
bool ShootingEvent::hasUsedGun(const GunMetadata& gun) const {
    for(const auto& entry : gunsUsed){
        if(entry.getGunInfo() == gun)
            return true;
    }
    
    return false;
}
int ShootingEvent::totalGunsUsed() const {
    return gunsUsed.size();
}
std::string ShootingEvent::printDate() const{
    return std::format("{:%Od %b %Y}", eventInfo.date);
}




// Helping Functions
std::chrono::system_clock::time_point ShooterCentral::stringToTimepoint(const std::string& timeString) {
    std::stringstream ss{ timeString };
    std::tm time{};
    time.tm_isdst = -1;
    ss >> std::get_time(&time, "%Y-%m-%d%n%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&time));
}




