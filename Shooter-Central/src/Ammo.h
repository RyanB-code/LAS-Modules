#pragma once

#include <LAS/json.h>
#include <LAS/HelperFunctions.h>
#include <LAS/Logging.h>

#include <stdint.h>
#include <string>
#include <memory>
#include <map>
#include <vector>

#include <iostream>


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

    std::ostream& operator<<(std::ostream& os, const Manufacturer& manufactuerer);

    using ManufacturerList = std::vector<Manufacturer>;

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

    std::ostream& operator<<(std::ostream& os, const Cartridge& cartridge);

    using CartridgeList             = std::vector<Cartridge>;
    using AmountPerCartridgeList    = std::vector<std::pair<Cartridge, int>>;




    struct AmmoType {
        std::string     name            { };
        Manufacturer    manufacturer    { };
        Cartridge       cartridge       { };     
        int             grainWeight     { 0 };

        bool operator==(const AmmoType& other) const;
    };

    void to_json    (LAS::json& j, const AmmoType& ammoType);
    void from_json  (const LAS::json& j, AmmoType& ammoType);

    using AmmoTypePtr   = std::shared_ptr<AmmoType>;

    struct TrackedAmmo {
        AmmoType    ammoType    { };
        int         amount      { 0 };

        bool operator==(const TrackedAmmo& other) const;
    };

    void to_json    (LAS::json& j, const TrackedAmmo& ammo);
    void from_json  (const LAS::json& j, TrackedAmmo& ammo);

    using TrackedAmmoPtr            = std::shared_ptr<TrackedAmmo>;
    using ConstTrackedAmmoPtr       = std::shared_ptr<const TrackedAmmo>;
    using ConstTrackedAmmoPtrList   = std::vector<ConstTrackedAmmoPtr>;

}

namespace std{
    template <>
    struct hash<ShooterCentral::Manufacturer> {
        size_t operator()(const ShooterCentral::Manufacturer& manufacturer) const{
            return std::hash<std::string>()(manufacturer.getName());
        }
    };
    template<>
    struct hash<ShooterCentral::Cartridge> {
        size_t operator()(const ShooterCentral::Cartridge& cartridge) const{
            return std::hash<std::string>()(cartridge.getName());
        }
    };

    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    template <>
    struct hash<ShooterCentral::AmmoType> {
        size_t operator()(const ShooterCentral::AmmoType& ammo) const{
            std::size_t seed { 0 };

            std::hash_combine(seed, ammo.name);
            std::hash_combine(seed, ammo.manufacturer);
            std::hash_combine(seed, ammo.cartridge);
            std::hash_combine(seed, ammo.grainWeight);

            return seed;
        }
    };
}

namespace ShooterCentral{

    class AmmoTracker{
    public:
        AmmoTracker(LAS::Logging::LoggerPtr setLogger);
        ~AmmoTracker();

        bool    addAmmoToStockpile          (const TrackedAmmo& trackedAmmo);
        bool    addCartridge            (const std::string& cartridge);
        bool    addManufacturer         (const std::string& manufacturer);

        bool    removeAmmoFromStockpile     (const TrackedAmmo& trackedAmmo);
        void    removeAllAmmoFromStockpile  ();

        void    getAllAmmoNames         (StringVector&                      names)                              const;  // Clears vector before adding elements
        void    getAllAmmo              (std::vector<ConstTrackedAmmoPtr>&  list)                               const;  // Clears vector before adding elements
        void    getAllCartridges        (CartridgeList&                     list)                               const;  // Clears vector before adding elements
        void    getAllManufacturers     (ManufacturerList&                  list)                               const;  // Clears vector before adding elements
        void    getAmmoCountByCartridge (AmountPerCartridgeList&            list)                               const;  // Clears vector before adding elements
        void    getAllAmmoByCartridge   (std::vector<TrackedAmmo>&          list, const Cartridge& cartridge)   const;  // Clears vector before adding elements, gives copies of ammo objects, adds if cartridge name is equal


        bool    writeAllAmmo            () const;
        bool    writeAllCartridges      () const;
        bool    writeAllManufacturers   () const;

        bool    readCartridges          ();
        bool    readAllAmmo             ();
        bool    readManufacturers       ();


        bool        setDirectory        (std::string directory);
        std::string getDirectory        () const;

    private:
        std::unordered_map<AmmoType, TrackedAmmoPtr>    ammoStockpile;
        std::map<std::string, Cartridge>                cartridges;
        std::map<std::string, Manufacturer>             manufacturers;


        std::string             saveDirectory;
        LAS::Logging::LoggerPtr logger;

        static constexpr std::string_view CARTRIDGES_FILENAME    {"Cartridges.json"};
        static constexpr std::string_view MANUFACTURERS_FILENAME {"Manufacturers.json"};

    };

    using AmmoTrackerPtr = std::shared_ptr<AmmoTracker>;

    namespace AmmoHelper{
        bool        writeTrackedAmmo        (std::string directory, const TrackedAmmo& ammo);
        bool        writeAllCartridges      (std::string path, const StringVector& cartridges);
        bool        writeAllManufacturers   (std::string path, const StringVector& manufacturers);

        TrackedAmmo readTrackedAmmo         (const std::string& path);
        bool        readCartridges          (std::string path, StringVector& cartridgeNames);       // Clears vector before adding elements
        bool        readManufacturers       (std::string path, StringVector& manufacturers);        // Clears vector before adding elements
    }

}