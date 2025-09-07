#include "Ammo.h"

using namespace ShooterCentral;


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
bool Manufacturer::operator< (const Manufacturer& other) const{
    return this->name < other.getName();
}


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
bool Cartridge::operator< (const Cartridge& other) const{
    return this->name < other.getName();
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
void ShooterCentral::to_json (LAS::json& j, const AmmoMetadata& data){
    j = LAS::json {
        { "name",           data.name },
        { "manufacturer",   data.manufacturer.getName() },
        { "cartridge",      data.cartridge.getName() },
        { "grain",          data.grainWeight},
        { "isActive",       data.isActive}
    };
}


AmountOfAmmo::AmountOfAmmo(const AmmoMetadata& setAmmo, int setAmount) : ammo { setAmmo }, amount {setAmount} {

}
AmountOfAmmo::~AmountOfAmmo() {

}
int AmountOfAmmo::getAmount() const {
    return amount;
}
void AmountOfAmmo::addAmount(int add){
    amount += add;
}
const AmmoMetadata& AmountOfAmmo::getAmmoInfo() const {
    return ammo;
}
void ShooterCentral::to_json(LAS::json& j, const AmountOfAmmo& data){
    j = LAS::json {
        { "ammoInfo",   data.getAmmoInfo() },
        { "amount",     data.getAmount() }
    };
}
