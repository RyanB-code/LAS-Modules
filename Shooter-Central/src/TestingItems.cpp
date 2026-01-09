#include "TestingItems.h"

namespace ShooterCentral {

// FOR TESTING
void printAmmoMetadata(const AmmoMetadata& info) {
    std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}\n",  info.name, info.manufacturer.getName(), info.cartridge.getName(), info.grainWeight );
}
void printGunMetadata(const GunMetadata& info) {
    std::cout << std::format("  Name: {}, WT: {}, Cart: {}\n", info.name, info.weaponType.getName(), info.cartridge.getName());
}
void printEventMetadata(const ShootingEventMetadata& info){
    std::cout << std::format("  Event: Location: {}, EventType: {}, Notes: {}, Date: {}\n", info.location.getName(), info.eventType.getName(), info.notes, std::format("{:%Od %b %Y}", info.date) );
}
void printStockpileAmmo(const StockpileAmmo& ammo){
    printAmmoMetadata(ammo.getAmmoInfo());
    std::cout << "  Amount: " << ammo.getAmountOnHand() << "\n";
    std::cout << "    isActive: " << std::boolalpha << ammo.isActive() << "\n";

    std::cout << "  ";

    for(const auto& gunInfo : ammo.getGunsUsed() )
        printGunMetadata(gunInfo);
}
void printArmoryGun(const ArmoryGun& gun){
    printGunMetadata(gun.getGunInfo());
    std::cout << "    isActive: " << std::boolalpha << gun.isActive() << "\n";
    std::cout << "    roundCount: " << gun.getRoundCount() << "\n";

    for(const auto& eventInfo : gun.getEventsUsed() ){
        std::cout << "  ";
        printEventMetadata(eventInfo);
    }
    std::cout << "   -\n";
    for(const auto& [info, amountOfAmmo] : gun.getAmmoUsed()) {
        std::cout << "  ";
        printAmmoMetadata(amountOfAmmo.getAmmoInfo());
        std::cout << "    Amount: " << amountOfAmmo.getAmount() << '\n';
    }
}
void printAmountOfAmmo(const AmountOfAmmo& ammo){
    printAmmoMetadata(ammo.getAmmoInfo());
    std::cout << "  Amount: " << ammo.getAmount() << "\n";
}
void printEvent(const ShootingEvent& event) {
    const auto& info { event.getInfo() };
    printEventMetadata(info); 

    for(const auto& gunTrackingAmmo : event.getGunsUsed() ){
        std::cout << "  ";
        printGunMetadata(gunTrackingAmmo.getGunInfo());
        for(const auto& amountOfAmmo : gunTrackingAmmo.getAmmoUsed()) {
            std::cout << "    ";
            printAmmoMetadata(amountOfAmmo.getAmmoInfo());
            std::cout << "      Amount: " << amountOfAmmo.getAmount() << '\n';
        }
    }
}

}   // End SC namespace
