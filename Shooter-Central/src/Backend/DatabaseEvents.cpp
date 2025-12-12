#include "DatabaseEvents.h"

namespace ShooterCentral::DatabaseEvents {

namespace Add {

Event::Event(const ShootingEvent& set, bool setApplyToArmory, bool setApplyToStockpile) : 
    event { set },
    applyToArmory       { setApplyToArmory },
    applyToStockpile    { setApplyToStockpile }
{

}
Status Event::execute(Database& db) {
    using namespace ShooterCentral::UI;

    bool success { false };
    try {
        success = applyEvent(db, event, applyToArmory, applyToStockpile);
    }
    catch(std::invalid_argument& e){
        auto bodyFunction = [e]() {
            centerText("Failed to Add Event");
            centerTextDisabled("(No changes were made)");

            ImGui::Separator();
            ImGui::Dummy( ImVec2{400, 0} );
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::BulletText("%s", e.what());
        };

        CustomClosePopup popup { "Add Event Failed", bodyFunction };

        UIEvents::PushPopup pushPopup { &popup };
        pushEvent(&pushPopup);
        return Status { false, "Add event failed" };
    }
    catch(AddEventFlags flags){
        auto bodyFunction = [flags]() {
            const VerifyEventFlags& verifyFlags { flags.verifyFlags };

            centerText("Failed to Add Event");
            centerTextDisabled("(No changes were made)");
            ImGui::Separator();
            ImGui::Dummy( ImVec2{400, 0} );
            ImGui::Spacing();
            ImGui::Spacing();

            if(flags.alreadyExists)
                ImGui::BulletText("Event Already Exists");
            if(verifyFlags.locationInvalid)
                ImGui::BulletText("Invalid Location");
            if(verifyFlags.eventTypeInvalid)
                ImGui::BulletText("Invalid Event Type");
            if(verifyFlags.dateInvalid)
                ImGui::BulletText("Invalid date");
            if(verifyFlags.noGuns)
                ImGui::BulletText("No Guns were used");
            if(verifyFlags.gunWasInvalid)
                ImGui::BulletText("A Gun used was invalid");
            if(verifyFlags.ammoWasInvalid)
                ImGui::BulletText("An Ammo type used was invalid");
        };

        CustomClosePopup popup { "Add Event Failed", bodyFunction };

        UIEvents::PushPopup pushPopup { &popup };
        pushEvent(&pushPopup);
        return Status { false, "Add event failed" };
    }


    // All was successful
    if(success){
        SimpleClosePopup popup {"Event Created", "Event was successfully created"};

        UIEvents::PushPopup pushPopup { &popup };
        UIEvents::SetScreenData::Add_EventWindow resetBuffers { };

        pushEvent(&pushPopup);
        pushEvent(&resetBuffers);

        return Status { true };
    }

    return Status { false, "Add event failed" };
}
Gun::Gun(const GunMetadata& setInfo) : gunInfo {setInfo} {

}
Status Gun::execute (Database& db) {
    using namespace ShooterCentral::UI;

    AddGunFlags flags { db.addToArmory(gunInfo) };

    if(!flags.wasAdded){
        auto bodyFunction = [flags]() {
            const VerifyGunMetadataFlags& verifyFlags { flags.verifyFlags };

            centerText("Failed to Add Gun");
            centerTextDisabled("(No changes were made)");
            ImGui::Separator();
            ImGui::Dummy( ImVec2{400, 0} );
            ImGui::Spacing();
            ImGui::Spacing();

            if(flags.alreadyExists)
                ImGui::BulletText("Gun Already Exists");
            if(verifyFlags.weaponTypeInvalid)
                ImGui::BulletText("Invalid Weapon Type");
            if(verifyFlags.cartridgeInvalid)
                ImGui::BulletText("Invalid Cartridge");
            if(verifyFlags.nameInvalid)
                ImGui::BulletText("Invalid Name");
        };

        CustomClosePopup popup { "Add Gun Failed", bodyFunction };

        UIEvents::PushPopup pushPopup { &popup };
        pushEvent(&pushPopup);
        return Status { false, "Add Gun failed" };
    }
    else{
        db.getGun(gunInfo).setActive(true);
        SimpleClosePopup popup {"Gun Created", "Gun was successfully created"};

        UIEvents::PushPopup pushPopup { &popup };
        UIEvents::SetScreenData::Add_GunWindow resetBuffers { };

        pushEvent(&pushPopup);
        pushEvent(&resetBuffers);

        return Status { true };
    }
}
Ammo::Ammo(const AmmoMetadata& setInfo) : amountOfAmmo { AmountOfAmmo { setInfo } } {

}
Ammo::Ammo(const AmountOfAmmo& setAmmo) : amountOfAmmo { setAmmo } {

}
Status Ammo::execute (Database& db) {
    using namespace ShooterCentral::UI;

    AddAmmoFlags flags { db.addToStockpile(amountOfAmmo) };

    if(!flags.wasAdded){
        auto bodyFunction = [flags]() {
            const VerifyAmountOfAmmoFlags& verifyFlags { flags.verifyFlags };

            centerText("Failed to Add Ammo");
            centerTextDisabled("(No changes were made)");
            ImGui::Separator();
            ImGui::Dummy( ImVec2{400, 0} );
            ImGui::Spacing();
            ImGui::Spacing();

            if(flags.alreadyExists)
                ImGui::BulletText("Ammo Type Already Exists");
            if(verifyFlags.nameInvalid)
                ImGui::BulletText("Invalid Name");
            if(verifyFlags.cartridgeInvalid)
                ImGui::BulletText("Invalid Cartridge");
            if(verifyFlags.manufacturerInvalid)
                ImGui::BulletText("Invalid Manufacturer");
            if(verifyFlags.grainWeightInvalid)
                ImGui::BulletText("Invalid Grain Weight");
            if(verifyFlags.amountInvalid)
                ImGui::BulletText("Invalid Amount");
        };

        CustomClosePopup popup { "Add Ammo Type Failed", bodyFunction };

        UIEvents::PushPopup pushPopup { &popup };
        pushEvent(&pushPopup);
        return Status { false, "Add Ammo Type failed" };
    }
    else{
        db.getAmmo(amountOfAmmo.getAmmoInfo()).setActive(true);
        SimpleClosePopup popup {"Ammo Type Created", "Ammo type was successfully created"};

        UIEvents::PushPopup pushPopup { &popup };
        UIEvents::SetScreenData::Add_AmmoWindow resetBuffers { };

        pushEvent(&pushPopup);
        pushEvent(&resetBuffers);

        return Status { true };

    }
}

Manufacturer::Manufacturer(const ShooterCentral::Manufacturer& m) : manufacturer { m } {

}
Status Manufacturer::execute (Database& db) {
    if(db.addManufacturer(manufacturer))
        return Status{true};

    return Status{false, "Failed to add Manufacturer" };
}
Cartridge::Cartridge(const ShooterCentral::Cartridge& c) : cartridge { c } {

}
Status Cartridge::execute (Database& db) {
    if(db.addCartridge(cartridge))
        return Status{true};

    return Status{false, "Failed to add Cartridge" };
}
EventType::EventType(const ShooterCentral::ShootingEventType& et) : eventType { et } {

}
Status EventType::execute (Database& db) {
    if(db.addEventType(eventType))
        return Status{true};

    return Status{false, "Failed to add ShootingEventType" };
}
WeaponType::WeaponType(const ShooterCentral::WeaponType& wt) : weaponType { wt } {
    
}
Status WeaponType::execute (Database& db) {
    if(db.addWeaponType(weaponType))
        return Status{true};

    return Status{false, "Failed to add WeaponType" };
}

Location::Location(const ShooterCentral::Location& loc) : location { loc } {
    
}
Status Location::execute (Database& db) {
    if(db.addLocation(location))
        return Status{true};

    return Status{false, "Failed to add Location" };
}

}   // Add namespace

}   // End DatabaseEvents namespace


