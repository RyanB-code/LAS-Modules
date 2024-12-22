#pragma once

#include "Ammo.h"

#include <string>
#include <stdint.h>
#include <memory>
#include <unordered_map>

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
        Gun();
        Gun(std::string setName, WeaponType setWeaponType, Cartridge setCartridge);
        ~Gun();

        std::string getName         () const;
        uint64_t    getRoundCount   () const;
        WeaponType  getWeaponType   () const;
        Cartridge   getCartridge    () const;

        bool addToRoundCount (int amount, const AmmoType& ammoType);

        void getAllAmmoUsed (std::vector<TrackedAmmo>& ammoUsed) const; // Clears vector before adding elements

        bool operator==(const Gun& other) const;

    private:
        std::string name;
        WeaponType  weaponType;
        Cartridge   cartridge;

        std::unordered_map<AmmoType, TrackedAmmoPtr> ammoTracker;
    };

    void to_json    (LAS::json& j, const Gun& gun);
    void from_json  (const LAS::json& j, Gun& gun); 
}
namespace std{
    template <>
    struct hash<ShooterCentral::WeaponType> {
        size_t operator()(const ShooterCentral::WeaponType& wt) const{
            return std::hash<std::string>()(wt.getName());
        }
    };
    template <>
    struct hash<ShooterCentral::Gun> {
        size_t operator()(const ShooterCentral::Gun& gun) const{
            std::size_t seed { 0 };

            std::hash_combine(seed, gun.getName());
            std::hash_combine(seed, gun.getWeaponType());
            std::hash_combine(seed, gun.getCartridge());

            return seed;
        }
    };
}

namespace ShooterCentral{

    using GunPtr            = std::shared_ptr<Gun>;
    using ConstGunPtr       = std::shared_ptr<const Gun>;
    using ConstGunPtrList   = std::vector<ConstGunPtr>;
    class GunTracker{
    public:
        GunTracker(LAS::Logging::LoggerPtr setLogger);
        ~GunTracker();

        bool        setDirectory    (std::string directory);
        std::string getDirectory    () const;

        GunPtr  createGun       (const std::string& name, const WeaponType& weaponType, const Cartridge& cartridge);
        bool    addGun          (Gun& gun);
        bool    addGun          (GunPtr gun);
        bool    addWeaponType   (const WeaponType& type);

        bool    removeGun       (const Gun& gun);

        void    getRoundsShotPerCartridge   (std::unordered_map<std::string, uint64_t>& list)   const;  // Clears vector before adding elements
        void    getAllWeaponTypeNames       (WeaponTypeList& names)                             const;  // Clears vector before adding elements
        int     getGunTotal                 ()                                                  const;
        void    getAllGuns                  (std::vector<ConstGunPtr>& list)                    const;  // Clears vector before adding elements 

        bool writeAllGuns           () const;
        bool writeAllWeaponTypes    () const;

        bool readGuns           ();
        bool readWeaponTypes    ();

    private:
        LAS::Logging::LoggerPtr             logger;
        std::unordered_map<Gun, GunPtr>     guns;
        std::map<std::string, WeaponType>   weaponTypes;

        std::string saveDirectory;

        static constexpr std::string_view WEAPON_TYPES_FILENAME {"WeaponTypes.json"};
    };

    namespace GunHelper{
        bool    writeGun            (std::string directory, const Gun& gun);                // Creates a file for the gun
        bool    writeAllWeaponTypes (std::string path,      const WeaponTypeList& list);    // Takes a list because they are all saved in the same file

        bool    readWeaponTypes     (std::string path, WeaponTypeList& list);       // Clears vector before adding elements
        Gun     readGun             (const std::string& path);

    }

    using GunTrackerPtr = std::shared_ptr<GunTracker>;
}