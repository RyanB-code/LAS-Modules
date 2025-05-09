#include <LAS/ModuleAPI.h>
#include <LAS/Window.h>

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
    return true;
}
void LASM_cleanup(){
    
}
