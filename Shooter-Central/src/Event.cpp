#include "Event.h"

using namespace ShooterCentral;

// MARK: Location
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
    if(this->name == other.getName())
        return true;
    else
        return false;
}

std::ostream& operator<<(std::ostream& os, const Location& location){
    os << location.getName();
    return os;
}

// MARK: Event Type
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
    if(this->name == other.getName())
        return true;
    else
        return false;
}

std::ostream& operator<<(std::ostream& os, const EventType& eventType){
    os << eventType.getName();
    return os;
}

// MARK: Event
Event::Event() 
    :   location {"N/A"},  
        eventType{"N/A"},  
        notes{"N/A"}, 
        date {std::chrono::sys_days{std::chrono::year_month_day{std::chrono::year{0}, std::chrono::month{0}, std::chrono::day{0}}}}
{

}
Event::Event(Location setLocation, EventType setEventType, std::string setNotes, ymd setDate)
    :   location    { setLocation },
        eventType   { setEventType },
        notes       { setNotes },
        date        { setDate }
{

}
Event::~Event(){

}
std::string Event::getName() const {
    std::ostringstream nameBuf;
    nameBuf << location.getName() << " " << eventType.getName() << " on " << printDate();
    return nameBuf.str(); 
}
std::string Event::getNotes() const{
    return notes;
}
EventType Event::getEventType() const{
    return eventType;
}
Location Event::getLocation() const{
    return location;
}
const ymd& Event::getDate() const{
    return date;
}
std::string Event::printDate() const{
    return std::format("{:%Od %b %Y}", date);
}
timepoint Event::getTimepoint()  const{
    return std::chrono::system_clock::time_point{std::chrono::sys_days{date}};
}


bool Event::addGun(Gun gun, TrackedAmmo ammoUsed){
    auto gunsInList { 0 };
    for(const auto& [gunElm, ammoElm] : gunsUsed){
        if(gunElm.getName() != "N/A")
            ++gunsInList;
    }

    if(size_t(gunsInList) >= gunsUsed.max_size())
        return false;

    // Add gun
    gunsUsed[gunsInList] = std::pair(gun, ammoUsed);    // Do not need to subtract one since list[i] is 1 greater than the count, which is what I want
    
    // Add to round counter for gun
    if(!gun.addToRoundCount(ammoUsed.amount, ammoUsed.ammoType))
        return false;
    
    return true;
}
void Event::getAllGunsUsed(std::vector<std::pair<Gun, TrackedAmmo>>& list) const {
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [gun, ammo] : gunsUsed)
        list.emplace_back(gun, ammo);
}
uint8_t Event::getNumGunsUsed() const{
    uint8_t retVal { 0 };
    for (const auto& [gun, ammo] : gunsUsed){
        if(gun.getName() != "N/A")
            ++retVal;
    }
    return retVal;
}

bool Event::operator==(const Event& other) const{
    if(this->getLocation() == other.getLocation() && this->printDate() == other.printDate())
        return true;

    return false;
}
// MARK: To/from JSON
void ShooterCentral::to_json(LAS::json& j, const Event& event){
    using LAS::json;

    // Write every ammo type used and amount
    std::vector<std::pair<Gun, TrackedAmmo>> gunsUsed;
    event.getAllGunsUsed(gunsUsed);
    
	nlohmann::json gunsArray = nlohmann::json::array();
	for (const auto& [gun, ammo] : gunsUsed){
        if(gun.getName() != "N/A"){
            json gunsJson { gun, ammo }; 
            gunsArray.emplace_back(gunsJson);
        }
    }

    std::ostringstream timeBuf;
    timeBuf << event.getTimepoint();
    
    // Make JSON
    j = json{
        { "location",       event.getLocation() },
        { "eventType",      event.getEventType()},
        { "notes",          event.getNotes()    },
        { "date",           timeBuf.str()       },
        { "gunsUsed",       gunsArray           }
    };
}
void ShooterCentral::from_json(const LAS::json& j, Event& event){
    std::string nameBuf, locBuf, notesBuf, eventTypeBuf, dateStringBuf;

    j.at("location").get_to(locBuf);
    j.at("eventType").get_to(eventTypeBuf);
    j.at("notes").get_to(notesBuf);
    j.at("date").get_to(dateStringBuf);

    ymd dateBuf { std::chrono::floor<std::chrono::days>(EventHelper::stringToTimepoint(dateStringBuf)) };

    Event eventBuf {Location{locBuf}, EventType{eventTypeBuf}, notesBuf, dateBuf};
    
    nlohmann::json gunList;
	j.at("gunsUsed").get_to(gunList);
	
    // Add for each element
	for (auto& gun : gunList.items()) {
		nlohmann::json gunJson = gun.value();
        eventBuf.addGun(Gun{ gunJson.at(0).template get<ShooterCentral::Gun>()}, gunJson.at(1).template get<ShooterCentral::TrackedAmmo>() );
	}
    event = eventBuf;
}

