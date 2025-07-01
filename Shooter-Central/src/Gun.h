#pragma once

#include "Ammo.h" // Needs cartridge definition

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
        bool operator==(const WeaponType& other) const;

    private:
        std::string name;
    };

    class Gun final {
    public:
        Gun(std::string setName="N/A", WeaponType setWeaponType=WeaponType{ }, Cartridge setCartridge=Cartridge{ });
        ~Gun();

        std::string getName         () const;
        WeaponType  getWeaponType   () const;
        Cartridge   getCartridge    () const;
        bool        isActive        () const;

        bool operator== (const Gun& other) const;
        bool operator<  (const Gun& other) const;

    private:
        std::string name        { };
        WeaponType  weaponType  { };
        Cartridge   cartridge   { };
        bool        m_isActive    { false }; 
    };

    class GunAndAmmo {
    private:
        static constexpr int MAX_NUM_AMMO_USED { 10 };
    public:
        GunAndAmmo(std::shared_ptr<const Gun> setGun);
        ~GunAndAmmo();

        bool    addAmmoUsed     (const AmountOfAmmo& ammo);
        bool    hasUsedAmmo     (const Ammo& ammo) const;
        int     totalAmmoUsed   () const;

        const Gun& getGun() const;

        std::array<AmountOfAmmo, MAX_NUM_AMMO_USED>::const_iterator cbegin() const;
        std::array<AmountOfAmmo, MAX_NUM_AMMO_USED>::const_iterator cend() const;
    private:
        std::shared_ptr<const Gun>                  gun;
        std::array<AmountOfAmmo, MAX_NUM_AMMO_USED> ammoUsedList;
        int nextIndex { 0 };
    };

}
