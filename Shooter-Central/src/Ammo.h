#pragma once

#include <string>
#include <memory>

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

    struct AmmoMetadata {
        std::string     name            { };
        Manufacturer    manufacturer    { };
        Cartridge       cartridge       { };     
        int             grainWeight     { 0 };
        bool            isActive        { true };
 
        bool operator== (const AmmoMetadata& other) const;
        bool operator<  (const AmmoMetadata& other) const;
    };

    class AmountOfAmmo {
    public:
        AmountOfAmmo(std::shared_ptr<const AmmoMetadata> setAmmo=nullptr, int setAmount=0);
        ~AmountOfAmmo();

        operator bool() const;

        int getAmount() const;
        
        void addAmount(int amount);

        const AmmoMetadata& getAmmo() const;
    private:
        std::shared_ptr<const AmmoMetadata> ammo { };
        int amount  { 0 };

         void throwIfInvalid() const; // Throws if any operation is attempted if ammo is nullptr
    };

}
