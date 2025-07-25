#pragma once

#include "Ammo.h" // Needs cartridge definition

#include <LAS/json.h>

#include <array>
#include <string>
#include <memory>

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
        std::string name        { "N/A" };
        WeaponType  weaponType  { };
        Cartridge   cartridge   { };
        bool        isActive    { false }; 

        bool operator== (const GunMetadata& other) const;
        bool operator<  (const GunMetadata& other) const;
    };

    void to_json(LAS::json& j, const GunMetadata& gun);
    void from_json(const LAS::json& j, GunMetadata& gun);


    class GunAndAmmo {
    private:
        static constexpr int MAX_NUM_AMMO_USED { 10 };
    public:
        GunAndAmmo(std::shared_ptr<const GunMetadata> setGun=nullptr);
        ~GunAndAmmo();

        bool    addAmmoUsed     (const AmountOfAmmo& ammo);             // Throws in gun ptr is not set
        bool    hasUsedAmmo     (const AmmoMetadata& ammo) const;
        int     totalAmmoUsed   () const;

        const GunMetadata& getGun() const;                              // Throws if gun ptr is not set

        operator bool() const;

        std::array<AmountOfAmmo, MAX_NUM_AMMO_USED>::const_iterator cbegin() const;
        std::array<AmountOfAmmo, MAX_NUM_AMMO_USED>::const_iterator cend() const;
    private:
        std::shared_ptr<const GunMetadata>                  gun { };
        std::array<AmountOfAmmo, MAX_NUM_AMMO_USED> ammoUsedList { };
        int nextIndex { 0 };

        void throwIfInvalid() const; // Throws if any operation is attempted if gun is nullptr
    };

}
