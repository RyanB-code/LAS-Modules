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



EventType::EventType(std::string setName) : name { setName } {

}
EventType::~EventType(){

}
std::string EventType::getName() const {
    return name;
}
EventType::operator std::string() const {
    return name;
}
bool EventType::operator==(const EventType& other) const{
    if(this->getName() == other.getName())
        return true;
    else
        return false;
}
bool EventType::operator<(const EventType& other) const{
    return name < other.getName();
}



bool EventMetadata::operator==(const EventMetadata& other) const{
    if(date != other.date)
        return false;

    if(eventType != other.eventType)
        return false;

    if(location != other.location)
        return false;

    return true;
}
bool EventMetadata::operator<(const EventMetadata& other) const{
    return std::tuple(other.date, other.eventType, other.location) < std::tuple(date, eventType, location);
}
void ShooterCentral::to_json(LAS::json& j, const EventMetadata& data){
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

Event::Event(const EventMetadata& info) : eventMetadata {info} {
    gunsUsedList.reserve(5);
}
Event::~Event(){

}
bool Event::addGun(const GunAndAmmo& gun) {
    for(auto& entry : gunsUsedList){
        if(gun.getGunInfo() == entry.getGunInfo())
            return false;
    }

    gunsUsedList.emplace_back(gun);
    return true;
}
bool Event::hasUsedGun(const GunMetadata& gun) const {
    for(const auto& entry : gunsUsedList){
        if(entry.getGunInfo() == gun)
            return true;
    }
    
    return false;
}
int Event::totalGunsUsed() const {
    return gunsUsedList.size();
}
const EventMetadata& Event::getInfo() const {
    return eventMetadata;
}
std::string Event::printDate() const{
    return std::format("{:%Od %b %Y}", eventMetadata.date);
}
const std::vector<GunAndAmmo>& Event::getGunsUsed() const{
    return gunsUsedList;
}
