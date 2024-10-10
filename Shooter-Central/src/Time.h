#pragma once

#include "LAS/json.h"


#include <chrono>
#include <format>
#include <istream>

namespace ShooterCentral{

    using timepoint = std::chrono::zoned_time<std::chrono::_V2::system_clock::duration, const std::chrono::time_zone *>;
    using sysclock 	= std::chrono::system_clock;
    using ymd		= std::chrono::year_month_day;

    class Datestamp {
    public:
        Datestamp();
        Datestamp(const timepoint& time);
        virtual ~Datestamp();

        bool stamp(const timepoint& time = std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()});

        bool isStamped() 			const;

        std::string printHMS()		const;		// Prints the HH:MM::SS			(14:23:01)

        std::string printDay()		const;
        std::string printMonth()	const;      // Prints full month name
        std::string printDate()		const;		// Prints in DD MMM YYYY format (12 Dec 2022)

        int getDay()    const;
        int getMonth()  const;
        int getYear()   const;

        timepoint	getRawTime()	const;

    private:
        timepoint time{};
        bool m_isStamped{ false };
    };
}