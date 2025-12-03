#pragma once

#include "Events.h"
#include "Database.h"

namespace ShooterCentral::DatabaseEvents {

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
    */

namespace Add {

    class Event : public DatabaseEvent {
        public:
            Event(const ShootingEvent& set);
            ~Event() = default;

            DATABASE_EVENT_FUNCTIONS(Event)

        private:
            ShootingEvent event;
    };


    // Metadata items
    class Manufacturer : public DatabaseEvent {
        public:
            Manufacturer(const ShooterCentral::Manufacturer& m);
            ~Manufacturer() = default;

            DATABASE_EVENT_FUNCTIONS(Manufacturer)

        private:
                ShooterCentral::Manufacturer manufacturer;
    };
    class Cartridge : public DatabaseEvent {
        public:
            Cartridge(const ShooterCentral::Cartridge& c);
            ~Cartridge() = default;

            DATABASE_EVENT_FUNCTIONS(Cartridge)

        private:
            ShooterCentral::Cartridge cartridge;
    };
    class EventType : public DatabaseEvent {
        public:
            EventType(const ShooterCentral::ShootingEventType& et);
            ~EventType() = default;

            DATABASE_EVENT_FUNCTIONS(EventType)

        private:
            ShooterCentral::ShootingEventType eventType;
    };
    class WeaponType : public DatabaseEvent {
        public:
            WeaponType(const ShooterCentral::WeaponType& wt);
            ~WeaponType() = default;

            DATABASE_EVENT_FUNCTIONS(WeaponType)

        private:
            ShooterCentral::WeaponType weaponType;
    };

    class Location : public DatabaseEvent {
        public:
            Location(const ShooterCentral::Location& loc);
            ~Location() = default;

            DATABASE_EVENT_FUNCTIONS(Location)

        private:
            ShooterCentral::Location location;
    };
}   // Add namespace

}   // End DatabaseEvents namespace

