#include "AssociatedItems.h"

using namespace ShooterCentral;

AssociatedAmmo::AssociatedAmmo(AmountOfAmmo setAmountOfAmmo) : amountOfAmmo {setAmountOfAmmo} {

}
AssociatedAmmo::~AssociatedAmmo() {

}
bool AssociatedAmmo::addGun(std::shared_ptr<const GunMetadata> gun) {
    if(gunsAssociated.contains(*gun))
        return false;

    return gunsAssociated.try_emplace(*gun, gun).second;
}
bool AssociatedAmmo::removeGun(const GunMetadata& gun){
    if(!gunsAssociated.contains(gun))
        return true;

    gunsAssociated.erase(gun);
    return !gunsAssociated.contains(gun); // Return the inverse of contains()
}
bool AssociatedAmmo::hasGun(const GunMetadata& gun) const {
    return gunsAssociated.contains(gun);
}
int AssociatedAmmo::totalGuns() const {
    return gunsAssociated.size();
}
const AmountOfAmmo& AssociatedAmmo::getAmountOfAmmo() const {
    return amountOfAmmo;
}
const std::map<GunMetadata, std::shared_ptr<const GunMetadata>>& AssociatedAmmo::getGunsUsed() const {
    return gunsAssociated;
}


AssociatedGun::AssociatedGun(const GunMetadata& setGun) : gun {setGun} {

}
AssociatedGun::~AssociatedGun(){

}
const GunMetadata& AssociatedGun::getGunInfo() const {
    return gun;
}
int AssociatedGun::getRoundCount() const {
    return totalRoundCount;
}
bool AssociatedGun::addAmmoUsed(const AmountOfAmmo& addAmmo) {
    const AmmoMetadata& ammoInfo {addAmmo.getAmmoInfo()};

    if(ammoUsedList.contains(ammoInfo)){
        ammoUsedList.at(ammoInfo).addAmount(addAmmo.getAmount());
        totalRoundCount += addAmmo.getAmount();
        return true;
    }

    if(ammoUsedList.try_emplace(ammoInfo, addAmmo).second){
        totalRoundCount += addAmmo.getAmount();
        return true;
    }
     
    return false;
}
bool AssociatedGun::removeAmmoUsed(const AmmoMetadata& ammo){
    if(!ammoUsedList.contains(ammo))
        return true;

    ammoUsedList.erase(ammo);
    return !ammoUsedList.contains(ammo); // Return the inverse of contains()
}
bool AssociatedGun::hasUsedAmmo(const AmmoMetadata& ammo) const {
    return ammoUsedList.contains(ammo);
}
bool AssociatedGun::addEvent(std::shared_ptr<Event> event) {
    if(eventsUsed.contains(event->getInfo()))
        return false;

    if(!eventsUsed.try_emplace(event->getInfo(), event).second)
        return false;

    // Go through the event and add all the ammo used for the gun
    bool gunFound { false };
    for(const GunAndAmmo& gunAndAmmo : event->getGunsUsed()){
        // Verify the GunAndAmmo is pointing to a valid gun object
        if(gunAndAmmo.getGunInfo() == gun){
            gunFound = true;

            // Iterate over GunAndAmmo and add to ammoUsedList
            for(auto& amountOfAmmo : gunAndAmmo.getAmmoUsedList())
                addAmmoUsed(amountOfAmmo);

            break;
        }
    }

    if(gunFound)
        return true;
    else{
        // If the gun was not found in the Event's used gun list, then it should not be associated with this gun
        // Therefore, remove event from the list and exit
        removeEvent(event->getInfo());
        return false;
    }
}
bool AssociatedGun::removeEvent(const EventMetadata& data) {
   if(!eventsUsed.contains(data))
        return true;

    eventsUsed.erase(data);
    return !eventsUsed.contains(data); // Return the inverse of contains()
}
bool AssociatedGun::wasUsedInEvent(const EventMetadata& data){
    return eventsUsed.contains(data);
}
int AssociatedGun::totalEventsUsed() const {
    return eventsUsed.size();
}
int AssociatedGun::totalAmmoTypesUsed() const {
    return ammoUsedList.size();
}
const std::map<AmmoMetadata, AmountOfAmmo>& AssociatedGun::getAmmoUsed()   const{
    return ammoUsedList;
}
const std::map<EventMetadata, std::shared_ptr<const Event>>& AssociatedGun::getEventsUsed() const{
    return eventsUsed;
}


