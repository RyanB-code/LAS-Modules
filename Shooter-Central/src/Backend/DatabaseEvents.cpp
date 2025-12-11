#include "DatabaseEvents.h"

namespace ShooterCentral::DatabaseEvents {

/* / Full objects
CreateGun::CreateGun(const ObjectBuffers::GunMetadata& set) : info { set } {

}
Status CreateGun::execute (Containers& container) {
    if(container.knownGuns_create(info).second)
        return Status { true };

    return Status { false, "Failed to create Gun object and add to database" };
}
CreateAmmo::CreateAmmo(const ObjectBuffers::AmmoMetadata& set) : info { set } {

}
Status CreateAmmo::execute (Containers& container) {
    if(container.knownAmmo_create(info).second)
        return Status { true };

    return Status { false, "Failed to create Ammo object and add to database" };
}
CreateEvent::CreateEvent(const ObjectBuffers::ShootingEventMetadata& set) : info { set } {

}
Status CreateEvent::execute (Containers& container) {
    if(container.events_create(info).second)
        return Status { true };

    return Status { false, "Failed to create Event object and add to database" };
}
*/

// Metadata items
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
                ImGui::BulletText("Invalid WeaponType");
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


