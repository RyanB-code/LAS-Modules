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

}
