#pragma once
#include "Mileage.h"
#include "Utilities.h"

#include <stdexcept>
#include <cstring>
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

    bool ok() const;

private:
    ymd date;
};


enum class MoneyUnit {
    DOLLAR,
    CENTS
};

class Cost {
public:
    Cost(long long int setCost=0, MoneyUnit unit=MoneyUnit::DOLLAR);
    ~Cost();

    int getScale() const;
    
    std::pair<long long, int> getCost() const;
    void setCost(long long setAmount, MoneyUnit unit);      // Throws domain_error if MoneyUnit case isnt handled

    std::string printDollar(bool dollarSign=false) const;   // Prints 1234.56 or $1234.56

private:
    long long               cost    { 0 };
    static constexpr int    scale   { 100 };
};


class ActionReminder { 
public:
    ActionReminder( YMD time=YMD{}, Mileage setMiles=Mileage{} ); 
    ~ActionReminder();

    YMD     getDate()       const;
    Mileage getMileage()    const;

    bool    setDate     (const YMD& setTime);
    void    setMileage  (const Mileage& setMileage);

private:
    YMD     time; 
    Mileage miles;

};

class Tags {
public:
    Tags(const char* tag);                                              // Throws out_of_range if Tags are invalid
    Tags(const char setTags[][MAX_CHAR_TAG], short numTags);            // Throws out_of_range if Tags are invalid
    Tags();
    Tags(const Tags& other);
    ~Tags();

    bool setTags (const char setTags[][MAX_CHAR_TAG], short numTags);   // Overwrites from beginning
    bool addTag(const char* tag);

    void clearTags();

    void getAllTags(char buffer[MAX_TAGS][MAX_CHAR_TAG], short* numTags=nullptr) const;
    bool contains(const char* key) const;                               // Ignores casing, Throws invalid_arg if key == nullptr

private:
    short nextTag { 0 };
    char tags[MAX_TAGS][MAX_CHAR_TAG] = { };
};


class Action {
public:
    Action(
            YMD             setTime,
            Mileage         setMileage,
            Tags            setTags,
            const char*     setNotes    =nullptr,
            Cost            setCost     =Cost{ },
            ActionReminder  setReminder =ActionReminder{ }
          );
    ~Action();
    
    bool setYMD         (const YMD& setYMD);
    void setMileage     (const Mileage& setMileage);
    void setTags        (const Tags& tags);
    bool setNotes       (const char* setNotes);
    void setCost        (const Cost& setCost);
    bool setReminder    (const ActionReminder& setReminder);

    YMD             getYMD()        const;
    Mileage         getMileage()    const;
    const Tags&     getTags()       const;
    Cost            getCost()       const;
    ActionReminder  getReminder()   const;
    bool            getNotes(char* destination, size_t destSize) const;


private:
    YMD                 time;
    Mileage             miles;
    Tags                tags;
    char                notes[MAX_CHAR_ACTION_NOTES] = { };
    Cost                cost;
    ActionReminder      reminder;
};


}
