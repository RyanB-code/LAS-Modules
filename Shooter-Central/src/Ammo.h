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

    struct AmmoType {
        std::string     name            { };
        std::string     manufacturer    { };
        std::string     cartridge       { };     
        uint8_t         grainWeight     { 0 };

        bool operator==(const AmmoType& other) const;
    };

    void to_json    (LAS::json& j, const AmmoType& ammoType);
    void from_json  (const LAS::json& j, AmmoType& ammoType);

    using AmmoTypePtr   = std::shared_ptr<AmmoType>;

    struct TrackedAmmo {
        AmmoType    ammoType    { };
        int         amount      { 0 };
    };

    void to_json    (LAS::json& j, const TrackedAmmo& ammo);
    void from_json  (const LAS::json& j, TrackedAmmo& ammo);

    using TrackedAmmoPtr        = std::shared_ptr<TrackedAmmo>;
    using ConstTrackedAmmoPtr   = std::shared_ptr<const TrackedAmmo>;

}

namespace std{

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
        bool    removeAmmoFromStockpile     (const TrackedAmmo& trackedAmmo);
        void    removeAllAmmoFromStockpile  ();

        void    getAllAmmoNames         (StringVector& names)                       const;          // Clears vector before adding elements
        void    getAllAmmo              (std::vector<ConstTrackedAmmoPtr>& list)    const;          // Clears vector before adding elements

        void    getAllCartridgeNames    (StringVector& names) const;                                                // Clears vector before adding elements
        void    getAmmoCountByCartridge (std::vector<std::pair<std::string, uint64_t>>& count) const;               // Clears vector before adding elements
        void    getAllAmmoByCartridge   (std::vector<TrackedAmmo>& list, const std::string& cartridgeName)   const; // Clears vector before adding elements, gives copies of ammo objects, adds if cartridge name is equal
        void    getAllManufacturerNames (StringVector& list) const;                                                 // Clears vector before adding elements

        bool    addCartridge            (const std::string& cartridge);
        bool    addManufacturer         (const std::string& manufacturer);

        bool    writeAllAmmo            () const;
        bool    readAllAmmo             ();

        bool    writeAllCartridges      () const;
        bool    readCartridges          ();
    
        bool    writeAllManufacturers   () const;
        bool    readManufacturers       ();

        bool        setDirectory        (std::string directory);
        std::string getDirectory        () const;

    private:
        std::unordered_map<AmmoType, TrackedAmmoPtr>    ammoStockpile;
        std::map<std::string, std::string>              cartridges;
        std::map<std::string, std::string>              manufacturers;


        std::string             saveDirectory;
        LAS::Logging::LoggerPtr logger;

        static constexpr std::string_view CARTRIDGES_FILENAME    {"Cartridges.json"};
        static constexpr std::string_view MANUFACTURERS_FILENAME {"Manufacturers.json"};

    };

    using AmmoTrackerPtr = std::shared_ptr<AmmoTracker>;

    namespace AmmoHelper{
        bool        writeTrackedAmmo    (std::string directory, const TrackedAmmo& ammo);
        TrackedAmmo readTrackedAmmo     (const std::string& path);

        bool    writeAllCartridges      (std::string path, const StringVector& cartridges);
        bool    readCartridges          (std::string path, StringVector& cartridgeNames);               // Clears vector before adding elements

        bool    writeAllManufacturers   (std::string path, const StringVector& manufacturers);
        bool    readManufacturers       (std::string path, StringVector& manufacturers);               // Clears vector before adding elements
    }

}