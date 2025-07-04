#include "Model.h"

using namespace ShooterCentral;

Event::Event(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate)
    :   eventMetadata {setLocation, setEventType, setNotes, setDate} {

}
Event::~Event(){

}
bool Event::addGun(const GunAndAmmo& gun) {
    for(auto& entry : gunsUsedList){
        if(!entry)
            continue;
        if(gun.getGun() == entry.getGun())
            return false;
    }

    // If not already used, add new entry
    if(nextIndex < MAX_NUM_GUNS){
        gunsUsedList[nextIndex] = gun;
        ++nextIndex;
        return true;
    }

    return false;
}
bool Event::hasUsedGun(const Gun& gun) const {
    for(const auto& entry : gunsUsedList){
        if(entry.getGun() == gun)
            return true;
    }
    
    return false;
}
int Event::totalGunsUsed() const {
    return nextIndex;
}
std::string Event::getNotes() const{
    return eventMetadata.getNotes();
}
EventType Event::getEventType() const{
    return eventMetadata.getEventType();
}
Location Event::getLocation() const{
    return eventMetadata.getLocation();
}
const ymd& Event::getDate() const{
    return eventMetadata.getDate();
}
timepoint Event::getTimepoint()  const{
    return eventMetadata.getTimepoint();
}
std::string Event::printDate() const{
    return std::format("{:%Od %b %Y}", eventMetadata.getDate());
}
std::array<GunAndAmmo, Event::MAX_NUM_GUNS>::const_iterator Event::cbegin() const{
    return gunsUsedList.cbegin();
}
std::array<GunAndAmmo, Event::MAX_NUM_GUNS>::const_iterator Event::cend() const {
    if(nextIndex > 0)
        return gunsUsedList.cbegin() + nextIndex;
    else
        return gunsUsedList.cbegin();
}
bool Event::operator==(const Event& other) const{
    if(this->eventMetadata.getDate() != other.getDate())
        return false;

    if(this->eventMetadata.getEventType() != other.getEventType())
        return false;

    if(this->eventMetadata.getLocation() != other.getLocation())
        return false;
    
    return true;
}



bool Event::operator<(const Event& other) const{
    // This orders with recent events first
    return std::tuple{other.getDate(), other.getEventType().getName(), other.getLocation().getName()} < std::tuple{eventMetadata.getDate(), eventMetadata.getEventType().getName(), eventMetadata.getLocation().getName()};
}


AssociatedAmmo::AssociatedAmmo(std::shared_ptr<const AmountOfAmmo> setAmountOfAmmo) : amountOfAmmo {setAmountOfAmmo} {

}
AssociatedAmmo::~AssociatedAmmo() {

}
bool AssociatedAmmo::addGun(std::shared_ptr<const Gun> gun) {
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
std::map<Gun, std::shared_ptr<const Gun>>::const_iterator AssociatedAmmo::cbegin() const {
    return gunsAssociated.cbegin();
}
std::map<Gun, std::shared_ptr<const Gun>>::const_iterator AssociatedAmmo::cend() const {
    return gunsAssociated.cend();
}
 


AssociatedGun::AssociatedGun(std::shared_ptr<const Gun> setGun) : gun {setGun} {

}
AssociatedGun::~AssociatedGun(){

}
int AssociatedGun::getRoundCount() const {
    return totalRoundCount;
}
bool AssociatedGun::addAmmoUsed(const AmountOfAmmo& addAmmo) {
    if(ammoUsedList.contains(addAmmo.ammo)){
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
