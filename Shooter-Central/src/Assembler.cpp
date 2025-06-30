//#include "Framework.h"
#include <Config.h>

#include <LAS/ModuleAPI.h>

// FOR TESTING
#include <map>
#include <iostream>
#include <format>
#include <memory>
#include "Model.h"

//static ShooterCentral::Framework application {};


bool LASM_loadModuleInfo(LAS::Information::ModuleInfo& moduleInfo){
    std::puts("\ntest from module load\n");
    using namespace ShooterCentral;


    std::map<Ammo, std::shared_ptr<AmountOfAmmo>> ammoStockpile;

    Ammo testAmmo1 { "testAmmo1", Manufacturer{"testMan1"}, Cartridge{"testCart1"}, 69 };
    AmountOfAmmo testAmount1 {testAmmo1, 50};
    ammoStockpile.try_emplace(testAmount1.ammo, std::make_shared<AmountOfAmmo>(testAmount1));

    Ammo testAmmo2 { "testAmmo2", Manufacturer{"testMan1"}, Cartridge{"testCart1"}, 69 };
    AmountOfAmmo testAmount2 {testAmmo2, 40};
    ammoStockpile.try_emplace(testAmount2.ammo, std::make_shared<AmountOfAmmo>(testAmount2));

    std::puts("Ammo list:");
    for(const auto& [key, value] : ammoStockpile){
        std::cout << std::format("Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n", value->ammo.name, value->ammo.manufacturer.getName(), value->ammo.cartridge.getName(), value->ammo.grainWeight, value->amount); 
        std::cout << "  " << value << "\n";
    }

    std::puts("\n");

    std::map<Gun, std::shared_ptr<Gun>> gunList;
    Gun testGun1 { "testGun1", WeaponType{"testWT1"}, Cartridge{"testCart1"} };
    gunList.try_emplace(testGun1, std::make_shared<Gun>(testGun1));

    std::puts("Gun list:");
    for(const auto& [key, value] : gunList){
        std::cout << std::format("Name: {}, WT: {}, Cart: {}\n", value->getName(), value->getWeaponType().getName(), value->getCartridge().getName());
        std::cout << "  " << value << "\n";
    }


    AssociatedAmmo testAssociatedAmmo1 { ammoStockpile.at(testAmmo1) };
    testAssociatedAmmo1.addGun(gunList.at(testGun1));

    std::cout << "\nAssociatedAmmo: ";
    const AmountOfAmmo& testRetrievedAmmo {testAssociatedAmmo1.getAmountOfAmmo()};
    std::cout << std::format("Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n", testRetrievedAmmo.ammo.name, testRetrievedAmmo.ammo.manufacturer.getName(), testRetrievedAmmo.ammo.cartridge.getName(), testRetrievedAmmo.ammo.grainWeight, testRetrievedAmmo.amount); 
    std::cout << "  " << &testAssociatedAmmo1.getAmountOfAmmo() << "\n";

    std::cout << "Associated Guns: ";
    for( auto itr{testAssociatedAmmo1.begin()}; itr != testAssociatedAmmo1.end(); ++itr){
        const auto& [key, value] {*itr};
        std::cout << std::format("Name: {}, WT: {}, Cart: {}\n", value->getName(), value->getWeaponType().getName(), value->getCartridge().getName());
        std::cout << "  " << value << "\n";
    }

    std::puts("\n");
    /*
    using namespace LAS;
    Information::Version sdkVersion { SDK::getVersionMajor(), SDK::getVersionMinor(), SDK::getVersionPatch()};

    moduleInfo.sdkVersion       = sdkVersion;
    moduleInfo.moduleVersion    = Information::Version{SC_VERSION_MAJOR, SC_VERSION_MINOR, SC_VERSION_PATCH};
    moduleInfo.title            = application.getWindow()->getTitle();
    moduleInfo.commandGroupName = application.getCommandGroupName();
    moduleInfo.window           = application.getWindow();

    // Add commands if needed
    */

    return true;
}
bool LASM_init(const LAS::Information::EnvironmentInfo& envInfo){
    //   return application.setup(envInfo.directory);
    return true;
}
void LASM_cleanup(){
    LAS::log_info("Cleanup from Shooter Central lasm_cleanup()");
}
