#include "Vehicle.h"

namespace Garage{

    // Model Year
    ModelYear::ModelYear(int setModelYear) {
        if(setModelYear < 1900)
            throw std::out_of_range("setModelYear < 1900");
        else
            year = setModelYear;
    }
    ModelYear::~ModelYear(){

    }
    int ModelYear::getModelYear() const {
        return year;
    }
    bool ModelYear::setModelYear(int setModelYear){
        if(setModelYear < 1900)
            return false;

        year = setModelYear;
        return true;
    }
    ModelYear::operator int() const {
        return year;
    }
    ModelYear::operator bool() const {
        if(year < 1900)
            return false;

        return true;
    }
    std::ostream& operator<<(std::ostream& os, const ModelYear& my){
        os << my.getModelYear();
        return os;
    }


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

    std::ostream& operator<<(std::ostream& os, const Mileage& miles) {
        os << miles.getMileage();
        return os;
    }


    // Vehicle
    Vehicle::Vehicle(const char* setName, ModelYear setModelYear, Mileage setMileage) : modelYear {setModelYear}, miles { setMileage } {
        if(std::strlen(setName) > MAX_CHAR_VEHICLE_NAME-1)
            throw std::overflow_error("Vehicle setName length > " TO_STRING(MAX_CHAR_VEHICLE_NAME) );
        else
            std::strcpy(name, setName);
    }
    Vehicle::~Vehicle(){

    }
    const char* Vehicle::getName() const {
        return name;
    }
    ModelYear Vehicle::getModelYear() const {
        return modelYear;
    }
    Mileage Vehicle::getMiles() const {
        return miles;
    }
    bool Vehicle::setName(const char* setName) {
        if(std::strlen(setName) > MAX_CHAR_VEHICLE_NAME-1)
            return false;
        else
            std::strcpy(name, setName);

        return true;
    }
    bool Vehicle::setModelYear(const ModelYear& setModelYear) {
        return modelYear.setModelYear(setModelYear.getModelYear());
    }
    void Vehicle::setMileage(const Mileage& setMiles){
        miles = setMiles;
    }
    void Vehicle::addMiles(uint64_t add) {
        miles.add(add);
    }
    Vehicle::operator bool() const {
        if(modelYear && std::strcmp(name, "NULL") != 0 )
            return true;

        return false;
    }

}
