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
            ScreenData::Edit::ItemBuffers& buffers,
            const Database& database, 
            const SubItem& selected, 
            const ImVec2& size
        );
    void editItemWindow(
            ScreenData::Edit& data,
            const Database& database,
            SubItem selectedItem
        );

    void editMetadataItem (
            char* oldInfo, 
            char* textBuf, 
            size_t size,
            bool& submitted,
            const ImVec2& buttonSize
        );

}   // End SC::UI::ScreenData

