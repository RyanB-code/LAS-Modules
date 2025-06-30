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
