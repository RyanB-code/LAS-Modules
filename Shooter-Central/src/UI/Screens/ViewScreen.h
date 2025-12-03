#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIData.h"
#include "UI/UIEvents.h" 
#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing


namespace ShooterCentral::UI::View {

void main(const Database&, ScreenData::View& );

namespace ArmoryWindow {
    void main(
            const std::map<Cartridge, std::map<GunMetadata, ArmoryGun>>& armory,
            ScreenData::View::ArmoryWindow& screenData 
        );
    void selectedGunInformation (const ArmoryGun& );
}

namespace EventsWindow {
    void main(
            const std::map<ShootingEventMetadata, ShootingEvent>&, 
            ScreenData::View::EventsWindow& data
        );
    void selectedEventInformation (const ShootingEvent& event);
}

namespace StockpileWindow {
    void main(
            const std::map<Cartridge, std::map<AmmoMetadata,  StockpileAmmo>>& ammoList,
            const std::map<Cartridge, int>& cartridgeList,
            ScreenData::View::StockpileWindow& tabData
        );
    void selectedAmmoInformation   (const StockpileAmmo& );
}


}   // End SC::UI::View namespace
