#include "Time.h"

using namespace ShooterCentral;

Datestamp::Datestamp(){
	
}
Datestamp::Datestamp(const timepoint& timestamp) {
	stamp(timestamp);
}
Datestamp::~Datestamp(){

}

bool Datestamp::stamp(const timepoint& timepoint) {	
	if (!m_isStamped) {
		time = timepoint;
		m_isStamped = true;
		return true;
	}
	else
		return false;
}
bool Datestamp::isStamped() const{
	return m_isStamped;
}

std::string Datestamp::printHMS() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time }; // Makes zoned time

	// Needed to output the seconds as 2 digits instead of a float
	std::string seconds {std::format("{:%S}", zonedTime)};
	std::ostringstream formattedTime;
	formattedTime << std::format("{:%H:%M:}", zonedTime) << seconds.substr(0,2);

    return formattedTime.str();
}

std::string Datestamp::printDay() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return std::format("{:%d}", zonedTime);
}
std::string Datestamp::printMonth() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return std::format("{:%B}", zonedTime);
}
std::string Datestamp::printDate() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return std::format("{:%Od %b %Y}", zonedTime);
}
int Datestamp::getDay()    const{
    std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
    std::stringstream stream;
    stream << std::format("{:%d}", zonedTime);

    int day { 0 };
    stream >> day;
    return day;
}
int Datestamp::getMonth()  const{
    std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
    std::stringstream stream;
    stream << std::format("{:%m}", zonedTime);

    int month { 0 };
    stream >> month;
    return month;
}
int Datestamp::getYear()   const{
    std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
    std::stringstream stream;
    stream << std::format("{:%Y}", zonedTime);

    int year { 0 };
    stream >> year;
    return year;
}
timepoint Datestamp::getRawTime() const {
	std::chrono::zoned_time zonedTime { std::chrono::current_zone(), time };
	return zonedTime;
}