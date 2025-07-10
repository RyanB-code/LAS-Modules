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
    return gunsInArmory_contains(key);
}
bool Model::events_contains        (const Event& key) const{
    return events.contains(key);
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




