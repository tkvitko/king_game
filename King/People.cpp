//
//  People.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include "People.hpp"

class People {
public:
    People() {};
    People(int countrymen) : countrymen_(countrymen) {};
    
    // получение данных
    int getCountrymen() { return countrymen_;}
    int getForeigners() { return foreigners_;}
    bool isPeopleEnoughToPlantLand(int square) { return square * square_countryman_can_plant_ < countrymen_; }
    int getSquareCountrymanCanPlant() { return square_countryman_can_plant_; }
    int getPopulationChange() { return population_change_; }
    int getForeignersChange() { return foreigners_change_; }
    int getSettled() { return countrymen_ - population_change_; }
    double getTourismMultiplyingFactor() { return tourism_multiplying_factor; }
    
    // изменение данных
    void decrease(int count) { countrymen_ -= count; }
    void increase(int count) { countrymen_ += count; }
    void increaseForeigners(int count) {foreigners_ += count;}
    
    void setSquareCountrymanCanPlant(int square) {
        square_countryman_can_plant_ = square;
    }
    
    void setPopulationChange(int count) {
        if (countrymen_ + count < 0) {
            count = -countrymen_;
        }
        population_change_ = count;
        countrymen_ += count;
    }
    
    void setForeignersChange(int count) {
        foreigners_change_ = count;
        foreigners_ += count;
    }
    
    void setTourismMultiplyingFactor(double factor) {
        tourism_multiplying_factor = factor;
    }

    
private:
    int countrymen_ = 500;
    int foreigners_ = 0;
    int population_change_ = 0;
    int foreigners_change_ = 0;
    
    int square_countryman_can_plant_ = 0;
    double tourism_multiplying_factor = 1.0;
};
