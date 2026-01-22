#include "Framework.h"

#include <Config.h>
#include <LAS/ModuleAPI.h>

#include <functional>

static ShooterCentral::Framework application {};

bool LASM_loadModuleInfo(LAS::Information::ModuleInfo& moduleInfo){
    using namespace LAS;
    using namespace ShooterCentral;

    Information::Version sdkVersion { SDK::getVersionMajor(), SDK::getVersionMinor(), SDK::getVersionPatch()};

    moduleInfo.sdkVersion       = sdkVersion;
    moduleInfo.moduleVersion    = Information::Version{SC_VERSION_MAJOR, SC_VERSION_MINOR, SC_VERSION_PATCH};

    moduleInfo.title            = Framework::TITLE;
    moduleInfo.shortTag         = Framework::SHORT_TAG;

    moduleInfo.updateFunction   = std::function<void()>{std::bind(&Framework::update, &application)};
    moduleInfo.drawFunction     = std::function<void()>{std::bind(&Framework::draw, &application)};
    // Add commands if needed
    
    return true;
}
bool LASM_init(const LAS::Information::EnvironmentInfo& envInfo){
    return application.setup(envInfo.directory);
}
void LASM_cleanup(){
    LAS::log_info("Cleanup from Shooter Central lasm_cleanup()");
}

