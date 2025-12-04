#pragma once

#include "Backend/Database.h"
#include <imgui/imgui.h>

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

    EVENT,
    EVENT_TYPE,
    LOCATION,

    AMMO,
    MANUFACTURER,

    CARTRIDGE,

    GUN,
    WEAPON_TYPE,

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
    // Alphabetocal Order
    SubItem::NONE,
        
    SubItem::AMMO,
    SubItem::CARTRIDGE,
    SubItem::EVENT,
    SubItem::EVENT_TYPE,
    SubItem::GUN,
    SubItem::LOCATION,
    SubItem::MANUFACTURER,
    SubItem::WEAPON_TYPE,
};

static const GunMetadata            EMPTY_GUN_METADATA      { };
static const AmmoMetadata           EMPTY_AMMO_METADATA     { };
static const ShootingEventMetadata  EMPTY_EVENT_METADATA    { };
static const AmountOfAmmo           EMPTY_AMOUNT_OF_AMMO    { };
static const Cartridge              EMPTY_CARTRIDGE         { };    
static const Location               EMPTY_LOCATION          { };
static const ShootingEventType      EMPTY_EVENT_TYPE        { };

namespace ScreenData {

struct Home{
    
    struct MainWindow {
        static constexpr ImVec2 MIN_WIN_SIZE  { 500, 500 };

        ImVec2  oneThirdWindowSize  { };        // 3 window panes for the 3 sections.
        ImVec2  childWindowSize     { };
        bool    verticalLayout      { false };
    };
    struct GunWindow {
        static constexpr ImVec2 buttonSize { 100, 40 };

        ImVec2      tableSize        { 0, 300 };
        GunMetadata selectedGun      { };

        bool selectedGunValid { false };
    };
    struct EventsWindow {
        static constexpr ImVec2 buttonSize { 100, 40 };

        ImVec2 tableSize        { 0, 300 };
        ShootingEventMetadata selectedEvent { };

        bool selectedEventValid { false };
    };
    struct StockpileWindow{
        static constexpr ImVec2 buttonSize { 100, 40 };

        ImVec2      tableSize        { 0, 300 };
        Cartridge   selectedCartridge { };

        bool selectedCartridgeValid { false };
    };


    MainWindow      mainWindow      { };
    EventsWindow    eventsWindow    { };
    GunWindow       gunWindow       { };
    StockpileWindow stockpileWindow { };
};

struct View {
    struct ArmoryWindow{
        GunMetadata             selectedGun;

        bool verticalLayout { false };

        static constexpr ImVec2 deselectButtonSize  { 100, 40 };
        static constexpr ImVec2 minWinSize          { 400, 600 };
        static constexpr ImVec2 minTableSize        { 400, 300 };
        static constexpr float  maxTableWidth       { 800 };     

        ImVec2 topWindowSize        { minWinSize };
        ImVec2 bottomWindowSize     { minWinSize };
        ImVec2 topTableSize         { minTableSize.x, 400 };
        ImVec2 bottomTableSize      { minTableSize.x, 400 };
    };

    struct EventsWindow {
        ShootingEventMetadata   selectedEvent   { };
        GunMetadata             selectedGun     { };

        bool verticalLayout { false };

        static constexpr ImVec2 deselectButtonSize  { 100, 40 };
        static constexpr ImVec2 minWinSize          { 400, 600 };
        static constexpr ImVec2 minTableSize        { 400, 300 };
        static constexpr float  maxTableWidth       { 600 };     


        ImVec2 topWindowSize        { minWinSize };
        ImVec2 bottomWindowSize     { minWinSize };
        ImVec2 topTableSize         { minTableSize.x, 400 };
        ImVec2 bottomTableSize      { minTableSize.x, 400 };
    };
    struct StockpileWindow{
        Cartridge       selectedCartridge   { };
        AmmoMetadata    selectedAmmo        { };
        
        bool selectedCartridgeValid { false };
        bool selectedAmmoValid      { false };

        bool verticalLayout { false };

        static constexpr ImVec2 deselectButtonSize  { 100, 40 };
        static constexpr ImVec2 minWinSize          { 400, 600 };
        static constexpr ImVec2 minTableSize        { 400, 300 };
        static constexpr float  maxTableWidth       { 600 };     


