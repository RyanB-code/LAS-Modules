#pragma once

#include "Events.h"

namespace ShooterCentral {

namespace DatabaseEvents {

    /* Full Objects
    class CreateGun : public ModelEvent {
    public:
        CreateGun(const ObjectBuffers::GunMetadata& set);
        ~CreateGun() = default;

        MODEL_EVENT_FUNCTIONS(CreateGun)

    private:
        ObjectBuffers::GunMetadata info;
    };
    class CreateAmmo : public ModelEvent {
    public:
        CreateAmmo(const ObjectBuffers::AmmoMetadata& set);
        ~CreateAmmo() = default;

        MODEL_EVENT_FUNCTIONS(CreateAmmo)

    private:
        ObjectBuffers::AmmoMetadata info;
    };
    class CreateEvent : public ModelEvent {
    public:
        CreateEvent(const ObjectBuffers::ShootingEventMetadata& set);
        ~CreateEvent() = default;

        MODEL_EVENT_FUNCTIONS(CreateEvent)

    private:
        ObjectBuffers::ShootingEventMetadata info;
    };

    // Metadata items
    class AddManufacturer : public ModelEvent {
        public:
            AddManufacturer(const Manufacturer& m);
            ~AddManufacturer() = default;

            MODEL_EVENT_FUNCTIONS(AddManufacturer)

        private:
            Manufacturer manufacturer;
    };
    class AddCartridge : public ModelEvent {
        public:
            AddCartridge(const Cartridge& c);
            ~AddCartridge() = default;

            MODEL_EVENT_FUNCTIONS(AddCartridge)

        private:
            Cartridge cartridge;
    };
    class AddEventType : public ModelEvent {
        public:
            AddEventType(const ShootingEventType& et);
            ~AddEventType() = default;

            MODEL_EVENT_FUNCTIONS(AddEventType)

        private:
                ShootingEventType eventType;
    };
    class AddWeaponType : public ModelEvent {
    public:
        AddWeaponType(const WeaponType& wt);
        ~AddWeaponType() = default;

        MODEL_EVENT_FUNCTIONS(AddWeaponType)

    private:
        WeaponType weaponType;
    };

    class AddLocation : public ModelEvent {
    public:
        AddLocation(const Location& loc);
        ~AddLocation() = default;

        MODEL_EVENT_FUNCTIONS(AddLocation)

    private:
        Location location;
    };

    */

}   // End DatabaseEvents namespace


}   // End SC namespace

