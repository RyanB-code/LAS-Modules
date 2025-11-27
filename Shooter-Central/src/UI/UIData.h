#pragma once

#include "Backend/Database.h"

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

static const GunMetadata            EMPTY_GUN_METADATA      { };
static const AmmoMetadata           EMPTY_AMMO_METADATA     { };
static const ShootingEventMetadata  EMPTY_EVENT_METADATA    { };
static const Cartridge              EMPTY_CARTRIDGE         { };    


// Holds data for what is shown that remains between frames
namespace ScreenData{
    struct Home{
        bool showGuns       { true };
        bool showEvents     { true };
        bool showStockpile  { true };

        GunMetadata             selectedGun         { };
        ShootingEventMetadata   selectedEvent       { };
        Cartridge               selectedCartridge   { };
    };


    struct View {
        /*
        struct EventTab {
            std::weak_ptr<ShootingEvent>                selectedEvent;
            std::reference_wrapper<const GunAndAmmo>    selectedGun     { std::cref(EMPTY_GUN_AND_AMMO) };
        };

        struct GunTab{
            std::weak_ptr<AssociatedGun>    selectedGun;
            std::weak_ptr<ShootingEvent>    selectedEvent;
        };
        */

        struct StockpileTab{
            Cartridge                       selectedCartridge { };
            //std::weak_ptr<AssociatedAmmo>   selectedAmmo;
        };


        Category        category                { Category::NONE }; 
        std::string     categoryComboBoxText    { };

        //EventTab        eventTab                { };
        //GunTab          gunTab                  { };
        StockpileTab    stockpileTab            { };
    };

    struct Add {
        /*
        static constexpr int MAX_CHAR_INPUT { 64 };

        struct SubItemBuffers {
            char weaponType[MAX_CHAR_INPUT]     = "";
            char manufacturer[MAX_CHAR_INPUT]   = "";
            char cartridge[MAX_CHAR_INPUT]      = "";
            char location[MAX_CHAR_INPUT]       = "";
            char eventType[MAX_CHAR_INPUT]      = "";
        };
        enum class EventTab {
            INFO,
            GUNS_AND_AMMO,
            REVIEW_AND_SUBMIT
        };
        struct EventBuffer {
            Location            location        { "" };
            ShootingEventType   eventType       { "" };

            std::vector<std::reference_wrapper<const GunAndAmmo>> gunsUsed { };

            char notes[MAX_CHAR_INPUT] = "";
            int day     { 0 };
            int month   { 0 };
            int year    { 0 };

            bool applyToStockpile   { true };
            bool applyToArmory      { true };

            bool eventInfoVerified  { false };
            bool gunsVerified       { false };
            EventTab currentTab { EventTab::INFO };
        };

        Category        category        { Category::NONE }; 
        SubItem         subItem         { SubItem::NONE };
        SubItemBuffers  subItemBuffers  { };
        EventBuffer     eventBuffer     { };
        */

    };

    struct Edit {
        /*
        Category category { Category::NONE }; 

        std::weak_ptr<ShootingEvent>    selectedEvent;
        std::weak_ptr<AssociatedAmmo>   selectedAmmo;
        std::weak_ptr<AssociatedGun>    selectedGun;
        */
    };
 
}   // End ScreenData namespace

}   // End UI namespace
