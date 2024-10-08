#pragma once

#include "Ammo.h"

#include <string>
#include <stdint.h>
#include <memory>
#include <unordered_map>

//MARK: GUN
namespace ShooterCentral{

    class WeaponType {
    public:
        WeaponType(std::string setName);
        ~WeaponType();

        std::string getName() const;

        operator std::string() const;
        bool operator==(const WeaponType& other) const;
        friend std::ostream& operator<<(std::ostream& stream, const WeaponType& weaponType);

    private:
        std::string name;
    };

    namespace WeaponTypes{
        const WeaponType PISTOL             { "PISTOL" };
        const WeaponType RIFLE              { "RIFLE" };
        const WeaponType PRECISION_RIFLE    { "PRECISION RIFLE" };
        const WeaponType WT_NA              { "N/A" };
    }


    class Gun final {
    public:
        Gun();
        Gun(std::string setName, WeaponType setWeaponType, std::string setCartridge);
        ~Gun();

        std::string getName         () const;
        uint64_t    getRoundCount   () const;
        WeaponType  getWeaponType   () const;
        std::string getCartridge    () const;

        bool addToRoundCount (uint64_t amount, const AmmoType& ammoType);

        void getAllAmmoUsed (std::vector<TrackedAmmo>& ammoUsed) const; // Clears vector before adding elements

        bool operator==(const Gun& other) const;

    private:
        std::string name;
        WeaponType  weaponType;
        std::string cartridge;

        std::unordered_map<AmmoType, TrackedAmmoPtr> ammoTracker;
    };

    void to_json    (LAS::json& j, const Gun& gun);
    void from_json  (const LAS::json& j, Gun& gun); 
}
namespace std{
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
    template <>
    struct hash<ShooterCentral::WeaponType> {
        size_t operator()(const ShooterCentral::WeaponType& wt) const{
            return std::hash<std::string>{}(wt.getName());
        }
    };

}

namespace ShooterCentral{

    using GunPtr = std::shared_ptr<Gun>;

    class GunTracker{
    public:
        GunTracker(LAS::Logging::LoggerPtr setLogger);
        ~GunTracker();

        bool        setDirectory    (std::string directory);
        std::string getDirectory    () const;

        GunPtr  createPistol            (const std::string& name, const std::string& cartridge);   
        GunPtr  createRifle             (const std::string& name, const std::string& cartridge);
        GunPtr  createPrecisionRifle    (const std::string& name, const std::string& cartridge);
        bool    removeGun               (const Gun& gun);

        uint64_t    getGunTotal ()                          const;
        void        getAllGuns  (std::vector<Gun>& list)    const;  // Clears vector before adding elements 

        bool writeAllGuns   () const;
        bool readGuns       ();

    private:
        LAS::Logging::LoggerPtr     logger;

        std::unordered_map<Gun, GunPtr> guns;

        std::string saveDirectory;

        bool addGun(Gun& gun);
    };

    namespace GunHelper{
        bool    writeGun    (std::string directory, const Gun& gun);
        Gun     readGun     (const std::string& path);
    }

    using GunTrackerPtr = std::shared_ptr<GunTracker>;
}