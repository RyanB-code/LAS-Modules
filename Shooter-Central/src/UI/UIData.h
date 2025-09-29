#pragma once

#include "Containers.h"

#include <memory>

namespace ShooterCentral::UI {

enum class Screen {
    HOME,
    VIEW,
    ADD,
    EDIT,
    SETTINGS
};
enum class Category {
    NONE,
    GUNS,
    EVENTS,
    STOCKPILE,

    SIZE
};
enum class SubItem {
    NONE,

    EVENT_EVENT,
    EVENT_TYPE,
    EVENT_LOCATION,

    AMMO_AMMO,
    AMMO_MANUFACTURER,

    GUN_AMMO_CARTRIDGE,

    GUN_GUN,
    GUN_WEAPON_TYPE,

    SIZE
};

// To iterate over categories
constexpr std::array<Category, static_cast<size_t>(Category::SIZE)> CATEGORY_LIST {
    Category::NONE,
    Category::GUNS,
    Category::EVENTS,
    Category::STOCKPILE
};
constexpr std::array<SubItem, static_cast<size_t>(SubItem::SIZE)> SUB_ITEM_LIST {
    SubItem::NONE,

    SubItem::EVENT_EVENT,
    SubItem::EVENT_TYPE,
    SubItem::EVENT_LOCATION,

    SubItem::AMMO_AMMO,
    SubItem::AMMO_MANUFACTURER,

    SubItem::GUN_AMMO_CARTRIDGE,

    SubItem::GUN_GUN,
    SubItem::GUN_WEAPON_TYPE,
};

const Cartridge      EMPTY_CARTRIDGE     { };
const WeaponType     EMPTY_WEAPON_TYPE   { };
const GunMetadata    EMPTY_GUN_METADATA = {
    .cartridge      = EMPTY_CARTRIDGE,
    .weaponType     = EMPTY_WEAPON_TYPE
};
const GunAndAmmo     EMPTY_GUN_AND_AMMO { EMPTY_GUN_METADATA };




// Holds data for what is shown that remains between frames
namespace ScreenData{
    struct Home{
        bool showGuns       { true };
        bool showEvents     { true };
        bool showStockpile  { true };

        std::weak_ptr<ShootingEvent>    selectedEvent;
        std::weak_ptr<AssociatedGun>    selectedGun;
        Cartridge                       selectedCartridge { };
    };

    struct View {
        struct EventTab {
            std::weak_ptr<ShootingEvent>                selectedEvent;
            std::reference_wrapper<const GunAndAmmo>    selectedGun     { std::cref(EMPTY_GUN_AND_AMMO) };
        };

        struct GunTab{
            std::weak_ptr<AssociatedGun>    selectedGun;
            std::weak_ptr<ShootingEvent>    selectedEvent;
        };

        struct StockpileTab{
            Cartridge                       selectedCartridge { };
            std::weak_ptr<AssociatedAmmo>   selectedAmmo;
        };


        Category        category                { Category::NONE }; 
        std::string     categoryComboBoxText    { };

        EventTab        eventTab                { };
        GunTab          gunTab                  { };
        StockpileTab    stockpileTab            { };
    };

    struct Add {
        static constexpr int MAX_CHAR_INPUT { 64 };
        Category category   { Category::NONE }; 
        SubItem  subItem    { SubItem::NONE };
    };

    struct Edit {
        Category category { Category::NONE }; 

        std::weak_ptr<ShootingEvent>    selectedEvent;
        std::weak_ptr<AssociatedAmmo>   selectedAmmo;
        std::weak_ptr<AssociatedGun>    selectedGun;
    };
 
}   // End ScreenData namespace

}   // End UI namespace