        ImVec2 windowSize   { minWinSize };
        ImVec2 tableSize    { minTableSize.x, 400 };
    };

    Category        category                { Category::NONE }; 
    std::string     categoryComboBoxText    { };

    EventsWindow    eventsWindow    { };
    ArmoryWindow    armoryWindow    { };
    StockpileWindow stockpileWindow { };
};


struct Add {
    static constexpr int MAX_CHAR_INPUT { 64 };

    struct SubItemBuffers {
        char weaponType[MAX_CHAR_INPUT]     = "";
        char manufacturer[MAX_CHAR_INPUT]   = "";
        char cartridge[MAX_CHAR_INPUT]      = "";
        char location[MAX_CHAR_INPUT]       = "";
        char eventType[MAX_CHAR_INPUT]      = "";
    }; 

    struct EventWindow {
        struct MetadataWindow {
            ShootingEventType   selectedET          { };
            Location            selectedLocation    { };

            char notes[ShootingEventMetadata::MAX_CHAR_NOTES] = "";
            int day     { 0 };
            int month   { 0 };
            int year    { 0 };

            bool triedToVerifyEventInfo     { false };

            static constexpr ImVec2 buttonSize { 100, 40 };
            static constexpr ImVec2 minWinSize { 200, 100 };

            ImVec2 mainWinSize  { minWinSize };
            ImVec2 infoWinSize  { minWinSize };

        };
        struct GunsAndAmmoWindow {
            struct AddGunWindow {
                GunMetadata     selectedGun     { };

                bool selectedGunValid   { false };

                static constexpr ImVec2 buttonSize { 100, 40 };
            };
            struct AddAmmoWindow {
                AmmoMetadata selectedAmmo { };
                int amountBuffer { 0 };

                bool isAmountOfAmmoValid { false };
                static constexpr ImVec2 buttonSize { 100, 40 };
            };

            static constexpr ImVec2 buttonSize          { 100, 40 };
            static constexpr ImVec2 minWinSize          { 400, 400 };
            static constexpr ImVec2 minTableSize        { 300, 400 };
            static constexpr float  maxTableWidth       { 800 };     

            AddAmmoWindow   addAmmoWindow   { };
            AddGunWindow    addGunWindow    { };

            GunMetadata    selectedGun     { };
            AmmoMetadata   selectedAmmo    { };

            bool verticalLayout     { false };
            bool selectedGunValid   { false };
            bool selectedAmmoValid  { false };
           
            ImVec2 mainWindowSize   { minWinSize };
            ImVec2 viewWindowSize   { minWinSize };
            ImVec2 mainTableSize    { minTableSize };
            ImVec2 viewTableSize    { minTableSize };
        };
        struct ReviewWindow {
            static constexpr ImVec2 buttonSize          { 100, 40 };
            static constexpr ImVec2 minWinSize          { 400, 400 };
            static constexpr ImVec2 minTableSize        { 300, 400 };
            static constexpr float  maxTableWidth       { 800 };     

            GunMetadata selectedGun { };

            bool selectedGunValid   { };
            bool eventMetadataValid { };

            bool verticalLayout { false };

            bool applyToArmory      { true };
            bool applyToStockpile   { true };

            ImVec2 optionsWinSize   { minWinSize.x, 100 };
            ImVec2 reviewWinSize    { minWinSize };
            ImVec2 tableSize        { minTableSize };
        };

        MetadataWindow      metadataWindow      { };
        GunsAndAmmoWindow   gunsAndAmmoWindow   { };
        ReviewWindow        reviewWindow        { };


        ShootingEvent event { };
    };

    static constexpr ImVec2 deselectButtonSize  { 100, 40 };
    static constexpr ImVec2 minWinSize          { 400, 600 };
    static constexpr ImVec2 minTableSize        { 300, 400 };
    static constexpr float  maxTableWidth       { 800 };     

    EventWindow     eventWindow     { };

    Category        category        { Category::NONE }; 
    SubItem         subItem         { SubItem::NONE };
    SubItemBuffers  subItemBuffers  { };

    bool verticalLayout { false };

    ImVec2 mainWindowSize   { minWinSize };
    ImVec2 infoWindowSize   { minWinSize };
    ImVec2 infoTableSize    { minTableSize };
};

struct Edit{
    
};

}   // ScreenData namespace





}   // End UI namespace
