#include "Model.h"

using namespace ShooterCentral;


Model::Model() {

}
Model::~Model() {

}
std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>::const_iterator Model::knownAmmo_cbegin() const {
    return knownAmmo.cbegin();
}
std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>>::const_iterator Model::knownAmmo_cend() const {
    return knownAmmo.cend();
}
std::map<GunMetadata, std::shared_ptr<GunMetadata>>::const_iterator Model::knownGuns_cbegin() const{
    return knownGuns.cbegin();
}
std::map<GunMetadata, std::shared_ptr<GunMetadata>>::const_iterator Model::knownGuns_cend() const{
    return knownGuns.cend();
}
std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator Model::ammoStockpile_cbegin() const {
    return ammoStockpile.cbegin();
}
std::map<AmmoMetadata,  AssociatedAmmo>::const_iterator Model::ammoStockpile_cend() const{
    return ammoStockpile.cend();
}
std::map<GunMetadata,   AssociatedGun>::const_iterator Model::gunsInArmory_cbegin() const{
    return gunsInArmory.cbegin();
}
std::map<GunMetadata,   AssociatedGun>::const_iterator Model::gunsInArmory_cend() const{
    return gunsInArmory.cend();
}
std::map<Event, std::shared_ptr<Event>>::const_iterator Model::events_cbegin() const{
    return events.cbegin();
}
std::map<Event, std::shared_ptr<Event>>::const_iterator Model::events_cend() const{
    return events.cend();
}

std::set<Manufacturer>::const_iterator      Model::manufacturers_cbegin() const {
    return manufacturers.cbegin();
}
std::set<Manufacturer>::const_iterator      Model::manufacturers_cend() const{
    return manufacturers.cend();
}
std::set<Cartridge>::const_iterator         Model::cartridges_cbegin() const {
    return cartridges.cbegin();
}
std::set<Cartridge>::const_iterator         Model::cartridges_cend() const{
    return cartridges.cend();
}
std::set<Location>::const_iterator          Model::locations_cbegin() const{
    return locations.cbegin();
}
std::set<Location>::const_iterator          Model::locations_cend() const{
    return locations.cend();
}
std::set<EventType>::const_iterator         Model::eventTypes_cbegin() const{
    return eventTypes.cbegin();
}
std::set<EventType>::const_iterator         Model::eventTypes_cend() const{
    return eventTypes.cend();
}
std::set<WeaponType>::const_iterator        Model::weaponTypes_cbegin() const{
    return weaponTypes.cbegin();
}
std::set<WeaponType>::const_iterator        Model::weaponTypes_cend() const{
    return weaponTypes.cend();
}


bool Model::knownAmmo_contains     (const AmmoMetadata& key) const{
    return knownAmmo.contains(key);
}
bool Model::knownGuns_contains     (const GunMetadata& key) const{
    return knownGuns.contains(key);
}
bool Model::ammoStockpile_contains (const AmmoMetadata& key) const{
    return ammoStockpile.contains(key);
}
bool Model::gunsInArmory_contains  (const GunMetadata& key) const{
    return gunsInArmory.contains(key);
}
bool Model::events_contains        (const Event& key) const{
    return events.contains(key);
}
bool Model::manufacturers_contains (const Manufacturer& key) const{
    return manufacturers.contains(key);
}
bool Model::cartridges_contains    (const Cartridge& key) const{
    return cartridges.contains(key);
}
bool Model::locations_contains     (const Location& key) const{
    return locations.contains(key);
}
bool Model::eventTypes_contains    (const EventType& key) const{
    return eventTypes.contains(key);
}
bool Model::weaponTypes_contains   (const WeaponType& key) const{
    return weaponTypes.contains(key);
}


std::shared_ptr<AmmoMetadata> Model::knownAmmo_at    (const AmmoMetadata& key){
    return knownAmmo.at(key);
}
std::shared_ptr<GunMetadata> Model::knownGuns_at    (const GunMetadata& key){
    return knownGuns.at(key);
}
std::shared_ptr<Event> Model::events_at       (const Event& key){
    return events.at(key);
}
AssociatedAmmo& Model::ammoStockpile_at(const AmmoMetadata& key){
    return ammoStockpile.at(key);
}
AssociatedGun& Model::gunsInArmory_at (const GunMetadata& key) {
    return gunsInArmory.at(key);
}
bool Model::knownAmmo_add      (std::shared_ptr<AmmoMetadata> add){
    if(!add)
        return false;

    return knownAmmo.try_emplace(*add, add).second;
}
bool Model::knownGuns_add      (std::shared_ptr<GunMetadata> add){
    if(!add)
        return false;

    return knownGuns.try_emplace(*add, add).second;
}
bool Model::events_add(std::shared_ptr<Event> add){
    if(!add)
        return false;

    return events.try_emplace(*add, add).second;
}       
bool Model::ammoStockpile_add  (const AssociatedAmmo& add){
    if(!add)
        return false;

    return ammoStockpile.try_emplace(add.getAmountOfAmmo().getAmmo(), add).second;
}
bool Model::gunsInArmory_add   (const AssociatedGun& add){
    if(!add)
        return false;

    return gunsInArmory.try_emplace(add.getGun(), add).second;
}
bool Model::manufacturers_add  (const Manufacturer& add){
    return manufacturers.emplace(add).second;
}
bool Model::cartridges_add     (const Cartridge& add){
    return cartridges.emplace(add).second;
}
bool Model::eventTypes_add     (const EventType& add){
    return eventTypes.emplace(add).second;
}
bool Model::weaponTypes_add    (const WeaponType& add){
    return weaponTypes.emplace(add).second;
}
bool Model::locations_add      (const Location& add){
    return locations.emplace(add).second;
}





