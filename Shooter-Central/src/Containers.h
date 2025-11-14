#pragma once

#include "Container Items/AssociatedItems.h"
#include "CommonItems.h"
#include "Events.h"

#include <map>
#include <set>
#include <memory>
#include <exception>

namespace ShooterCentral {

    namespace ObjectBuffers {
        struct GunMetadata {
            std::string name        { };
            std::string weaponType  { };
            std::string cartridge   { };
            bool        isActive    { false };
        };
        struct AmmoMetadata {
            std::string name            { };
            std::string manufacturer    { };
            std::string cartridge       { };
            int         grainWeight     { 0 };
            bool        isActive        { false };
        };
        struct ShootingEventMetadata {
            std::string location    { };
            std::string eventType   { };
            std::string notes       { };
            std::string date        { };
        };
    }

    class Containers {
    public:
        Containers();
        ~Containers();

        // DELETED GETTERS

        std::pair<std::shared_ptr<AmmoMetadata>, bool>  knownAmmo_create   (const ObjectBuffers::AmmoMetadata& add);    // Strong rollback guarantee if any operation fails
        std::pair<std::shared_ptr<GunMetadata>, bool>   knownGuns_create   (const ObjectBuffers::GunMetadata& add);
        std::pair<std::shared_ptr<ShootingEvent>, bool> events_create      (const ObjectBuffers::ShootingEventMetadata& add);


        bool ammoStockpile_add  (const AssociatedAmmo& add); // Will return false if cannot add to ammoStockpile OR amountPerCartridge container
        bool gunsInArmory_add   (const AssociatedGun& add);

        // DELETED METADATA ITEMS ADD

        std::shared_ptr<AmmoMetadata>   knownAmmo_at    (const AmmoMetadata& key);
        std::shared_ptr<GunMetadata>    knownGuns_at    (const GunMetadata& key);
        std::shared_ptr<ShootingEvent>  events_at       (const ShootingEvent& key);
        std::shared_ptr<AssociatedAmmo> ammoStockpile_at(const AmmoMetadata& key);
        std::shared_ptr<AssociatedGun>  gunsInArmory_at (const GunMetadata& key);

        bool ammoStockpile_contains (const AmmoMetadata& key) const;
        bool gunsInArmory_contains  (const GunMetadata& key) const;

        int getAmountOfCartridge(const Cartridge& cartridge) const;

    private:
        std::map<AmmoMetadata,  std::shared_ptr<AmmoMetadata>>  knownAmmo;
        std::map<GunMetadata,   std::shared_ptr<GunMetadata>>   knownGuns;
        std::map<ShootingEventMetadata, std::shared_ptr<ShootingEvent>>         events;

        std::map<Manufacturer,  std::shared_ptr<Manufacturer>>  manufacturers;
        std::map<Cartridge,     std::shared_ptr<Cartridge>>     cartridges;
        std::map<Location,      std::shared_ptr<Location>>      locations;
        std::map<ShootingEventType,     std::shared_ptr<ShootingEventType>>     eventTypes;
        std::map<WeaponType,    std::shared_ptr<WeaponType>>    weaponTypes;


        std::map<Cartridge, std::map<AmmoMetadata,  std::shared_ptr<AssociatedAmmo>>> ammoStockpile;
        std::map<Cartridge, int> amountPerCartridge;

        std::map<Cartridge, std::map<GunMetadata,   std::shared_ptr<AssociatedGun>>>  gunsInArmory;

        
        bool addAmountPerCartridge(const Cartridge& cartridge, int amount);
    };


    namespace ModelEvents {

        // Full Objects
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

    }
}
