#include "Event.h"

using namespace ShooterCentral;

// MARK: EVENT TYPE
EventType::EventType(std::string setName) : name{setName} {

}
EventType::~EventType(){

}
std::string EventType::getName() const {
    return name;
}
EventType::operator std::string() const {
    return name;
}
bool EventType::operator==(const EventType& other) const {
    if(this->name == other.getName())
        return true;

    return false;
}
std::ostream& operator<<(std::ostream& stream, const EventType& event){
    stream << event.getName();
    return stream;
}

// MARK: EVENT
Event::Event(std::string setName, std::string setLocation, EventType setEventType, std::string setNotes, Datestamp setDate)
    :   name        { setName },
        location    { setLocation },
        eventType   { setEventType },
        notes       { setNotes },
        date        { setDate }
{

}
Event::~Event(){

}
std::string Event::getName() const{
    return name;
}
std::string Event::getNotes() const{
    return notes;
}
EventType Event::getEventType() const{
    return eventType;
}
std::string Event::getLocation() const{
    return location;
}
const Datestamp& Event::getDatestamp() const{
    return date;
}
bool Event::addGun(GunPtr gun, TrackedAmmo ammoUsed){
    if(!gun)
        return false;

    uint8_t gunsInList { 0 };
    for(const auto& pair : gunsUsed){
        if(pair.first != nullptr)
            ++gunsInList;
    }

    if(gunsInList >= gunsUsed.max_size())
        return false;

    // Add gun
    gunsUsed[gunsInList-1] = std::pair(gun, ammoUsed);
    
    // Add to round counter for gun
    if(!gun->addToRoundCount(ammoUsed.amount, ammoUsed.ammoType))
        return false;
    
    return true;
}
bool Event::operator==(const Event& other) const{
    if(this->name == other.getName() && this->location == other.getLocation() && this->date.printDate() == other.getDatestamp().printDate())
        return true;

    return false;
}


// MARK: EVENT TRACKER
EventTracker::EventTracker(LAS::Logging::LoggerPtr setLogger) : logger { setLogger }
{

}
EventTracker::~EventTracker()
{

}
bool EventTracker::setDirectory(std::string path) {
    path = LAS::TextManip::ensureSlash(path);

    if(std::filesystem::exists(path)){
        saveDirectory = path;
        return true;
    }
    else
        return false;
}
std::string EventTracker::getDirectory() const{
    return saveDirectory;
}
bool EventTracker::addEvent (Event& event){
   if(events.contains(event))
        return false;

    return events.try_emplace(event, std::make_shared<Event>(event)).second;
}

void EventTracker::getAllEvents(std::vector<Event>& list) const {
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, event] : events)
        list.push_back(*event);
}