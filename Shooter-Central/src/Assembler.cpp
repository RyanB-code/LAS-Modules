#include "Framework.h"

#include <Config.h>
#include <LAS/ModuleAPI.h>

#include <functional>

static ShooterCentral::Framework application {};

bool LASM_loadModuleInfo(LAS::Information::ModuleInfo& moduleInfo){
    using namespace LAS;
    using namespace ShooterCentral;

    Information::Version sdkVersion { SDK::getVersionMajor(), SDK::getVersionMinor(), SDK::getVersionPatch()};

    moduleInfo.sdkVersion       = sdkVersion;
    moduleInfo.moduleVersion    = Information::Version{SC_VERSION_MAJOR, SC_VERSION_MINOR, SC_VERSION_PATCH};

    moduleInfo.title            = Framework::TITLE;
    moduleInfo.commandGroupName = Framework::COMMAND_GROUP_NAME;

    moduleInfo.updateFunction   = std::function<void()>{std::bind(&Framework::update, &application)};
    moduleInfo.drawFunction     = std::function<void()>{std::bind(&Framework::draw, &application)};
    // Add commands if needed

    return true;
}
bool LASM_init(const LAS::Information::EnvironmentInfo& envInfo){
    return application.setup(envInfo.directory, envInfo.moduleShown);
}
void LASM_cleanup(){
    LAS::log_info("Cleanup from Shooter Central lasm_cleanup()");
}

