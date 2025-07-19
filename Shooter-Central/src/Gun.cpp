#include "Gun.h"

using namespace ShooterCentral;


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



bool GunMetadata::operator==(const GunMetadata& other) const{
    if(this->name == other.name && this->weaponType == other.weaponType && this->cartridge == other.cartridge)
        return true;
    else
        return false;
}
bool GunMetadata::operator<(const GunMetadata& other) const{
    return std::tuple{std::string{weaponType}, std::string{cartridge}, name} < std::tuple{std::string{other.weaponType}, std::string{other.cartridge}, other.name}; 
}
void ShooterCentral::to_json(LAS::json& j, const GunMetadata& gun){
    using LAS::json;

    j = LAS::json{
        { "name",           gun.name                },
        { "weaponType",     gun.weaponType.getName()},
        { "cartridge",      gun.cartridge.getName() },
        { "isActive",       gun.isActive           }
    };
}
void ShooterCentral::from_json(const LAS::json& j, GunMetadata& gun){
    using LAS::json;

    std::string nameBuf, weaponTypeBuf, cartBuf; 
    bool isActiveBuf { false };

    j.at("name").get_to(nameBuf);
    j.at("weaponType").get_to(weaponTypeBuf);
    j.at("cartridge").get_to(cartBuf);

    j["isActive"].get_to(isActiveBuf);

    gun = GunMetadata {nameBuf, WeaponType {weaponTypeBuf}, Cartridge {cartBuf}, isActiveBuf };
}


GunAndAmmo::GunAndAmmo(std::shared_ptr<const GunMetadata> setGun) : gun {setGun} {

}
GunAndAmmo::~GunAndAmmo(){

}
bool GunAndAmmo::addAmmoUsed(const AmountOfAmmo& ammo) {
    // Check if ammo already in list then add
    for(auto& entry : ammoUsedList){
        if(!entry)
            continue;
        if(ammo.getAmmo() == entry.getAmmo()){
            entry.addAmount(ammo.getAmount());
            return true;
        }
    }

    // If not already used, add new entry
    if(nextIndex < MAX_NUM_AMMO_USED){
        ammoUsedList[nextIndex] = ammo;
        ++nextIndex;
        return true;
    }

    return false;
}
bool GunAndAmmo::hasUsedAmmo(const AmmoMetadata& ammo) const {
    for(const auto& entry : ammoUsedList){
        if(ammo == entry.getAmmo())
            return true;
    }
    
    return false;
}
int GunAndAmmo::totalAmmoUsed() const {
    return nextIndex;
}
const GunMetadata& GunAndAmmo::getGun() const {
    return *gun;
}
GunAndAmmo::operator bool() const{
    if(gun)
        return true;
    else
        return false;
}
std::array<AmountOfAmmo, GunAndAmmo::MAX_NUM_AMMO_USED>::const_iterator GunAndAmmo::cbegin() const{
    return ammoUsedList.cbegin();
}
std::array<AmountOfAmmo, GunAndAmmo::MAX_NUM_AMMO_USED>::const_iterator GunAndAmmo::cend() const {
     if(nextIndex > 0)
        return ammoUsedList.cbegin() + nextIndex;
    else
        return ammoUsedList.cbegin();
}
