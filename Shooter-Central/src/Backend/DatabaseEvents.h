#pragma once

#include "Events.h"
#include "Database.h"
#include "UI/UIEvents.h"

namespace ShooterCentral::DatabaseEvents {

namespace Add {

    class Event : public DatabaseEvent {
        public:
            Event(const ShootingEvent& set, bool applyToArmory, bool applyToStockpile);
            ~Event() = default;

            DATABASE_EVENT_FUNCTIONS(Event)

        private:
            ShootingEvent event;
            bool applyToArmory      { false };
            bool applyToStockpile   { false };
    };
    class Gun : public DatabaseEvent {
        public:
            Gun(const GunMetadata& setInfo);
            ~Gun() = default;

            DATABASE_EVENT_FUNCTIONS(Gun)

        private:
            GunMetadata gunInfo { };
    };
    class Ammo : public DatabaseEvent {
        public:
            Ammo(const AmmoMetadata& setInfo);
            Ammo(const AmountOfAmmo& setAmmo);
            ~Ammo() = default;

            DATABASE_EVENT_FUNCTIONS(Ammo)

        private:
            AmountOfAmmo amountOfAmmo { };
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

namespace Edit {
    class Manufacturer : public DatabaseEvent {
        public:
            Manufacturer(const ShooterCentral::Manufacturer& setOld, const ShooterCentral::Manufacturer& setNew );
            ~Manufacturer() = default;

            DATABASE_EVENT_FUNCTIONS(Manufacturer)

        private:
            ShooterCentral::Manufacturer old        { };
            ShooterCentral::Manufacturer revised    { };
    };
    class Location : public DatabaseEvent {
        public:
            Location(const ShooterCentral::Location& setOld, const ShooterCentral::Location& setNew );
            ~Location() = default;

            DATABASE_EVENT_FUNCTIONS(Location)

        private:
            ShooterCentral::Location old        { };
            ShooterCentral::Location revised    { };
    };
    class EventType : public DatabaseEvent {
        public:
            EventType(const ShooterCentral::ShootingEventType& setOld, const ShooterCentral::ShootingEventType& setNew );
            ~EventType() = default;

            DATABASE_EVENT_FUNCTIONS(EventType)

        private:
            ShooterCentral::ShootingEventType old        { };
            ShooterCentral::ShootingEventType revised    { };
    };
    class WeaponType : public DatabaseEvent {
        public:
            WeaponType(const ShooterCentral::WeaponType& setOld, const ShooterCentral::WeaponType& setNew );
            ~WeaponType() = default;

            DATABASE_EVENT_FUNCTIONS(WeaponType)

        private:
            ShooterCentral::WeaponType old        { };
            ShooterCentral::WeaponType revised    { };
    };
    class Cartridge: public DatabaseEvent {
        public:
            Cartridge(const ShooterCentral::Cartridge& setOld, const ShooterCentral::Cartridge& setNew );
            ~Cartridge() = default;

            DATABASE_EVENT_FUNCTIONS(Cartridge)

        private:
            ShooterCentral::Cartridge old        { };
            ShooterCentral::Cartridge revised    { };
    };
    class GunMetadata: public DatabaseEvent {
        public:
            GunMetadata(const ShooterCentral::GunMetadata& setOld, const ShooterCentral::GunMetadata& setNew );
            ~GunMetadata() = default;

            DATABASE_EVENT_FUNCTIONS(GunMetadata)

        private:
            ShooterCentral::GunMetadata old        { };
            ShooterCentral::GunMetadata revised    { };
    };
}   // Edit nameespce

}   // End DatabaseEvents namespace

