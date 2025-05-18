#include "Action.h"

#include <LAS/ModuleAPI.h>
#include <LAS/Window.h>

#include <iostream> // FOR TESTING

class GarageWindow : public LAS::Windowing::Window {
public:
    GarageWindow() : LAS::Windowing::Window{"Garage"} {
    
    }
    ~GarageWindow(){

    }

    void draw() override {
        if(!ImGui::Begin("Garage", &shown)){
            ImGui::End();
            return;
        }


        ImGui::End();
    }
};

std::shared_ptr<GarageWindow> window { std::make_shared<GarageWindow>() };

bool LASM_loadModuleInfo(LAS::Information::ModuleInfo& moduleInfo){
    using namespace LAS;
    Information::Version sdkVersion { SDK::getVersionMajor(), SDK::getVersionMinor(), SDK::getVersionPatch()};

    moduleInfo.sdkVersion       = sdkVersion;
    moduleInfo.moduleVersion    = Information::Version{0, 0 , 1};
    moduleInfo.title            = "Garage";
    moduleInfo.commandGroupName = "garage";
    moduleInfo.window           = window;

    // Add commands if needed


    

    return true;
}
bool LASM_init(const LAS::Information::EnvironmentInfo& envInfo){
    using namespace Garage;

    std::cout << "Executing from Garage...\n";
    
    char fuck[1][MAX_CHAR_TAG] = { "12345678901234567890123456789_1" };
    char FUCK[MAX_CHAR_ACTION_NOTES] = { "FUCK" };

    Tags test { fuck, 1 };

    char array[MAX_TAGS][MAX_CHAR_TAG] = { };

    test.getAllTags(array);

    test.addTag("fuck");
    test.addTag("x0");
    test.addTag("4 (3 successful)");
    test.addTag("5");
    test.addTag("6");
    test.getAllTags(array);

    std::cout << "Tags: \n";
    for(int curRow { 0 }; curRow < MAX_TAGS; ++curRow)
        std::cout << '\t' << array[curRow] << "\n";


    YMD ymd;
    ymd.setDateToday();
    Mileage miles { 1600 };

    Action testAction { ymd, miles, Tags{"tag"}, "This is some test notes", Cost { 50, MoneyUnit::DOLLAR} };

    Tags testTags { testAction.getTags() };
    char tagsArray[MAX_TAGS][MAX_CHAR_TAG] = { };
    testTags.getAllTags(tagsArray);
    std::cout << "Action tags: \n";
    for(int curRow { 0 }; curRow < MAX_TAGS; ++curRow)
        if(char{*tagsArray[curRow]} != '\0')
            std::cout << '\t' << tagsArray[curRow] << "\n";


    YMD testYMD { testAction.getYMD() };
    std::cout << "Date: " << testYMD.printAlpha() << "\n";

    Mileage testMiles { testAction.getMileage() };
    std::cout << "Miles: " << testMiles << "\n";

    Cost testCost { testAction.getCost() };
    std::cout << "Cost: " << testCost.printDollar(true) << "\n";

    char testNotes[256] = { };
    testAction.getNotes(testNotes, 256);
    std::cout << "Notes: " << testNotes << "\n";

    ActionReminder testRemind { testAction.getReminder() };
    std::cout << "Remind Date: " << testRemind.getDate().printAlpha() << "\n";
    std::cout << "Remind miles: " << testRemind.getMileage() << "\n";
    return true;
}
void LASM_cleanup(){
    
}