/*


    std::puts("\n");

    std::map<AmmoMetadata, std::shared_ptr<AmmoMetadata>> ammoList;
    std::map<AmmoMetadata, std::shared_ptr<AmountOfAmmo>> ammoStockpile;

    AmmoMetadata testAmmo1 { "testAmmo1", Manufacturer{"testMan1"}, Cartridge{"testCart1"}, 69 };
    AmmoMetadata testAmmo2 { "testAmmo2", Manufacturer{"testMan1"}, Cartridge{"testCart1"}, 69 };
    ammoList.try_emplace(testAmmo1, std::make_shared<AmmoMetadata>(testAmmo1));
    ammoList.try_emplace(testAmmo2, std::make_shared<AmmoMetadata>(testAmmo2));

    AmountOfAmmo testAmount1 {ammoList.at(testAmmo1), 50};
    AmountOfAmmo testAmount2 {ammoList.at(testAmmo2), 40};
    ammoStockpile.try_emplace(testAmount1.getAmmo(), std::make_shared<AmountOfAmmo>(testAmount1));
    ammoStockpile.try_emplace(testAmount2.getAmmo(), std::make_shared<AmountOfAmmo>(testAmount2));

    std::puts("Ammo in list:");
    for(const auto& [key, value] : ammoList){
        std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}\n", value->name, value->manufacturer.getName(), value->cartridge.getName(), value->grainWeight); 
        std::cout << "    " << value << "\n";
    }

    std::puts("\nAmmo in Stockpile:");
    for(const auto& [key, value] : ammoStockpile){
        std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}\n", value->getAmmo().name, value->getAmmo().manufacturer.getName(), value->getAmmo().cartridge.getName(), value->getAmmo().grainWeight, value->getAmount()); 
        std::cout << "    AmountOfAmmo: " << value << "\n";
        std::cout << "    AmmoMetadata: " << &value->getAmmo() << "\n";
    }

    std::puts("\n\n");

    std::map<GunMetadata, std::shared_ptr<GunMetadata>> gunList;
    GunMetadata testGun1 { "testGun1", WeaponType{"testWT1"}, Cartridge{"testCart1"} };
    GunMetadata testGun2 { "testGun2", WeaponType{"testWT1"}, Cartridge{"testCart1"} };
    gunList.try_emplace(testGun1, std::make_shared<GunMetadata>(testGun1));
    gunList.try_emplace(testGun2, std::make_shared<GunMetadata>(testGun2));

    std::puts("Guns in Armory:");
    for(const auto& [key, value] : gunList){
        std::cout << std::format("  Name: {}, WT: {}, Cart: {}\n", value->name, value->weaponType.getName(), value->cartridge.getName());
        std::cout << "    " << value << "\n";
    }

    std::puts("\n\n");

    AssociatedAmmo testAssociatedAmmo1 { ammoStockpile.at(testAmmo1) };
    testAssociatedAmmo1.addGun(gunList.at(testGun1));
    testAssociatedAmmo1.addGun(gunList.at(testGun2));

    std::cout << "AssociatedAmmo: ";
    const AmountOfAmmo& testRetrievedAmmo {testAssociatedAmmo1.getAmountOfAmmo()};
    std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n", testRetrievedAmmo.getAmmo().name, testRetrievedAmmo.getAmmo().manufacturer.getName(), testRetrievedAmmo.getAmmo().cartridge.getName(), testRetrievedAmmo.getAmmo().grainWeight, testRetrievedAmmo.getAmount()); 
    std::cout << "    " << &testAssociatedAmmo1.getAmountOfAmmo() << "\n";

    std::cout << "\n  Associated Guns: \n";
    for(auto itr{testAssociatedAmmo1.cbegin()}; itr != testAssociatedAmmo1.cend(); ++itr){
        const auto& [key, value] {*itr};
        std::cout << std::format("    Name: {}, WT: {}, Cart: {}\n", value->name, value->weaponType.getName(), value->cartridge.getName());
        std::cout << "      " << value << "\n";
    }

    std::puts("\n\n");


    GunAndAmmo gunAndAmmoTest1 {gunList.at(testGun1)};
    gunAndAmmoTest1.addAmmoUsed(testAmount1);
    gunAndAmmoTest1.addAmmoUsed(testAmount2);
    gunAndAmmoTest1.addAmmoUsed(testAmount1);

    std::cout << "GunAndAmmo: \n";
    std::cout << std::format("Gun Name: {}, WT: {}, Cart: {}\n", gunAndAmmoTest1.getGun().name, gunAndAmmoTest1.getGun().weaponType.getName(), gunAndAmmoTest1.getGun().cartridge.getName());
    std::cout << "  " << &gunAndAmmoTest1.getGun() << "\n";

    std::cout << "Ammo used in gun: \n";
    for(auto itr {gunAndAmmoTest1.cbegin()}; itr != gunAndAmmoTest1.cend(); ++itr) {
        auto& amountOfAmmo { *itr };
        std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n", amountOfAmmo.getAmmo().name, amountOfAmmo.getAmmo().manufacturer.getName(), amountOfAmmo.getAmmo().cartridge.getName(), amountOfAmmo.getAmmo().grainWeight, amountOfAmmo.getAmount()); 
        std::cout << "    AmountOfAmmo: " << &amountOfAmmo << "\n";
        std::cout << "    AmmoMetadata: " << &amountOfAmmo.getAmmo() << "\n";


    }

    std::puts("\n\n");


    const std::chrono::zoned_time now {std::chrono::current_zone(), std::chrono::system_clock::now( ) };
    const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now.get_local_time())};

    std::cout << "\nEvents:\n";
    Event testEvent1 { Location{"Test Location"}, EventType{"Test EventType"}, "no notes here fuckwad", ymd};
    std::cout << std::format("Location: {}, EventType: {}, Notes: {}, Date: {}\n", testEvent1.getLocation().getName(), testEvent1.getEventType().getName(), testEvent1.getNotes(), testEvent1.printDate());

    testEvent1.addGun(gunAndAmmoTest1);
    std::cout << "Guns Used: \n";
    for(auto itr {testEvent1.cbegin()}; itr != testEvent1.cend(); ++itr) {
        auto& gun {itr->getGun()};
        std::cout << std::format("  Gun Name: {}, WT: {}, Cart: {}\n", gun.name, gun.weaponType.getName(), gun.cartridge.getName());
        std::cout << "    GunMetadata: " << &gun << "\n";

        // Print AmmoUsed
        std::cout << "\n";
        for(auto itr2 {itr->cbegin()}; itr2 != itr->cend(); ++itr2) {
            auto& amountOfAmmo { *itr2 };
            std::cout << std::format("  Ammo Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n", amountOfAmmo.getAmmo().name, amountOfAmmo.getAmmo().manufacturer.getName(), amountOfAmmo.getAmmo().cartridge.getName(), amountOfAmmo.getAmmo().grainWeight, amountOfAmmo.getAmount()); 
            std::cout << "    AmountOfAmmo: " << &amountOfAmmo << "\n";
            std::cout << "    AmmoMetadata: " << &amountOfAmmo.getAmmo() << "\n";
        }
    }

    std::puts("\n\n");


    AssociatedGun testAssGun1 (gunList.at(testGun1));
    testAssGun1.addEvent(std::make_shared<Event>(testEvent1));

    std::cout << "AssociatedGun: \n";
    std::cout << std::format("  Gun Name: {}, WT: {}, Cart: {}, RoundCount: {}\n", testAssGun1.getGun().name, testAssGun1.getGun().weaponType.getName(), testAssGun1.getGun().cartridge.getName(), testAssGun1.getRoundCount());
    std::cout << "  " << &testAssGun1.getGun() << "\n";

    for(auto itr{testAssGun1.eventsUsed_cbegin()}; itr != testAssGun1.eventsUsed_cend(); ++itr){
        const Event& event {*itr->second};
        std::cout << std::format("  Event: Location: {}, EventType: {}, Notes: {}, Date: {}\n", event.getLocation().getName(), event.getEventType().getName(), event.getNotes(), event.printDate());
    }
    std::cout << "\n";
    for(auto itr{testAssGun1.ammoUsed_cbegin()}; itr != testAssGun1.ammoUsed_cend(); ++itr){
        auto& amountOfAmmo { itr->second };
        std::cout << std::format("  Name: {}, Man: {}, Cart: {}, GW: {}, Amount: {}\n", amountOfAmmo.getAmmo().name, amountOfAmmo.getAmmo().manufacturer.getName(), amountOfAmmo.getAmmo().cartridge.getName(), amountOfAmmo.getAmmo().grainWeight, amountOfAmmo.getAmount()); 
         std::cout << "    AmountOfAmmo: " << &amountOfAmmo << "\n";
         std::cout << "    AmmoMetadata: " << &amountOfAmmo.getAmmo() << "\n";

    }


    std::cout << "\n\n";
 
*/
