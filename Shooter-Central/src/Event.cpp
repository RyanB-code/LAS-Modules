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
