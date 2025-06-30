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


EventMetadata::EventMetadata() 
    :   location {Location { }},  
        eventType{EventType{ }},  
        notes{""}, 
        date {std::chrono::sys_days{std::chrono::year_month_day{std::chrono::year{0}, std::chrono::month{0}, std::chrono::day{0}}}}
{

}
EventMetadata::EventMetadata(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate)
    :   location    { setLocation },
        eventType   { setEventType },
        notes       { setNotes },
        date        { setDate }
{

}
EventMetadata::~EventMetadata(){

}
std::string EventMetadata::getNotes() const{
    return notes;
}
EventType EventMetadata::getEventType() const{
    return eventType;
}
Location EventMetadata::getLocation() const{
    return location;
}
const ymd& EventMetadata::getDate() const{
    return date;
}
timepoint EventMetadata::getTimepoint()  const{
    return std::chrono::system_clock::time_point{std::chrono::sys_days{date}};
}
