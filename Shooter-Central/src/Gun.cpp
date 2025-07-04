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
WeaponType Gun::getWeaponType() const{
    return weaponType;
}
Cartridge Gun::getCartridge () const{
    return cartridge;
}
bool Gun::isActive() const {
    return m_isActive;
}
bool Gun::operator==(const Gun& other) const{
    if(this->name == other.name && this->weaponType == other.weaponType && this->cartridge == other.cartridge)
        return true;
    else
        return false;
}
bool Gun::operator<(const Gun& other) const{
    return std::tuple{getWeaponType().getName(), getCartridge().getName(), getName()} < std::tuple{other.getWeaponType().getName(), other.getCartridge().getName(), other.getName()};
}


GunAndAmmo::GunAndAmmo(std::shared_ptr<const Gun> setGun) : gun {setGun} {

}
GunAndAmmo::~GunAndAmmo(){

}
bool GunAndAmmo::addAmmoUsed(const AmountOfAmmo& ammo) {
    // Check if ammo already in list then add
    for(auto& entry : ammoUsedList){
        if(ammo.ammo == entry.ammo){
            entry.amount += ammo.amount;
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
bool GunAndAmmo::hasUsedAmmo(const Ammo& ammo) const {
    for(const auto& entry : ammoUsedList){
        if(ammo == entry.ammo)
            return true;
    }
    
    return false;
}
int GunAndAmmo::totalAmmoUsed() const {
    return nextIndex;
}
const Gun& GunAndAmmo::getGun() const {
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
