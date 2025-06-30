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


bool Ammo::operator==(const Ammo& other) const {
    if(name == other.name && manufacturer == other.manufacturer && cartridge == other.cartridge && grainWeight == other.grainWeight)
        return true;
    else
        return false;
}
bool Ammo::operator<(const Ammo& other) const{
   std::tuple<std::string, std::string, std::string, int> lhs {cartridge.getName(), manufacturer.getName(), name, grainWeight};
   std::tuple<std::string, std::string, std::string, int> rhs {other.cartridge.getName(), other.manufacturer.getName(), other.name, other.grainWeight};

   return lhs < rhs;
}
