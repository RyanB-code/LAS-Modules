#pragma once

#include "Backend/DatabaseEvents.h"
#include "Backend/Database.h"

#include "UI/UIData.h"
#include "UI/UIEvents.h" 
#include "UI/UIHelperElements.h"

#include <LAS/Logging.h>
#include <imgui/imgui.h>

#include <iostream> // For testing
                    
                    
namespace ShooterCentral::UI::Edit {

    void main   (const Database&, ScreenData::Edit& );
    void selectExistingItemWindow (
            const Database& database, 
            const SubItem& selected, 
            const ImVec2& size
        );



}   // End SC::UI::ScreenData

