#pragma once

#include "Utilities.h"
#include "Mileage.h"

#include <stdexcept>
#include <cstring>
#include <ostream>
#include <cstdint>

namespace Garage {

    class ModelYear {
    public:
            ModelYear(int setModelYear);
            ~ModelYear(); 

            int     getModelYear() const;
            bool    setModelYear(int setModelYear);

            operator int() const;
            operator bool() const;
            
    private:
        int year { 0 };
    };

    std::ostream& operator<<(std::ostream& os, const ModelYear& my);
    
    class Vehicle {
    public:
        Vehicle(const char* name, ModelYear setModelYear, Mileage setMileage);   // Throws overflow_error if name is too long
        ~Vehicle();

        const char* getName()       const;
        ModelYear   getModelYear()  const;
        Mileage     getMiles()      const;

        bool setName        (const char* setName);
        bool setModelYear   (const ModelYear& setModelYear);
        void setMileage     (const Mileage& setMileage);        // Will add if setMileage < m_miles

        void addMiles(uint64_t add);

        operator bool() const;

    private:
        char name[MAX_CHAR_VEHICLE_NAME] = "NULL";

        ModelYear   modelYear;
        Mileage     miles;
    };

}
