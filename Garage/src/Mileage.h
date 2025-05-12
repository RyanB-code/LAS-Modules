#pragma once

#include <cstdint>
#include <ostream>


namespace Garage {

class Mileage {
public:
    Mileage(uint64_t setMileage=0);
    ~Mileage();

    uint64_t    getMileage() const;
    bool        setMileage(uint64_t setMileage);
    void        add(uint64_t miles);

    operator int() const;
    bool operator==(const Mileage& other) const;

private:
    uint64_t mileage;
};

std::ostream& operator<<(std::ostream& os, const Mileage& miles);

}
