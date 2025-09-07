#pragma once

#include "Ammo.h" // Needs cartridge definition

#include <LAS/json.h>

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace ShooterCentral{

    class WeaponType{
    public:
        explicit WeaponType(std::string setName="NULL");
        ~WeaponType();

        std::string getName() const;
        operator std::string() const;
        bool operator== (const WeaponType& other) const;
        bool operator<  (const WeaponType& other) const;

    private:
        std::string name;
    };

    struct GunMetadata {
        std::string         name        { "N/A" };
        bool                isActive    { false }; 
        const Cartridge&    cartridge;
        const WeaponType&   weaponType;

        bool operator== (const GunMetadata& other) const;
        bool operator<  (const GunMetadata& other) const;
    };

    void to_json(LAS::json& j, const GunMetadata& gun);

    class GunAndAmmo {
    public:
        GunAndAmmo(const GunMetadata& setGun);
        ~GunAndAmmo();

        bool    addAmmoUsed     (const AmountOfAmmo& ammo);
        bool    hasUsedAmmo     (const AmmoMetadata& ammo) const;
        int     totalAmmoUsed   () const;
        int     totalRoundsShot () const;

        const GunMetadata& getGunInfo() const;

        const std::vector<AmountOfAmmo>& getAmmoUsedList() const;
    private:
        const GunMetadata&        gun;
        std::vector<AmountOfAmmo> ammoUsedList { };

        int totalRoundCount { 0 };
    };

}
