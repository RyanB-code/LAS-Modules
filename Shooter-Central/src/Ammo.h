#pragma once

#include <string>

namespace ShooterCentral{

    class Manufacturer{
    public:
        explicit Manufacturer(std::string setName="NULL");
        ~Manufacturer();

        std::string getName() const;
        operator std::string() const;
        bool operator==(const Manufacturer& other) const;

    private:
        std::string name;
    };

    class Cartridge{
    public:
        explicit Cartridge(std::string setName="NULL");
        ~Cartridge();

        std::string getName() const;
        operator std::string() const;
        bool operator==(const Cartridge& other) const;

    private:
        std::string name;
    };

    struct Ammo {
        std::string     name            { };
        Manufacturer    manufacturer    { };
        Cartridge       cartridge       { };     
        int             grainWeight     { 0 };
        bool            isActive        { true };
 
        bool operator== (const Ammo& other) const;
        bool operator<  (const Ammo& other) const;
    };

    struct AmountOfAmmo {
        Ammo    ammo    { };
        int     amount  { 0 };
    };
}
