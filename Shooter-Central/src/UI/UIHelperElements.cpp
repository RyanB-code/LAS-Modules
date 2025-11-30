#include "UI/UIHelperElements.h"

namespace ShooterCentral::UI {

void centerNextItemX(float x){
    float windowWidth { ImGui::GetContentRegionAvail().x };
    if(x < windowWidth)
        ImGui::SetCursorPosX((windowWidth - x) * 0.5f);

    return;
}
void centerNextItemY(float y){
    float windowHeight { ImGui::GetContentRegionAvail().y };
    if(y < windowHeight)
        ImGui::SetCursorPosY((windowHeight - y) * 0.5f);

    return;

}
void centerNextComboBoxX(const std::string& text, float comboBoxWidth){
    centerNextItemX(ImGui::CalcTextSize(text.c_str()).x + comboBoxWidth );
    ImGui::Text("%s", text.c_str()); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboBoxWidth);
}

void centerText(const std::string& text){
    float windowWidth { ImGui::GetWindowSize().x };
    float textWidth   { ImGui::CalcTextSize(text.c_str()).x };

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text.c_str());
}
void centerTextDisabled(const std::string& text){
    float windowWidth { ImGui::GetWindowSize().x };
    float textWidth   { ImGui::CalcTextSize(text.c_str()).x };

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextDisabled("%s", text.c_str());
}
bool centerButton(const std::string& text, ImVec2 size){
    float windowWidth = ImGui::GetWindowSize().x;

    ImGui::SetCursorPosX((windowWidth - size.x) * 0.5f);
    return ImGui::Button(text.c_str(), size);
}
std::string categoryToString(const Category& category, const std::string& noneStr){
    switch(category){
        case Category::NONE:
            if(noneStr.empty())
                return std::string {"None"};
            else
                return noneStr;
            break;
        case Category::GUNS:
            return std::string {"Guns"};
            break;
        case Category::EVENTS:
            return std::string {"Events"};
            break;
        case Category::STOCKPILE:
            return std::string {"Stockpile"};
            break;
        default:
            return std::string {"NOT HANDLED"};
            break;
    }
}
std::string subItemToString (const SubItem& item, const std::string& noneStr){
    switch(item){
        case SubItem::NONE:
            if(noneStr.empty())
                return std::string {"None"};
            else
                return noneStr;
            break;
        case SubItem::EVENT_EVENT:
            return std::string {"Event"};
            break;
        case SubItem::EVENT_TYPE:
            return std::string {"Event Type"};
            break;
        case SubItem::EVENT_LOCATION:
            return std::string {"Event Location"};
            break;
        case SubItem::AMMO_AMMO:
            return std::string {"Ammo"};
            break;
        case SubItem::AMMO_MANUFACTURER:
            return std::string {"Manufacuter"};
            break;
        case SubItem::GUN_AMMO_CARTRIDGE:
            return std::string {"Cartridge"};
            break;
        case SubItem::GUN_GUN:
            return  std::string {"Gun"};
            break;
        case SubItem::GUN_WEAPON_TYPE:
            return std::string {"Weapon Type"};
            break;
        default:
            return std::string {"NOT HANDLED"}; 
            break;
    }
}

void resetText (char* dest, size_t size, const char* replacementText) {
    if(!dest){
        LAS::log_warn("UI::resetText() - destination is null");
        return;
    }
    
    if(strlen(dest) > size) {
        LAS::log_warn("UI::resetText() - buffer > allowed size");
        return;
    }

    if(strlen(replacementText) > size) {
        LAS::log_warn("UI::resetText() - replacement text > allowed size");
        return;
    }

    memset(dest, '\0', size);
    std::strcpy(dest, replacementText);

    return;
}




