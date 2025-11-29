#pragma once

#include "Events.h"
#include "Database.h"

namespace ShooterCentral {

namespace DatabaseEvents {

    /*
    class CreateGun : public DatabaseEvent {
    public:
        CreateGun(const ObjectBuffers::GunMetadata& set);
        ~CreateGun() = default;

        DATABASEL_EVENT_FUNCTIONS(CreateGun)

    private:
        ObjectBuffers::GunMetadata info;
    };
    class CreateAmmo : public DatabaselEvent {
    public:
        CreateAmmo(const ObjectBuffers::AmmoMetadata& set);
        ~CreateAmmo() = default;

        DATABASEL_EVENT_FUNCTIONS(CreateAmmo)

    private:
        ObjectBuffers::AmmoMetadata info;
    };
    class CreateEvent : public DatabaseEvent {
    public:
        CreateEvent(const ObjectBuffers::ShootingEventMetadata& set);
        ~CreateEvent() = default;

        DatabaseL_EVENT_FUNCTIONS(CreateEvent)

    private:
        ObjectBuffers::ShootingEventMetadata info;
    };
    */

    // Metadata items
    class AddManufacturer : public DatabaseEvent {
        public:
            AddManufacturer(const Manufacturer& m);
            ~AddManufacturer() = default;

            DATABASE_EVENT_FUNCTIONS(AddManufacturer)

        private:
            Manufacturer manufacturer;
    };
    class AddCartridge : public DatabaseEvent {
        public:
            AddCartridge(const Cartridge& c);
            ~AddCartridge() = default;

            DATABASE_EVENT_FUNCTIONS(AddCartridge)

        private:
            Cartridge cartridge;
    };
    class AddEventType : public DatabaseEvent {
        public:
            AddEventType(const ShootingEventType& et);
            ~AddEventType() = default;

            DATABASE_EVENT_FUNCTIONS(AddEventType)

        private:
                ShootingEventType eventType;
    };
    class AddWeaponType : public DatabaseEvent {
    public:
        AddWeaponType(const WeaponType& wt);
        ~AddWeaponType() = default;

        DATABASE_EVENT_FUNCTIONS(AddWeaponType)

    private:
        WeaponType weaponType;
    };

    class AddLocation : public DatabaseEvent {
    public:
        AddLocation(const Location& loc);
        ~AddLocation() = default;

        DATABASE_EVENT_FUNCTIONS(AddLocation)

    private:
        Location location;
    };

}   // End DatabaseEvents namespace


}   // End SC namespace

