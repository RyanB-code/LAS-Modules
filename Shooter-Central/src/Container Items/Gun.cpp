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
bool WeaponType::operator<(const WeaponType& other) const{
    return name < other.getName();
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
void ShooterCentral::to_json(LAS::json& j, const GunMetadata& gun){
    using LAS::json;

    j = LAS::json{
        { "name",           gun.name        },
        { "weaponType",     gun.weaponType  },
        { "cartridge",      gun.cartridge   },
        { "isActive",       gun.isActive    }
    };
}


GunAndAmmo::GunAndAmmo(const GunMetadata& setGun) : gun {setGun} {
    ammoUsedList.reserve(5);
}
GunAndAmmo::~GunAndAmmo(){

}
bool GunAndAmmo::addAmmoUsed(const AmountOfAmmo& ammo) {
    // Check if ammo already in list then add
    for(auto& entry : ammoUsedList){
        if(ammo.getAmmoInfo() == entry.getAmmoInfo()){
            entry.addAmount(ammo.getAmount());
            totalRoundCount += ammo.getAmount();
            return true;
        }
    }

    ammoUsedList.emplace_back(ammo);
    totalRoundCount += ammo.getAmount();

    return true;
}
bool GunAndAmmo::hasUsedAmmo(const AmmoMetadata& ammo) const {
    for(const auto& entry : ammoUsedList){
        if(ammo == entry.getAmmoInfo())
            return true;
    }
    
    return false;
}
int GunAndAmmo::totalAmmoUsed() const {
    return ammoUsedList.size();
}
int GunAndAmmo::totalRoundsShot() const{
    return totalRoundCount;
}
const GunMetadata& GunAndAmmo::getGunInfo() const {
    return gun;
}
const std::vector<AmountOfAmmo>& GunAndAmmo::getAmmoUsedList() const{
    return ammoUsedList;
}

