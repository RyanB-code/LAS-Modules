#pragma once

#include <string>

#include "Ammo.h" // Needs cartridge definition

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

}
