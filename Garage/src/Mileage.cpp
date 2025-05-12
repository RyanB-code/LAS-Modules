#include "Mileage.h"

namespace Garage {

    // Mileage
    Mileage::Mileage(uint64_t setMileage) : mileage { setMileage } {

    }
    Mileage::~Mileage(){

    }
    uint64_t Mileage::getMileage() const {
        return mileage;
    }
    bool Mileage::setMileage(uint64_t setMileage) {
        if(setMileage <= mileage)
            return false;
        
        mileage = setMileage;
        return true;
    }
    void Mileage::add(uint64_t miles){
        mileage += miles;
    }
    Mileage::operator int() const{
        return mileage;
    }
    bool Mileage::operator==(const Mileage& other) const {
        return this->mileage == other.getMileage();
    }

    std::ostream& operator<<(std::ostream& os, const Mileage& miles) {
        os << miles.getMileage();
        return os;
    }
}
