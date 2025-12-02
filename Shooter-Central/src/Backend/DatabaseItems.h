#pragma once


#include <string>
#include <cstring>      // strncpy
#include <stdexcept>    // throws
#include <set>
#include <memory>
#include <map>
#include <chrono>
#include <vector>

using timepoint = std::chrono::system_clock::time_point;
using ymd       = std::chrono::year_month_day;

namespace ShooterCentral{

// Constants
static constexpr int MAX_CHAR_METADATA_ITEM { 32 };


// Forward Declarations
class ShootingEvent;


// Objects
class Manufacturer{
public:
    explicit Manufacturer(const char* setName=nullptr);
    ~Manufacturer() = default;

    const char* getName() const;
    bool setName(const char* set);

    bool operator== (const Manufacturer& other) const;
    bool operator<  (const Manufacturer& other) const;

private:
    char name[MAX_CHAR_METADATA_ITEM] = "";
};

class Cartridge{
public:
    explicit Cartridge(const char* setName=nullptr);
    ~Cartridge() = default;

    const char* getName() const;
    bool setName(const char* set);

    bool operator== (const Cartridge& other) const;
    bool operator<  (const Cartridge& other) const;

private:
    char name[MAX_CHAR_METADATA_ITEM] = "";
};

class WeaponType{
public:
    explicit WeaponType(const char* setName=nullptr);
    ~WeaponType() = default;

    const char* getName() const;
    bool setName(const char* set);

    bool operator== (const WeaponType& other) const;
    bool operator<  (const WeaponType& other) const;

private:
    char name[MAX_CHAR_METADATA_ITEM] = "";
};

class Location{
public:
    explicit Location(const char* setName=nullptr);
    ~Location() = default;

    const char* getName() const;
    bool setName(const char* set);

    bool operator== (const Location& other) const;
    bool operator<  (const Location& other) const;

private:
    char name[MAX_CHAR_METADATA_ITEM] = "";
};

class ShootingEventType{
public:
    explicit ShootingEventType(const char* setName=nullptr);
    ~ShootingEventType() = default;

    const char* getName() const;
    bool setName(const char* set);

    bool operator== (const ShootingEventType& other) const;
    bool operator<  (const ShootingEventType& other) const;

private:
    char name[MAX_CHAR_METADATA_ITEM] = "";
};




struct ShootingEventMetadata {
    std::string         notes       { };
    Location            location    { };
    ShootingEventType   eventType   { };

    ymd date { 
        std::chrono::sys_days{  
            std::chrono::year_month_day{
                std::chrono::year{0}, 
                std::chrono::month{0}, 
                std::chrono::day{0}
            }
        }
    };

    bool operator== (const ShootingEventMetadata& other) const;
    bool operator<  (const ShootingEventMetadata& other) const;
};

struct AmmoMetadata {
    std::string     name            { };
    Cartridge       cartridge       { };
    Manufacturer    manufacturer    { };
    int             grainWeight     { 0 };

    bool operator== (const AmmoMetadata& other) const;
    bool operator<  (const AmmoMetadata& other) const;
};



struct GunMetadata {
    std::string name        { };
    Cartridge   cartridge   { };
    WeaponType  weaponType  { };

    bool operator== (const GunMetadata& other) const;
    bool operator<  (const GunMetadata& other) const;
};




class AmountOfAmmo {
public:
    AmountOfAmmo(const AmmoMetadata& setAmmo=AmmoMetadata{ }, int amount=0);
    ~AmountOfAmmo() = default;

    int getAmount() const;
    void addAmount(int amount);

    const AmmoMetadata& getAmmoInfo() const;
private:
    AmmoMetadata ammoInfo { };
    int amount  { 0 };

};



class StockpileAmmo {
public:
    StockpileAmmo(const AmountOfAmmo& setAmountofAmmo);
    StockpileAmmo(const AmmoMetadata& setAmmoInfo = AmmoMetadata{ });
    ~StockpileAmmo() = default;

    void addAmount      (int amount);
    void removeAmount   (int amount);

