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
    // This orders with recent events first
    return std::tuple(other.date, std::string{other.eventType}, std::string{other.location}) < std::tuple(date, std::string{eventType}, std::string{location});
}
void ShooterCentral::to_json(LAS::json& j, const EventMetadata& data){
    std::ostringstream timeString;
    timeString << std::chrono::system_clock::time_point{std::chrono::sys_days{data.date}};

    j = LAS::json{
        { "location",       data.location   },
        { "eventType",      data.eventType  },
        { "notes",          data.notes      },
        { "date",           timeString.str()}
    };
}
void ShooterCentral::from_json(const LAS::json& j, EventMetadata& data){
    std::string  dateBuf{ }, locationBuf { }, eventTypeBuf { }, notesBuf;

    j.at("date").get_to(dateBuf);
    j.at("eventType").get_to(eventTypeBuf);
    j.at("location").get_to(locationBuf);
    j.at("notes").get_to(notesBuf);

    // Time conversion
    ymd ymdBuf { std::chrono::floor<std::chrono::days>(stringToTimepoint(dateBuf)) };

    data.location   = Location   {locationBuf};
    data.eventType  = EventType {eventTypeBuf};
    data.notes      = notesBuf;
    data.date       = ymdBuf;
}
std::chrono::system_clock::time_point ShooterCentral::stringToTimepoint(const std::string& timeString) {
    std::stringstream ss{ timeString };
    std::tm time{};
    time.tm_isdst = -1;
    ss >> std::get_time(&time, "%Y-%m-%d%n%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&time));
}

Event::Event(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate)
    :   eventMetadata {setLocation, setEventType, setNotes, setDate} {

}
Event::Event(const EventMetadata& info) : eventMetadata {info} {

}
Event::~Event(){

}
bool Event::addGun(const GunAndAmmo& gun) {
    for(auto& entry : gunsUsedList){
        if(!entry)
            continue;
        if(gun.getGun() == entry.getGun())
            return false;
    }

    // If not already used, add new entry
    if(nextIndex < MAX_NUM_GUNS){
        gunsUsedList[nextIndex] = gun;
        ++nextIndex;
        return true;
    }

    return false;
}
bool Event::hasUsedGun(const GunMetadata& gun) const {
    for(const auto& entry : gunsUsedList){
        if(entry.getGun() == gun)
            return true;
    }
    
    return false;
}
int Event::totalGunsUsed() const {
    return nextIndex;
}
const EventMetadata& Event::getInfo() const {
    return eventMetadata;
}
std::string Event::getNotes() const{
    return eventMetadata.notes;
}
EventType Event::getEventType() const{
    return eventMetadata.eventType;
}
Location Event::getLocation() const{
    return eventMetadata.location;
}
const ymd& Event::getDate() const{
    return eventMetadata.date;
}
std::string Event::printDate() const{
    return std::format("{:%Od %b %Y}", eventMetadata.date);
}
const std::array<GunAndAmmo, Event::MAX_NUM_GUNS>& Event::getGunsUsed() const{
    return gunsUsedList;
}

