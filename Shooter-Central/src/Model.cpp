#include "Model.h"

using namespace ShooterCentral;

AssociatedAmmo::AssociatedAmmo(std::shared_ptr<AmountOfAmmo> setAmountOfAmmo) : amountOfAmmo {setAmountOfAmmo} {

}
AssociatedAmmo::~AssociatedAmmo() {

}
bool AssociatedAmmo::addGun(std::shared_ptr<Gun> gun) {
    if(gunsAssociated.contains(*gun))
        return true;

    return gunsAssociated.try_emplace(*gun, gun).second;
}
bool AssociatedAmmo::removeGun(const Gun& gun){
    if(!gunsAssociated.contains(gun))
        return true;

    gunsAssociated.erase(gun);
    return !gunsAssociated.contains(gun); // Return the inverse of contains()
}
bool AssociatedAmmo::hasGun(const Gun& gun) const {
    return gunsAssociated.contains(gun);
}
int AssociatedAmmo::totalGuns() const {
    return gunsAssociated.size();
}
const AmountOfAmmo& AssociatedAmmo::getAmountOfAmmo() const {
    return *amountOfAmmo;
}
std::map<Gun, std::shared_ptr<Gun>>::const_iterator AssociatedAmmo::begin() const {
    return gunsAssociated.cbegin();
}
std::map<Gun, std::shared_ptr<Gun>>::const_iterator AssociatedAmmo::end() const {
    return gunsAssociated.cend();
}
 

AssociatedGun::AssociatedGun(std::shared_ptr<Gun> setGun) : gun {setGun} {

}
AssociatedGun::~AssociatedGun(){

}
int AssociatedGun::getRoundCount() const {
    return totalRoundCount;
}
bool AssociatedGun::addAmmoUsed(const AmountOfAmmo& addAmmo) {
    if(ammoUsedList.contains(addAmmo.ammo){
        ammoUsedList.at(addAmmo.ammo).amount += addAmmo.amount;
        totalRoundCount += addAmmo.amount;
        return true;
    }

    if(ammoUsedList.try_emplace(addAmmo.ammo, addAmmo).second){
        totalRoundCount += addAmmo.amount;
        return true;
    }
     
    return false;
}
bool AssociatedGun::removeAmmoUsed(const Ammo& ammo){
    if(!ammoUsedList.contains(ammo))
        return true;

    ammoUsedList.erase(ammo);
    return !ammoUsedList.contains(ammo); // Return the inverse of contains()
}
bool AssociatedGun::hasUsedAmmo(const Ammo& ammo) const {
    return ammoUsedList.contains(ammo);
}
