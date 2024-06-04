//
//  People.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include "People.hpp"


People::People() {};
People::People(int countrymen) : countrymen_(countrymen) {
        start_countrymen_ = countrymen;
    };
    
// получение данных
int People::getCountrymen() { return countrymen_;}
int People::getForeigners() { return foreigners_;}
bool People::isPeopleEnoughToPlantLand(int square) { return square * square_countryman_can_plant_ < countrymen_; }
int People::getSquareCountrymanCanPlant() { return square_countryman_can_plant_; }
int People::getPopulationChange() { return population_change_; }
int People::getForeignersChange() { return foreigners_change_; }
int People::getSettled() { return countrymen_ - population_change_; }
double People::getTourismMultiplyingFactor() { return tourism_multiplying_factor_; }
int People::getStartCountrymen() { return start_countrymen_; }

// изменение данных
void People::decrease(int count) { countrymen_ -= count; }
void People::increase(int count) { countrymen_ += count; }
void People::increaseForeigners(int count) {foreigners_ += count;}

void People::setSquareCountrymanCanPlant(int square) {
    square_countryman_can_plant_ = square;
}

void People::setPopulationChange(int count) {
    if (countrymen_ + count < 0) {
        count = -countrymen_;
    }
    population_change_ = count;
    countrymen_ += count;
}

void People::setForeigners(int count) {
    foreigners_ = count;
}

void People::setForeignersChange(int count) {
    foreigners_change_ = count;
    foreigners_ += count;
}

void People::increaseTourismMultiplyingFactor(double value) {
    tourism_multiplying_factor_ += value;
}

void People::resetTourismMultiplyingFactor() {
    tourism_multiplying_factor_ = 1.0;
}

int countrymen_ = 500;
int start_countrymen_ = 500;
int foreigners_ = 0;
int population_change_ = 0;
int foreigners_change_ = 0;

int square_countryman_can_plant_ = 2;
double tourism_multiplying_factor_ = 1.0;
