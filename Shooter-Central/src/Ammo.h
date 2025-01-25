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


    struct AmmoType {
        std::string     name            { };
        Manufacturer    manufacturer    { };
        Cartridge       cartridge       { };     
        int             grainWeight     { 0 };

        bool operator== (const AmmoType& other) const;
        bool operator<  (const AmmoType& other) const;
    };


    void to_json    (LAS::json& j, const AmmoType& ammoType);
    void from_json  (const LAS::json& j, AmmoType& ammoType);

    struct TrackedAmmo {
        AmmoType    ammoType    { };
        int         amount      { 0 };

        bool operator==(const TrackedAmmo& other) const;
    };

    void to_json    (LAS::json& j, const TrackedAmmo& ammo);
    void from_json  (const LAS::json& j, TrackedAmmo& ammo);


    using ManufacturerList          = std::vector<Manufacturer>;
    using CartridgeList             = std::vector<Cartridge>;
    using AmountPerCartridgeList    = std::vector<std::pair<Cartridge, int>>;
    using AmmoTypePtr               = std::shared_ptr<AmmoType>;

    using TrackedAmmoPtr            = std::shared_ptr<TrackedAmmo>;
    using ConstTrackedAmmoPtr       = std::shared_ptr<const TrackedAmmo>;
    using ConstTrackedAmmoPtrList   = std::vector<ConstTrackedAmmoPtr>;


    class AmmoTracker{
    public:
        AmmoTracker(LAS::Logging::LoggerPtr setLogger);
        ~AmmoTracker();

        bool    addAmmoToStockpile          (const TrackedAmmo& trackedAmmo);
        bool    addCartridge                (const std::string& cartridge);
        bool    addManufacturer             (const std::string& manufacturer);

        bool    removeAmmoFromStockpile     (const TrackedAmmo& trackedAmmo);
        void    removeAllAmmoFromStockpile  ();

        bool    contains                (const AmmoType& ammoType) const;

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
        std::map<AmmoType, TrackedAmmoPtr>  ammoStockpile;
        std::map<std::string, Cartridge>    cartridges;
        std::map<std::string, Manufacturer> manufacturers;


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