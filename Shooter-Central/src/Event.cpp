#include "Event.h"

using namespace ShooterCentral;


// MARK: EVENT
Event::Event() : name { "N/A"}, location {"N/A"}, eventType{"N/A"}, notes{"N/A"}, date {std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())}{

}
Event::Event(std::string setName, std::string setLocation, std::string setEventType, std::string setNotes, ymd setDate)
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
std::string Event::getEventType() const{
    return eventType;
}
std::string Event::getLocation() const{
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

    if(gunsInList >= gunsUsed.max_size())
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
    if(this->name == other.getName() && this->location == other.getLocation() && this->printDate() == other.printDate())
        return true;

    return false;
}
// MARK: TO/FROM JSON
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
        { "name",           event.getName()     },
        { "location",       event.getLocation() },
        { "eventType",      event.getEventType()},
        { "notes",          event.getNotes()    },
        { "date",           timeBuf.str()       },
        { "gunsUsed",       gunsArray           }
    };
}
void ShooterCentral::from_json(const LAS::json& j, Event& event){
    std::string nameBuf, locBuf, notesBuf, eventTypeBuf, dateStringBuf;

    j.at("name").get_to(nameBuf);
    j.at("location").get_to(locBuf);
    j.at("eventType").get_to(eventTypeBuf);
    j.at("notes").get_to(notesBuf);
    j.at("date").get_to(dateStringBuf);

    ymd dateBuf { std::chrono::floor<std::chrono::days>(EventHelper::stringToTimepoint(dateStringBuf)) };

    Event eventBuf { nameBuf, locBuf, eventTypeBuf, notesBuf, dateBuf};
    
    nlohmann::json gunList;
	j.at("gunsUsed").get_to(gunList);
	
    // Add for each element
	for (auto& gun : gunList.items()) {
		nlohmann::json gunJson = gun.value();
        eventBuf.addGun(Gun{ gunJson.at(0).template get<ShooterCentral::Gun>()}, gunJson.at(1).template get<ShooterCentral::TrackedAmmo>() );
	}
    event = eventBuf;
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
uint64_t EventTracker::getTotalEvents  () const{
    return events.size();
}

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

void EventTracker::getAllEvents(std::vector<Event>& list) const {
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, event] : events)
        list.push_back(*event);
}
// MARK: R/W EVENTS
bool EventTracker::writeAllEvents () const{
    using LAS::json;

    if(events.empty())
        return true;

    if(!std::filesystem::exists(saveDirectory)){
        logger->log("Directory [" + saveDirectory + "] was not found. Did not attempt to save any Event objects.", LAS::Logging::Tags{"ERROR", "SC"});
        return false;
    }

	for(const auto& [key, event] : events) {
        if(!EventHelper::writeEvent(saveDirectory, *event)) 
            logger->log("Directory [" + saveDirectory + "] was not found. Event [" + event->getName() + "] was not saved.", LAS::Logging::Tags{"ERROR", "SC"});
	}

    return true;
}
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
// MARK: GET EVENT TYPES
void EventTracker::getAllEventTypes(StringVector& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, value] : eventTypes){
        list.emplace_back(value);
    }
}
// MARK: ADD EVENT TYPE
bool EventTracker::addEventType (const std::string& type){
    if(eventTypes.contains(type))
        return false;

    eventTypes.try_emplace(type, type);
    return eventTypes.contains(type);
}
// MARK: GET LOCATIONS
void EventTracker::getAllLocations(StringVector& list) const{
    if(!list.empty())
        list.erase(list.begin(), list.end());

    for(const auto& [key, value] : locations){
        list.emplace_back(value);
    }
}
// MARK: ADD LOCATION
bool EventTracker::addLocation (const std::string& location){
    if(locations.contains(location))
        return false;

    locations.try_emplace(location, location);
    return locations.contains(location);
}
// MARK: R/W EVENT TYPE
bool EventTracker::writeAllEventTypes() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += EVENT_TYPES_FILENAME;

    StringVector rawEventTypes;

    for(const auto& [key, value] : eventTypes){
        rawEventTypes.emplace_back(value);
    }

    return EventHelper::writeAllEventTypes(fullPath, rawEventTypes);
}
bool EventTracker::readEventTypes(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += EVENT_TYPES_FILENAME;

    StringVector eventTypesBuf;
    try{
        EventHelper::readEventTypes(fullPath, eventTypesBuf);
    }
    catch(std::exception& e){
        logger->log("Error reading Event Types", LAS::Logging::Tags{"ERROR", "SC"});
        logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
    }

    for(const auto& s : eventTypesBuf){
        if(!addEventType(s))
            logger->log("Event Type [" + s + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
    }

    return true;
}
// MARK: R/W EVENT TYPE
bool EventTracker::writeAllLocations() const{
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += LOCATIONS_FILENAME;

    StringVector rawLocations;

    for(const auto& [key, value] : locations){
        rawLocations.emplace_back(value);
    }

    return EventHelper::writeAllLocations(fullPath, rawLocations);
}
bool EventTracker::readLocations(){
    if(!std::filesystem::exists(saveDirectory))
        return false;

    std::string fullPath { saveDirectory };
    fullPath += LOCATIONS_FILENAME;

    StringVector locationsBuf;
    try{
        EventHelper::readLocations(fullPath, locationsBuf);
    }
    catch(std::exception& e){
        logger->log("Error reading locations", LAS::Logging::Tags{"ERROR", "SC"});
        logger->log("What: " + std::string{e.what()}, LAS::Logging::Tags{"CONTINUED"});
    }

    for(const auto& s : locationsBuf){
        if(!addLocation(s))
            logger->log("Location named [" + s + "] already exists.", LAS::Logging::Tags{"ROUTINE", "SC"});
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
bool EventHelper::writeAllEventTypes(std::string path, const StringVector& eventTypes){
    using LAS::json;

    if(eventTypes.empty())
        return true;

    json j;
    json entries = nlohmann::json::array();

	for (const auto& eventType : eventTypes) 
        entries.emplace_back(eventType);
    
    j["eventTypes"] = entries;

    // Write to file
    std::ofstream file{path};
    file << std::setw(1) << std::setfill('\t') << j;
    file.close();

    return true;
}
bool EventHelper::readEventTypes (std::string path, StringVector& eventTypes){
    using LAS::json;

    if(!std::filesystem::exists(path))
        return false;

    if(!eventTypes.empty())
        eventTypes.erase(eventTypes.begin(), eventTypes.end());

    std::ifstream inputFile{ path, std::ios::in };
    json j = json::parse(inputFile);

    json eventTypesJson;
	j.at("eventTypes").get_to(eventTypesJson);

	for (auto& elment : eventTypesJson.items()) {
		json obj = elment.value();
        std::string nameBuf;

		obj.get_to(nameBuf);

        eventTypes.emplace_back(nameBuf);
	}

    return true;
}
// MARK: R/W LOCATIONS
bool EventHelper::writeAllLocations(std::string path, const StringVector& locations){
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
bool EventHelper::readLocations (std::string path, StringVector& locations){
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