    bool addGun     (const GunMetadata& gun); // If key already exists, returns false since data is not overwritten
    bool removeGun  (const GunMetadata& gun);
    bool hasGun     (const GunMetadata& gun) const;

    void setActive  (bool set) { m_isActive = set; }

    int totalGuns   () const;

    bool        isActive()          const { return m_isActive; }
    int         getAmountOnHand()   const { return amountOnHand; }
    const auto& getAmmoInfo()       const { return ammoInfo; }
    const auto& getGunsUsed()       const { return gunsUsed; }
    const auto  getAmountOfAmmo()   const { return AmountOfAmmo { ammoInfo, amountOnHand }; }
private:
    AmmoMetadata            ammoInfo        { };
    int                     amountOnHand    { 0 };
    bool                    m_isActive      { false };
    std::set<GunMetadata>   gunsUsed;
};



class GunTrackingAmmoUsed {
public:
    GunTrackingAmmoUsed(const GunMetadata& setGun=GunMetadata{ } );
    ~GunTrackingAmmoUsed() = default;

    bool    addAmmoUsed     (const AmountOfAmmo& ammo);
    bool    removeAmmoUsed  (const AmmoMetadata& ammo);
    bool    hasUsedAmmo     (const AmmoMetadata& ammo) const;
    int     totalAmmoUsed   () const;
    int     totalRoundsShot () const;

    const GunMetadata& getGunInfo() const;

    const auto& getAmmoUsed() const { return ammoUsed; }
private:
    GunMetadata                 gun         { };
    std::vector<AmountOfAmmo>   ammoUsed    { };

    int roundCount { 0 };
};

class ArmoryGun {
public:
    ArmoryGun(const GunMetadata& setGun=GunMetadata{ } );
    ~ArmoryGun() = default;

    const GunMetadata& getGunInfo() const;

    int getRoundCount()         const;
    int totalEventsUsed()       const;
    int totalAmmoTypesUsed()    const;

    bool addAmmoUsed    (const AmountOfAmmo& ammo);
    /* bool removeAmmoUsed (const AmmoMetadata& ammo); */
    bool hasUsedAmmo    (const AmmoMetadata& ammo) const;

    bool addEvent       (const ShootingEvent& event);           // If key already exists, returns false since data is not overwritten 
    bool wasUsedInEvent (const ShootingEventMetadata& data);

    void        setActive(bool set)     { m_isActive = set; }

    bool        isActive()      const   { return m_isActive; }
    const auto& getAmmoUsed()   const   { return ammoUsed; }
    const auto& getEventsUsed() const   { return eventsUsed; }
private:
    GunMetadata gun         { };
    bool        m_isActive    { false };
    int         roundCount  { 0 };

    std::set<ShootingEventMetadata>         eventsUsed  { };
    std::map<AmmoMetadata, AmountOfAmmo>    ammoUsed    { };
};



class ShootingEvent {
public:
    ShootingEvent(const ShootingEventMetadata& setInfo=ShootingEventMetadata{ } );
    ~ShootingEvent() = default;

    void    setInfo (const ShootingEventMetadata& info) { eventInfo = info; }

    int     totalGunsUsed   () const;
    bool    addGun          (const GunTrackingAmmoUsed& gun);        // Will not amend existing entries if a gun is already in the container
    bool    removeGun       (const GunMetadata& gun);
    bool    hasUsedGun      (const GunMetadata& gun) const;

    GunTrackingAmmoUsed&        getGun (const GunMetadata& gun);           // throws out_of_range if not found
    const GunTrackingAmmoUsed&  getGun (const GunMetadata& gun) const;

    std::string printDate()     const;

    const auto& getInfo() const     { return eventInfo; }
    const auto& getGunsUsed() const { return gunsUsed; }
private:
    ShootingEventMetadata eventInfo;

    std::vector<GunTrackingAmmoUsed> gunsUsed;
};



// Helping Functions
std::chrono::system_clock::time_point stringToTimepoint(const std::string& timeString);
std::string printDate(const std::chrono::year_month_day&);

}   // End SC namespace
