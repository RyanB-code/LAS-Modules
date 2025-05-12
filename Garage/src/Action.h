#pragma once

#include "Mileage.h"

#include <stdexcept>
#include <format>
#include <chrono>

namespace Garage {

using ymd = std::chrono::year_month_day;

class YMD {
public:
    YMD(ymd setYMD); // Will throw invalid_argument f YMD is invalid
    YMD();
    ~YMD();

    void setDateToday();
    bool setDate(const ymd& setYMD);

    ymd getDate() const;

    std::string printAlpha() const;     // Format: 2025 May 09
    std::string printNumeric() const;   // Format: 2025 05 09

private:
    ymd date;
};


class Cost {
public:
    Cost(long long int setCost=0);
    ~Cost();

    int getScale() const;
    
    std::pair<long long, int> getCost() const;
    void setCost(long long setAmount);

    std::string printCost(bool dollarSign=false) const; // Prints 1234.56 or $1234.56

private:
    long long               cost    { 0 };
    static constexpr int    scale   { 100 };
};

/*
class ActionReminder { 
public:
    ActionReminder(
            YMD   time    = {std::chrono::sys_days{std::chrono::year_month_day{std::chrono::year{0}, std::chrono::month{0}, std::chrono::day{0}}}}
            Mileage     miles);
    ~ActionReminder();

private:
    Timepoint time;
    Mileage miles;
};

class Action {
public:
    Action(
            Timepoint                               time,
            Mileage                                 mileage,
            const char[MAX_TAGS, MAX_CHAR_TAG]      tags,
            Cost                                    cost    =Cost { },
            ActionReminder                          reminder=ActionReminder{ }
    );
    ~Action();

private:

};
*/

}
