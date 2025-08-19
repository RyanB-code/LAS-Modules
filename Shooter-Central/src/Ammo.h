#pragma once

#include <LAS/json.h>

#include <string>
#include <memory>

namespace ShooterCentral{

    class Manufacturer{
    public:
        explicit Manufacturer(std::string setName="NULL");
        ~Manufacturer();

        std::string getName() const;

        operator std::string() const;
        bool operator== (const Manufacturer& other) const;
        bool operator<  (const Manufacturer& other) const;

    private:
        std::string name;
    };

    class Cartridge{
    public:
        explicit Cartridge(std::string setName="NULL");
        ~Cartridge();

        std::string getName() const;
        operator std::string() const;
        bool operator== (const Cartridge& other) const;
        bool operator<  (const Cartridge& other) const;

    private:
        std::string name;
    };

    struct AmmoMetadata {
        std::string     name            { };
        int             grainWeight     { 0 };
        bool            isActive        { true };

        const Manufacturer&    manufacturer;
        const Cartridge&       cartridge;
 
        bool operator== (const AmmoMetadata& other) const;
        bool operator<  (const AmmoMetadata& other) const;
    };

    void to_json (LAS::json& j, const AmmoMetadata& data);

    class AmountOfAmmo {
    public:
        AmountOfAmmo(const AmmoMetadata& setAmmo, int amount=0);
        ~AmountOfAmmo();

        int getAmount() const;
        void addAmount(int amount);

        const AmmoMetadata& getAmmoInfo() const;
    private:
        const AmmoMetadata& ammo;
        int amount  { 0 };

    };

    void to_json(LAS::json& j, const AmountOfAmmo& data);
}
