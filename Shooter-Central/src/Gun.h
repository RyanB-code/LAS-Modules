#pragma once

#include "Ammo.h"

#include <string>
#include <stdint.h>
#include <memory>
#include <unordered_map>

//MARK: GUN
namespace ShooterCentral{
    namespace WeaponTypes{
        const std::string PISTOL             { "PISTOL" };
        const std::string RIFLE              { "RIFLE" };
        const std::string PRECISION_RIFLE    { "PRECISION RIFLE" };
        const std::string WT_NA              { "N/A" };
    }


    class Gun final {
    public:
        Gun();
        Gun(std::string setName, std::string setWeaponType, std::string setCartridge);
        ~Gun();

        std::string getName         () const;
        uint64_t    getRoundCount   () const;
        std::string getWeaponType   () const;
        std::string getCartridge    () const;

        bool addToRoundCount (uint64_t amount, const AmmoType& ammoType);

        void getAllAmmoUsed (std::vector<TrackedAmmo>& ammoUsed) const; // Clears vector before adding elements

        bool operator==(const Gun& other) const;

    private:
        std::string name;
        std::string weaponType;
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
        GunPtr  createGun               (const std::string& name, const std::string& weaponType, const std::string& cartridge);
        bool    removeGun               (const Gun& gun);

        void    getRoundsShotPerCartridge   (std::unordered_map<std::string, uint64_t>& list) const;

        uint64_t    getGunTotal ()                          const;
        void        getAllGuns  (std::vector<Gun>& list)    const;      // Clears vector before adding elements 

        void    getAllWeaponTypeNames   (StringVector& names) const;   // Clears vector before adding elements
        bool    addWeaponType           (const std::string& type);


        bool writeAllGuns   () const;
        bool readGuns       ();

    private:
        LAS::Logging::LoggerPtr             logger;
        std::unordered_map<Gun, GunPtr>     guns;
        std::map<std::string, std::string> weaponTypes;

        std::string saveDirectory;

        bool addGun(Gun& gun);
    };

    namespace GunHelper{
        bool    writeGun    (std::string directory, const Gun& gun);
        Gun     readGun     (const std::string& path);
    }

    using GunTrackerPtr = std::shared_ptr<GunTracker>;
}