void Tables::Selectable::ammoMetadata(
        const std::map<AmmoMetadata, StockpileAmmo>& list, 
        AmmoMetadata& selected,
        ImVec2 size 
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Ammo Table", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);

        ImGui::TableHeadersRow();

        for(const auto& [key, stockpileAmmo] : list){
            const AmmoMetadata& ammoInfo { stockpileAmmo.getAmmoInfo() };
            bool isSelected { selected == ammoInfo };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 3; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(ammoInfo.manufacturer.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = ammoInfo;

                        break;
                    case 1:
                        ImGui::Text("%s", ammoInfo.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%d", ammoInfo.grainWeight);
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }        
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::ammoMetadata(
    const std::map<Cartridge, std::map<AmmoMetadata, StockpileAmmo>>& list,       
    AmmoMetadata& selected,
    ImVec2 size
){
    int row { 0 };
    if(ImGui::BeginTable("Ammo Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Cartridge", 0);
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);

        ImGui::TableHeadersRow();

        for(const auto& [cartridge, map] : list){
            for(const auto& [key, stockpileAmmo] : map){
                const AmmoMetadata& ammoInfo { stockpileAmmo.getAmmoInfo() };
                bool isSelected { selected == ammoInfo };

                ImGui::PushID(std::to_string(row).c_str());
                ImGui::TableNextRow();

                for (int column{0}; column < 4; ++column) {
                    ImGui::TableSetColumnIndex(column);
                    switch( column ){
                        case 0:
                            if(ImGui::Selectable(ammoInfo.cartridge.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns))
                                selected = ammoInfo;
                            break;
                        case 1:
                            ImGui::Text("%s", ammoInfo.manufacturer.getName());
                            break;
                        case 2:
                            ImGui::Text("%s", ammoInfo.name.c_str());
                            break;
                        case 3:
                            ImGui::Text("%d", ammoInfo.grainWeight);
                            break;
                        default:
                            ImGui::Text("Broken table");
                            break;
                    }
                }        
                ImGui::PopID();
                ++row;
            }
        }
        ImGui::EndTable();
    }

}
void Tables::Selectable::ammoAmountOnHand(
        const std::map<AmmoMetadata, StockpileAmmo>& list, 
        AmmoMetadata& selected,
        ImVec2 size 
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Ammo Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableSetupColumn("Amount On Hand",  0);

        ImGui::TableHeadersRow();

        for(const auto& [key, stockpileAmmo] : list){
            const AmmoMetadata& ammoInfo { stockpileAmmo.getAmmoInfo() };
            bool isSelected { selected == ammoInfo };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(ammoInfo.manufacturer.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = ammoInfo;

                        break;
                    case 1:
                        ImGui::Text("%s", ammoInfo.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%d", ammoInfo.grainWeight);
                        break;
                    case 3:
                        ImGui::Text("%d", stockpileAmmo.getAmountOnHand());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }        
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::cartridgeAmountOnHand(
        const std::map<Cartridge, int>& cartridges, 
        Cartridge& selected, 
        ImVec2 size
    )
{
    int row { 0 };

    if(ImGui::BeginTable("Cartridge Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Cartridge",    0);
        ImGui::TableSetupColumn("Amount",       0);
        ImGui::TableHeadersRow();

        for(const auto& [cartridge, amount] : cartridges){
            bool isSelected { selected == cartridge };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 2; ++column){        
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(cartridge.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = cartridge;
                        break;
                    case 1:
                        ImGui::Text("%d", amount);
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::gunMetadata(   
            const std::set<GunMetadata>& guns, 
            GunMetadata& selected,
            ImVec2 size
        )
{
    int row { 0 };
    if(ImGui::BeginTable("GunMetadata Table", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )){
        ImGui::TableSetupColumn("WeaponType",   ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& info : guns){
            bool isSelected    { selected == info };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 3; ++column){                   
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(info.weaponType.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = info;
                        break;
                    case 1:
                        ImGui::Text("%s", info.cartridge.getName());
                        break;
                    case 2:
                        ImGui::Text("%s", info.name.c_str());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::gunMetadata(
        std::vector<GunTrackingAmmoUsed>& guns,
        std::vector<GunTrackingAmmoUsed>::iterator& selected,
        ImVec2 size
    )
{
    int row { 0 };
    if(ImGui::BeginTable("GunMetadata Table", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )){
        ImGui::TableSetupColumn("WeaponType",   ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(auto itr { guns.begin() }; itr != guns.end(); ++itr){
            bool isSelected { selected == itr };
            const GunMetadata& info { itr->getGunInfo() };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 3; ++column){                   
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(info.weaponType.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = itr;
                        break;
                    case 1:
                        ImGui::Text("%s", info.cartridge.getName());
                        break;
                    case 2:
                        ImGui::Text("%s", info.name.c_str());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::gunMetadataWithRoundCount(
        const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& list, 
        GunMetadata& selected, 
        ImVec2 size
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Guns Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size)) {
        ImGui::TableSetupColumn("Weapon Type",          ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",            ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",                 ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Lifetime Round Count", ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& [cartridge, map] : list){
            // Do not sort by Cartridge so just ignore it
            for(const auto& [gunMetadata, armoryGun] : map){
                const GunMetadata& gun {armoryGun.getGunInfo()};
                bool isGunSelected { selected == gun };
            
                ImGui::PushID(std::to_string(row).c_str());
                ImGui::TableNextRow();

                for (int column{0}; column < 4; ++column){
                    ImGui::TableSetColumnIndex(column);
                    switch( column ){
                        case 0:
                            if(ImGui::Selectable(gun.weaponType.getName(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns))
                                selected = gun;
                            break;
                        case 1:
                            ImGui::Text("%s", gun.cartridge.getName());
                            break;
                        case 2:
                            ImGui::Text("%s", gun.name.c_str());
                            break;
                        case 3:
                            ImGui::Text("%d", armoryGun.getRoundCount());
                            break;
                        default:
                            ImGui::TextDisabled("Broken table");
                            break;
                    }
                }
                ImGui::PopID();
                ++row;       
            }
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::gunMetadataWithRoundCount(
        const std::vector<GunTrackingAmmoUsed>& list, 
        GunMetadata& selected, 
        ImVec2 size 
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Guns Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size)) {
        ImGui::TableSetupColumn("Weapon Type",  ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Cartridge",    ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Round Count",  ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& gunAmmoUsed : list){
            const GunMetadata& gunInfo {gunAmmoUsed.getGunInfo()};
            bool isGunSelected { selected == gunInfo };
        
            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column){
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(gunInfo.weaponType.getName(), &isGunSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = gunInfo;
                        break;
                    case 1:
                        ImGui::Text("%s", gunInfo.cartridge.getName());
                        break;
                    case 2:
                        ImGui::Text("%s", gunInfo.name.c_str());
                        break;
                    case 3:
                        ImGui::Text("%d", gunAmmoUsed.totalRoundsShot());
                        break;
                    default:
                        ImGui::TextDisabled("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;       
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::eventMetadata( 
        const std::set<ShootingEventMetadata>& events, 
        ShootingEventMetadata& selected,
        ImVec2 size
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Event Table", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )){
        ImGui::TableSetupColumn("Date",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Event Type",   ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Location",     ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& eventInfo : events){
            bool isEventSelected    { selected == eventInfo };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 3; ++column){                   
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(printDate(eventInfo.date).c_str(), &isEventSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = eventInfo;
                        break;
                    case 1:
                        ImGui::Text("%s", eventInfo.eventType.getName());
                        break;
                    case 2:
                        ImGui::Text("%s", eventInfo.location.getName());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::eventsWithNumGunsUsed( 
        const std::map<ShootingEventMetadata, ShootingEvent>& events, 
        ShootingEventMetadata& selected,
        ImVec2 size
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Event Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )){
        ImGui::TableSetupColumn("Date",         ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Event Type",   ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Location",     ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Guns Used",    ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        for(const auto& [key, event] : events){
            const ShootingEventMetadata& eventInfo  { event.getInfo() };
            bool isEventSelected    { selected == eventInfo };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column){                   
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(printDate(eventInfo.date).c_str(), &isEventSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = eventInfo;
                        break;
                    case 1:
                        ImGui::Text("%s", eventInfo.eventType.getName());
                        break;
                    case 2:
                        ImGui::Text("%s", eventInfo.location.getName());
                        break;
                    case 3:
                        ImGui::Text("%d", event.totalGunsUsed());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::Selectable::amountOfAmmo(  
        const std::map<AmmoMetadata, AmountOfAmmo>& ammoUsed, 
        AmmoMetadata& selected,
        ImVec2 size
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Detailed Cartridge Breakdown", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableSetupColumn("Amount Used",  0);

        ImGui::TableHeadersRow();

        for(const auto& [info, amountOfAmmo] : ammoUsed){
            bool isItemSelected { false };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        if(ImGui::Selectable(info.manufacturer.getName(), &isItemSelected, ImGuiSelectableFlags_SpanAllColumns))
                            selected = info;
                        break;
                    case 1:
                        ImGui::Text("%s", info.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%d", info.grainWeight);
                        break;
                    case 3:
                        ImGui::Text("%d", amountOfAmmo.getAmount());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }        
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }
}
void Tables::amountOfAmmo(
        const std::vector<AmountOfAmmo>& ammoUsed, 
        ImVec2 size
    )
{
    int row { 0 };
    if(ImGui::BeginTable("Ammo Used Table", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn, size )) {
        ImGui::TableSetupColumn("Manufacturer", 0);
        ImGui::TableSetupColumn("Name",         0);
        ImGui::TableSetupColumn("Grain Weight", 0);
        ImGui::TableSetupColumn("Amount Used",  0);

        ImGui::TableHeadersRow();

        for(const auto& amountOfAmmo : ammoUsed){
            const AmmoMetadata& info { amountOfAmmo.getAmmoInfo() };

            ImGui::PushID(std::to_string(row).c_str());
            ImGui::TableNextRow();

            for (int column{0}; column < 4; ++column) {
                ImGui::TableSetColumnIndex(column);
                switch( column ){
                    case 0:
                        ImGui::Text("%s", info.manufacturer.getName());
                        break;
                    case 1:
                        ImGui::Text("%s", info.name.c_str());
                        break;
                    case 2:
                        ImGui::Text("%d", info.grainWeight);
                        break;
                    case 3:
                        ImGui::Text("%d", amountOfAmmo.getAmount());
                        break;
                    default:
                        ImGui::Text("Broken table");
                        break;
                }
            }        
            ImGui::PopID();
            ++row;
        }
        ImGui::EndTable();
    }

}




void ListBoxes::cartridges(const std::set<Cartridge>& list, ImVec2 size){
    if(ImGui::BeginListBox("##Cartridge List Box", size)){
        for(const auto& cartridge : list) {
            bool isSelected { false };
            ImGui::Selectable(cartridge.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::manufacturers(const std::set<Manufacturer>& list, ImVec2 size){
    if(ImGui::BeginListBox("##Manufacturer List Box", size)){
        for(const auto& manufacturer : list) {
            bool isSelected { false };
            ImGui::Selectable(manufacturer.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::eventLocations (const std::set<Location>& list,         ImVec2 size){
    if(ImGui::BeginListBox("##Event Location List Box", size)){
        for(const auto& location : list) {
            bool isSelected { false };
            ImGui::Selectable(location.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::eventTypes(const std::set<ShootingEventType>& list,       ImVec2 size){
    if(ImGui::BeginListBox("##Event Type List Box", size)){
        for(const auto& et : list) {
            bool isSelected { false };
            ImGui::Selectable(et.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}
void ListBoxes::weaponTypes(const std::set<WeaponType>& list,     ImVec2 size){
    if(ImGui::BeginListBox("##Weapon Type List Box", size)){
        for(const auto& wt : list) {
            bool isSelected { false };
            ImGui::Selectable(wt.getName(), &isSelected, ImGuiSelectableFlags_SpanAllColumns);
        }
        ImGui::EndListBox();
    }
}

void ComboBoxes::category(Category& selected) {
    std::string text { categoryToString(selected, "Select A Category") };

    if (ImGui::BeginCombo("##Category Select Combo", text.c_str(), ImGuiComboFlags_HeightLarge)) {
        for (const Category& category : CATEGORY_LIST) {
            const bool isSelected = (selected == category);

            if(ImGui::Selectable(categoryToString(category, "Select A Category").c_str(), isSelected)){
                selected = category;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
void ComboBoxes::subItem(SubItem& selected){
    std::string text { subItemToString(selected, "Select A Category") };

    if (ImGui::BeginCombo("##Sub Item Select Combo", text.c_str(), ImGuiComboFlags_HeightLarge)) {
        for (const SubItem& item : SUB_ITEM_LIST) {
            const bool isSelected = (selected == item);

            if(ImGui::Selectable(subItemToString(item, "Select A Category").c_str(), isSelected)){
                selected = item;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
void ComboBoxes::locations  (const std::set<Location>& list, Location& selected){
    if (ImGui::BeginCombo("##Location Combo", selected.getName(), ImGuiComboFlags_HeightLarge)) {
        for (const auto& loc : list) {
            bool isSelected { selected == loc };

            if (ImGui::Selectable(loc.getName(), isSelected)){
                selected = loc;
                ImGui::SetItemDefaultFocus();
            } 
        }
        ImGui::EndCombo();
    }
}
void ComboBoxes::eventTypes (const std::set<ShootingEventType>& list, ShootingEventType& selected){
    if (ImGui::BeginCombo("##Event Type Combo", selected.getName(), ImGuiComboFlags_HeightLarge)) {
        for (const auto& et : list) {
            bool isSelected {selected == et};

            if (ImGui::Selectable(et.getName(), isSelected)){
                selected = et;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}






Popup::Popup(const char* set) {
    strncpy(title, set, sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';            // Manually null-terminate
}
Popup::~Popup()  {

}
void Popup::close() const {
    ImGui::CloseCurrentPopup();

    //UIEvents::ClosePopup close{ };
    //pushEvent(&close);
}
const char* Popup::getTitle() const {
    return title;
}
void Popup::buttons_Close() const {
    if (centerButton("Close", ImVec2{120, 0}))
        close();
}
bool Popup::buttons_YesOrNo() const{ 
    float windowWidth { ImGui::GetWindowSize().x };
    float buttonWidth { 100 };

    ImGui::SetCursorPosX( (windowWidth * 0.5f) - (buttonWidth) - 3 ); // Subtracting 3 makes it seem more centered

    if(ImGui::Button("Yes", ImVec2 {buttonWidth, 0}))
        return true;

    ImGui::SameLine();

    if(ImGui::Button("No", ImVec2 {buttonWidth, 0})) 
       close(); 

    return false;
}

SimplePopup_Close::SimplePopup_Close(const char* setTitle, const char* setText) : Popup(setTitle) {
    strncpy(text, setText, sizeof(text) - 1);
    text[sizeof(text) - 1] = '\0';            // Manually null-terminate
}
void SimplePopup_Close::show() {
    UI::centerText(text);

    buttons_Close();
}


}   // End UI namespace
