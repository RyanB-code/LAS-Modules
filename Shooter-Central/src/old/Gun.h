#pragma once

#include "Ammo.h"

#include <string>
#include <stdint.h>
#include <memory>
#include <map>
#include <tuple>

//MARK: GUN
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

    std::ostream& operator<<(std::ostream& os, const WeaponType& weaponType);

    using WeaponTypeList = std::vector<WeaponType>;


    class Gun final {
    public:
        Gun(std::string setName="N/A", WeaponType setWeaponType=WeaponType{ }, Cartridge setCartridge=Cartridge{ });
        ~Gun();

        std::string getName         () const;
        int         getRoundCount   () const;
        WeaponType  getWeaponType   () const;
        Cartridge   getCartridge    () const;

        bool addToRoundCount (TrackedAmmo trackedAmmo);

        void getAllAmmoUsed (ConstTrackedAmmoPtrList& ammoUsed) const; // Clears vector before adding elements

        bool operator== (const Gun& other) const;
        bool operator<  (const Gun& other) const;


    private:
        std::string name;
        WeaponType  weaponType;
        Cartridge   cartridge;

        std::map<AmmoType, TrackedAmmo> ammoTracker;
    };


    void to_json    (LAS::json& j, const Gun& gun);
    void from_json  (const LAS::json& j, Gun& gun); 

    using GunPtr            = std::shared_ptr<Gun>;
    using ConstGunPtr       = std::shared_ptr<const Gun>;
    using ConstGunPtrList   = std::vector<ConstGunPtr>;


    class GunTracker{
    public:
        GunTracker();
        ~GunTracker();

        bool        setDirectory    (std::string directory);
        std::string getDirectory    () const;

        GunPtr                      createGun       (const std::string& name, const WeaponType& weaponType, const Cartridge& cartridge);
        bool                        addWeaponType   (const WeaponType& type);
        std::pair<GunPtr, bool>     addGun          (Gun& gun);       // Return a pair, first is ptr to possibly inserted item, second is whether the item was inserted

        bool    removeGun       (const Gun& gun);

        bool    contains                    (const Gun& gun) const;

        GunPtr  getGun                      (const Gun& gun);
        void    getRoundsShotPerCartridge   (std::unordered_map<std::string, uint64_t>& list)   const;  // Clears vector before adding elements
        void    getAllWeaponTypeNames       (WeaponTypeList& names)                             const;  // Clears vector before adding elements
        void    getAllGuns                  (std::vector<ConstGunPtr>& list)                    const;  // Clears vector before adding elements 
        int     getGunTotal                 ()                                                  const;

        bool writeAllGuns           () const;
        bool writeAllWeaponTypes    () const;

        bool readGuns           ();
        bool readWeaponTypes    ();

    private:
        std::map<Gun, GunPtr>               guns;
        std::map<std::string, WeaponType>   weaponTypes;

        std::string saveDirectory;

        static constexpr std::string_view WEAPON_TYPES_FILENAME {"WeaponTypes.json"};
    };
    using GunTrackerPtr = std::shared_ptr<GunTracker>;



    namespace GunHelper{
        std::string makeFileName    (std::string directory, const Gun& gun);

        bool    writeGun            (std::string directory, const Gun& gun);                // Creates a file for the gun
        bool    writeAllWeaponTypes (std::string path,      const WeaponTypeList& list);    // Takes a list because they are all saved in the same file

        bool    readWeaponTypes     (std::string path, WeaponTypeList& list);       // Clears vector before adding elements
        Gun     readGun             (const std::string& path);

    }
}
