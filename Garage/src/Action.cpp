#include "Action.h"

namespace Garage {

// YMD
YMD::YMD(ymd setYMD) : date { setYMD } {
    if(!date.ok())
        throw std::invalid_argument("Invalid date");
}
YMD::~YMD(){

}
YMD::YMD() : date { std::chrono::year_month_day{std::chrono::year{0}, std::chrono::month{1}, std::chrono::day{1} } }{

}
void YMD::setDateToday() {
    std::chrono::zoned_time now { std::chrono::current_zone(), std::chrono::system_clock::now() };

    date = std::chrono::year_month_day { std::chrono::floor<std::chrono::days>(now.get_local_time()) };
}
bool YMD::setDate(const ymd& setYMD){
    if(!setYMD.ok())
        return false;

    date = setYMD;
    return true;
}
ymd YMD::getDate() const {
    return date;
}
std::string YMD::printAlpha() const {
    return std::format("{:%Y %b %d}", date);
}
std::string YMD::printNumeric() const {
    return std::format("{:%Y %m %d}", date);
}
bool YMD::ok() const {
    return date.ok();
}


// Cost
Cost::Cost(long long setCost) : cost { setCost } {

}
Cost::~Cost(){

}
int Cost::getScale() const {
    return scale;
}
std::pair<long long, int> Cost::getCost() const {
    return std::pair(cost, scale);
}
void Cost::setCost(long long setAmount) {
    cost = setAmount;
}
std::string Cost::printCost(bool dollarSign) const {
    double val { static_cast<double>(cost) / scale };

    if(dollarSign) 
        return std::format("${:.2f}", val); 
    else
        return std::format("{:.2f}", val);
}


// Action Reminder
ActionReminder::ActionReminder(YMD setTime, Mileage setMiles) : time { setTime }, miles { setMiles } {

}
ActionReminder::~ActionReminder(){

}
YMD ActionReminder::getDate() const {
    return time;
}
Mileage ActionReminder::getMileage() const {
    return miles;
}
bool ActionReminder::setDate(const YMD& setTime) {
    if(!setTime.ok())
        return false;

    time = setTime;
    return true;
}
void ActionReminder::setMileage(const Mileage& setMileage) {
    miles = setMileage;
}


// Tags
Tags::Tags(const char* tag) {
    if(!addTag(tag))
        throw std::invalid_argument("Invalid tag");
}
Tags::Tags(const char setTagsBuffer[][MAX_CHAR_TAG], short numTags) {
    clearTags();
    if(!setTags(setTagsBuffer, numTags))
        throw std::out_of_range("Invalid Tags");
}
Tags::Tags() {
    clearTags();
}
Tags::Tags(const Tags& other){
    char buffer[MAX_TAGS][MAX_CHAR_TAG] = { };
    short numTags { };
    other.getAllTags(buffer, &numTags);

    if(!setTags(buffer, numTags))
        throw std::out_of_range("Invalid Tags");
}

Tags::~Tags(){

}
bool Tags::setTags(const char setTags[][MAX_CHAR_TAG], short numTags) {
    // Empty matrix or invalid tag amount
    if(setTags[0][0] == 0 || numTags > MAX_TAGS || numTags <= 0)
        return false;

    for(int curRow { 0 }; curRow < numTags; ++curRow){
        if(!addTag(setTags[curRow])){
            clearTags();
            return false;
        }
    }

    return true;
}
bool Tags::addTag(const char* tag){
    if(tag == nullptr || nextTag >= MAX_TAGS || tag == 0 || std::strlen(tag) > MAX_CHAR_TAG-1)
        return false;

    std::strcpy(tags[nextTag], tag); 
    ++nextTag;
    return true;
}
void Tags::clearTags() {
    for(int curRow { 0 }; curRow < MAX_TAGS; ++curRow)
        std::memset(tags[curRow], 0, MAX_CHAR_TAG * sizeof(char));
}
void Tags::getAllTags(char buffer[MAX_TAGS][MAX_CHAR_TAG], short* numTags) const{
    for(int curRow { 0 }; curRow < nextTag; ++curRow)
        std::strcpy(buffer[curRow], tags[curRow]); 

    if(numTags)
        *numTags = nextTag;
}
bool Tags::contains(const char* key) const {
    if(nextTag == 0)
        return false;

    if(key == nullptr)
        throw std::invalid_argument("Key cannot be null");

    char keyBuffer[MAX_CHAR_TAG] = { };

    for(size_t i { 0 }; i < std::strlen(key); ++i)
        keyBuffer[i] = std::toupper(key[i]);


    // Compare against all tags
    for(int curRow { 0 }; curRow < nextTag; ++curRow){
        char tagBuffer[MAX_CHAR_TAG] = { };

        for(size_t i { 0 }; i < std::strlen(tags[curRow]); ++i)
            tagBuffer[i] = std::toupper(tags[curRow][i]);

        if(std::strcmp(keyBuffer, tagBuffer) == 0)
            return true;
    }

    return false;
}



// Action
Action::Action(
            YMD             setTime,
            Mileage         setMileage,
            const char      setTags[][MAX_CHAR_TAG],
            short           numTags,
            const char      setNotesVar[MAX_CHAR_ACTION_NOTES],
            Cost            setCost,
            ActionReminder  setReminder
    ) :
            time        { setTime },
            miles       { setMileage },
            cost        { setCost },
            reminder    { setReminder }
{
    clearTags();
    if(!setAllTags(setTags, numTags) || !setNotes(setNotesVar) || numTags > MAX_TAGS )
        throw std::out_of_range("Invalid tags");

}
Action::~Action() {

}
bool Action::setAllTags(const char setTags[][MAX_CHAR_TAG], short numTags) {
    // Empty matrix or invalid tag amount
    if(setTags[0][0] == 0 || numTags > MAX_TAGS || numTags <= 0)
        return false;

    for(int curRow { 0 }; curRow < numTags; ++curRow){
        if(std::strlen(setTags[curRow]) > MAX_CHAR_TAG-1){ // If any of the tags are too long, return false
            clearTags();
            return false;
        }
        std::strcpy(tags[curRow], setTags[curRow]); // Set for that row
    }

    return true;
}
bool Action::setNotes(const char setNotes[MAX_CHAR_ACTION_NOTES]){
    if(std::strlen(setNotes) > MAX_CHAR_ACTION_NOTES-1)
        return false;
    
    std::strcpy(notes, setNotes);
    
    return true;
}
void Action::clearTags() {
    for(int curRow { 0 }; curRow < MAX_TAGS; ++curRow)
        std::memset(tags[curRow], 0, MAX_CHAR_TAG * sizeof(char));
}
void Action::getAllTags(char buffer[MAX_TAGS][MAX_CHAR_TAG]) const{
    for(int curRow { 0 }; curRow < MAX_TAGS; ++curRow)
        std::strcpy(buffer[curRow], tags[curRow]); 
}
void Action::getNotes(char buffer[MAX_CHAR_ACTION_NOTES]) const{
    std::strcpy(buffer, notes); 
}

YMD Action::getYMD() const {
    return time;
}
Mileage Action::getMiles() const {
    return miles;
}
Cost Action::getCost() const {
    return cost;
}
ActionReminder Action::getReminder() const {
    return reminder;
}



}