// MARK: Event Tracker
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

// MARK: Add Items
bool EventTracker::addEvent (Event& event){
    if(events.contains(event))
        return false;

    events.try_emplace(event, std::make_shared<Event>(event));

    if(events.contains(event)){
        addEventType(event.getEventType());
        addLocation(event.getLocation());
        return true;
    }
    else
        return false;
}
bool EventTracker::addEventType (const EventType& type){
    if(eventTypes.contains(type))
        return false;

    eventTypes.try_emplace(type, type);
    return eventTypes.contains(type);
}
bool EventTracker::addLocation (const Location& location){
    if(locations.contains(location))
        return false;

    locations.try_emplace(location, location);
    return locations.contains(location);
}


// MARK: Get Info
void EventTracker::getAllEvents(std::vector<EventPtr>& list) const {
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, event] : events)
        list.emplace_back(event);
}
void EventTracker::getAllEventTypes(EventTypeList& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, value] : eventTypes){
        list.emplace_back(value);
    }
}
void EventTracker::getAllLocations(LocationList& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, value] : locations){
        list.emplace_back(value);
    }
}
int EventTracker::getTotalEvents() const{
    return events.size();
}

// MARK: Write
bool EventTracker::writeAllEvents () const{
    using LAS::json;

    if(events.empty())
        return true;

    if(!std::filesystem::exists(saveDirectory)){
        logger->log("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Event objects.", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }

    int eventsNotSaved { 0 };
	for(const auto& [key, event] : events) {
        if(!EventHelper::writeEvent(saveDirectory, *event)){
            logger->log("Failed to save Event [" + event->getName() + "]", LAS::Logging::Tags{"ERROR", "SC"});
            ++eventsNotSaved;
        }
	}

    if(eventsNotSaved > 0)
        logger->log("Could not save " + std::to_string(eventsNotSaved) + " events.", LAS::Logging::Tags{"ERROR", "SC"});
    else
        logger->log("Saved events", LAS::Logging::Tags{"ROUTINE", "SC"});

    return true;
}
bool EventTracker::writeAllEventTypes() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += EVENT_TYPES_FILENAME;

    EventTypeList rawEventTypes;

    for(const auto& [key, value] : eventTypes){
        rawEventTypes.emplace_back(value);
    }

    if(EventHelper::writeAllEventTypes(fullPath, rawEventTypes)){
        logger->log("Saved event types", LAS::Logging::Tags{"ROUTINE", "SC"});
        return true;
    }
    else{
        logger->log("Could not save event types", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
}
bool EventTracker::writeAllLocations() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += LOCATIONS_FILENAME;

    LocationList rawLocations;

    for(const auto& [key, value] : locations){
        rawLocations.emplace_back(value);
    }

    if(EventHelper::writeAllLocations(fullPath, rawLocations)){
        logger->log("Saved event locations", LAS::Logging::Tags{"ROUTINE", "SC"});
        return true;
    }
    else{
        logger->log("Could not save event locations", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }
}
// MARK: Read
bool EventTracker::readEvents(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    const std::filesystem::path workingDirectory{saveDirectory};
	for(auto const& dirEntry : std::filesystem::directory_iterator(workingDirectory)){
		try{
			Event eventBuf {EventHelper::readEvent(dirEntry.path().string())};

            if(!addEvent(eventBuf))
                logger->log("Event object already exists from file [" + dirEntry.path().string() + ']', LAS::Logging::Tags{"ROUTINE", "SC"});
		}
		catch(std::exception& e){
            if(dirEntry.path().filename().string() != EVENT_TYPES_FILENAME && dirEntry.path().filename().string() != LOCATIONS_FILENAME){  // Ignore the known files
                logger->log("Failed to create Event object from file [" + dirEntry.path().string() + ']', LAS::Logging::Tags{"ERROR", "SC"});
                logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
            }
		}
	}
    
	return true;
}
bool EventTracker::readEventTypes(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += EVENT_TYPES_FILENAME;

    EventTypeList eventTypesBuf;
    try{
        EventHelper::readEventTypes(fullPath, eventTypesBuf);
    }
    catch(std::exception& e){
        logger->log("Error reading Event Types", LAS::Logging::Tags{"ERROR", "SC"});
        logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
    }

    for(const auto& et : eventTypesBuf){
        if(!addEventType(et))
            logger->log("Event Type [" + et.getName() + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
    }

    return true;
}
bool EventTracker::readLocations(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += LOCATIONS_FILENAME;

    LocationList locationsBuf;
    try{
        EventHelper::readLocations(fullPath, locationsBuf);
    }
    catch(std::exception& e){
        logger->log("Error reading locations", LAS::Logging::Tags{"ERROR", "SC"});
        logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
    }

    for(const auto& location : locationsBuf){
        if(!addLocation(location))
            logger->log("Location named [" + location.getName() + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
    }

    return true;
}

// MARK: EVENT HELPER
bool EventHelper::writeEvent(std::string directory, const Event& event){
    using LAS::json;

    if(directory.empty())
       return false;

    directory = LAS::TextManip::ensureSlash(directory);

    if(!std::filesystem::exists(directory))
		return false;

    // Make JSON
    json j = event;

    // Create JSON file name
    std::string fileName;
    for(const auto& c : event.getName()){     // Remove spaces and make lowercase
        if(isalpha(c))
            fileName += tolower(c);
        else if(c == ' ' || c == '\t')
            fileName += '_';
        else if(isalnum(c))
            fileName += c;
    }
    fileName += ".json";

    // Make fully qualified path
    std::string filePath;
    filePath = directory + fileName;

    // Write to file
    std::ofstream file{filePath};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();
   
    return true;
}
Event EventHelper::readEvent(const std::string& path){
    using LAS::json;

    if(!std::filesystem::exists(path))
        throw std::filesystem::filesystem_error::runtime_error{"File does not exist"};

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    return Event{j.template get<ShooterCentral::Event>()};
}
std::chrono::system_clock::time_point EventHelper::stringToTimepoint(const std::string& timeString) {
    std::stringstream ss{ timeString };
    std::tm time{};
    time.tm_isdst = -1;
    ss >> std::get_time(&time, "%Y-%m-%d%n%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&time));
}
// MARK: R/W EVENT TYPES
bool EventHelper::writeAllEventTypes(std::string path, const EventTypeList& list){
    using LAS::json;

    if(list.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& eventType : list) 
        entries.emplace_back(eventType);
    
    j["eventTypes"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
bool EventHelper::readEventTypes (std::string path, EventTypeList& list){
    using LAS::json;

    if(!std::filesystem::exists(path))
        return false;

    if(!list.empty())
        list.erase(list.begin(), list.end());

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    json eventTypesJson;
	j.at("eventTypes").get_to(eventTypesJson);

	for (auto& elment : eventTypesJson.items()) {
		json obj = elment.value();
        std::string nameBuf;

		obj.get_to(nameBuf);

        list.emplace_back(nameBuf);
	}

    return true;
}
// MARK: R/W LOCATIONS
bool EventHelper::writeAllLocations(std::string path, const LocationList& locations){
    using LAS::json;

    if(locations.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& location : locations) 
        entries.emplace_back(location);
    
    j["locations"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
bool EventHelper::readLocations (std::string path, LocationList& locations){
    using LAS::json;

    if(!std::filesystem::exists(path))
        return false;

    if(!locations.empty())
        locations.erase(locations.begin(), locations.end());

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    json locationsJson;
	j.at("locations").get_to(locationsJson);

	for (auto& elment : locationsJson.items()) {
		json obj = elment.value();
        std::string nameBuf;

		obj.get_to(nameBuf);

        locations.emplace_back(nameBuf);
	}

    return true;
}