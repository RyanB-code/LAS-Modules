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
bool Event::hasUsedGun(const GunMetadata& gun) const {
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
    return eventMetadata.notes;
}
EventType Event::getEventType() const{
    return eventMetadata.eventType;
}
Location Event::getLocation() const{
    return eventMetadata.location;
}
const ymd& Event::getDate() const{
    return eventMetadata.date;
}
std::string Event::printDate() const{
    return std::format("{:%Od %b %Y}", eventMetadata.date);
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
    if(this->getDate() != other.getDate())
        return false;

    if(this->getEventType() != other.getEventType())
        return false;

    if(this->getLocation() != other.getLocation())
        return false;
    
    return true;
}
bool Event::operator<(const Event& other) const{
    // This orders with recent events first
    return std::tuple{other.getDate(), other.getEventType().getName(), other.getLocation().getName()} < std::tuple{getDate(), getEventType().getName(), getLocation().getName()};
}



AssociatedAmmo::AssociatedAmmo(AmountOfAmmo setAmountOfAmmo) : amountOfAmmo {setAmountOfAmmo} {

}
AssociatedAmmo::~AssociatedAmmo() {

}
AssociatedAmmo::operator bool() const {
    if(amountOfAmmo)
        return true;
    else
        return false;
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
    throwIfInvalid();
    return amountOfAmmo;
}
std::map<GunMetadata, std::shared_ptr<const GunMetadata>>::const_iterator AssociatedAmmo::cbegin() const {
    return gunsAssociated.cbegin();
}
std::map<GunMetadata, std::shared_ptr<const GunMetadata>>::const_iterator AssociatedAmmo::cend() const {
    return gunsAssociated.cend();
}
void AssociatedAmmo::throwIfInvalid() const {
    if(!amountOfAmmo)
        throw std::invalid_argument("Associated ammo cannot be null");
}


AssociatedGun::AssociatedGun(std::shared_ptr<const GunMetadata> setGun) : gun {setGun} {

}
AssociatedGun::~AssociatedGun(){

}
const GunMetadata& AssociatedGun::getGun() const {
    throwIfGunInvalid();

    return *gun;
}
AssociatedGun::operator bool() const{
    if(gun)
        return true;
    else
        return false;
}
int AssociatedGun::getRoundCount() const {
    return totalRoundCount;
}
bool AssociatedGun::addAmmoUsed(const AmountOfAmmo& addAmmo) {
    throwIfGunInvalid();

    if(ammoUsedList.contains(addAmmo.getAmmo())){
        ammoUsedList.at(addAmmo.getAmmo()).addAmount(addAmmo.getAmount());
        totalRoundCount += addAmmo.getAmount();
        return true;
    }

    if(ammoUsedList.try_emplace(addAmmo.getAmmo(), addAmmo).second){
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
bool AssociatedGun::addEvent(std::shared_ptr<const Event> event) {
    throwIfGunInvalid();

    if(!event)
        return false;

    if(eventsUsed.contains(*event))
        return false;

    if(!eventsUsed.try_emplace(*event, event).second)
        return false;

    // Go through the event and add all the ammo used for the gun
    bool gunFound { false };
    for(auto itr { event->cbegin()}; itr != event->cend(); ++itr){
        // Verify the GunAndAmmo is pointing to a valid gun object
        if(!*itr)
            continue;

        const GunAndAmmo& gunAndAmmo { *itr };
        if(gunAndAmmo.getGun() == *gun){
            gunFound = true;
            // Iterate over GunAndAmmo and add to ammoUsedList
            for(auto itr2 {gunAndAmmo.cbegin()}; itr2 != gunAndAmmo.cend(); ++itr2){
                const AmountOfAmmo& amountOfAmmo {*itr2};
                addAmmoUsed(amountOfAmmo);
            }
            break;
        }
    }

    if(gunFound)
        return true;
    else{
        // If the gun was not found in the Event's used gun list, then it should not be associated with this gun
        // Therefore, remove event from the list and exit
        removeEvent(*event);
        return false;
    }
}
bool AssociatedGun::removeEvent(const Event& event) {
   if(!eventsUsed.contains(event))
        return true;

    eventsUsed.erase(event);
    return !eventsUsed.contains(event); // Return the inverse of contains()
}
bool AssociatedGun::wasUsedInEvent(const Event& event){
    return eventsUsed.contains(event);
}
int AssociatedGun::totalEventsUsed() const {
    return eventsUsed.size();
}
void AssociatedGun::throwIfGunInvalid() const {
    if(!gun)
        throw std::invalid_argument("Associated gun cannot be null");
}
std::map<AmmoMetadata, AmountOfAmmo>::const_iterator AssociatedGun::ammoUsed_cbegin() const{
    return ammoUsedList.cbegin();
}
std::map<AmmoMetadata, AmountOfAmmo>::const_iterator AssociatedGun::ammoUsed_cend() const {
    return ammoUsedList.cend();
}
std::map<Event, std::shared_ptr<const Event>>::const_iterator AssociatedGun::eventsUsed_cbegin() const{
    return eventsUsed.cbegin();
}
std::map<Event, std::shared_ptr<const Event>>::const_iterator AssociatedGun::eventsUsed_cend() const{
    return eventsUsed.cend();
}

