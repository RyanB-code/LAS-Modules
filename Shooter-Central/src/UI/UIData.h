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
static const AmountOfAmmo           EMPTY_AMOUNT_OF_AMMO    { };

}   // End UI namespace
