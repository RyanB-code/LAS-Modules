#include "Event.h"

using namespace ShooterCentral;


Location::Location(std::string setName) : name { setName } {

}
Location::~Location(){

}
std::string Location::getName() const {
    return name;
}
Location::operator std::string() const {
    return name;
}
bool Location::operator==(const Location& other) const{
    if(this->getName() == other.getName())
        return true;
    else
        return false;
}
bool Location::operator<(const Location& other) const{
    return name < other.getName();
}



ShootingEventType::ShootingEventType(std::string setName) : name { setName } {

}
ShootingEventType::~ShootingEventType(){

}
std::string ShootingEventType::getName() const {
    return name;
}
ShootingEventType::operator std::string() const {
    return name;
}
bool ShootingEventType::operator==(const ShootingEventType& other) const{
    if(this->getName() == other.getName())
        return true;
    else
        return false;
}
bool ShootingEventType::operator<(const ShootingEventType& other) const{
    return name < other.getName();
}



bool ShootingEventMetadata::operator==(const ShootingEventMetadata& other) const{
    if(date != other.date)
        return false;

    if(eventType != other.eventType)
        return false;

    if(location != other.location)
        return false;

    return true;
}
bool ShootingEventMetadata::operator<(const ShootingEventMetadata& other) const{
    return std::tuple(other.date, other.eventType, other.location) < std::tuple(date, eventType, location);
}
void ShooterCentral::to_json(LAS::json& j, const ShootingEventMetadata& data){
    std::ostringstream timeString;
    timeString << std::chrono::system_clock::time_point{std::chrono::sys_days{data.date}};

    j = LAS::json{
        { "location",       data.location    },
        { "eventType",      data.eventType   },
        { "notes",          data.notes       },
        { "date",           timeString.str() }
    };
}
std::chrono::system_clock::time_point ShooterCentral::stringToTimepoint(const std::string& timeString) {
    std::stringstream ss{ timeString };
    std::tm time{};
    time.tm_isdst = -1;
    ss >> std::get_time(&time, "%Y-%m-%d%n%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&time));
}

ShootingEvent::ShootingEvent(const ShootingEventMetadata& info) : eventMetadata {info} {
    gunsUsedList.reserve(5);
}
ShootingEvent::~ShootingEvent(){

}
bool ShootingEvent::addGun(const GunAndAmmo& gun) {
    for(auto& entry : gunsUsedList){
        if(gun.getGunInfo() == entry.getGunInfo())
            return false;
    }

    gunsUsedList.emplace_back(gun);
    return true;
}
bool ShootingEvent::hasUsedGun(const GunMetadata& gun) const {
    for(const auto& entry : gunsUsedList){
        if(entry.getGunInfo() == gun)
            return true;
    }
    
    return false;
}
int ShootingEvent::totalGunsUsed() const {
    return gunsUsedList.size();
}
const ShootingEventMetadata& ShootingEvent::getInfo() const {
    return eventMetadata;
}
std::string ShootingEvent::printDate() const{
    return std::format("{:%Od %b %Y}", eventMetadata.date);
}
const std::vector<GunAndAmmo>& ShootingEvent::getGunsUsed() const{
    return gunsUsedList;
}
