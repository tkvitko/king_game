//
//  Land.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include <iostream>
#include "Land.hpp"
#include "utils.hpp"


Land::Land() {};
Land::Land(int farm_square, int forest_square) : farm_square_(farm_square), forest_square_(forest_square) {};

// получение данных
int Land::getFarmSquare() { return farm_square_; }
int Land::getForestSquare() { return forest_square_; }
int Land::getTotalSquare() { return farm_square_ + forest_square_; }
int Land::getPlantedSquare() { return farm_square_planted_; }
int Land::getSoldSquare() { return farm_square_sold_; }
int Land::getHarvestedSquare() { return farm_square_harvested_; }
double Land::getHarvestMultiplyingFactor() { return harvest_multiplying_factor_; }

// изменение данных
void Land::cutDownForest(int square) {
    // вырубка леса
    forest_square_ -= square;
    farm_square_ += square;
};

void Land::sellFarmToIndustry(int square) {
    // продажа сельской земли под промышленность
    if (square > farm_square_) {
        std::cout << "Не хватает земли для продажи." << std::endl;
        throw GameOver();
    } else {
        farm_square_ -= square;
        farm_square_sold_ = square;
    }
}

void Land::increaseHarvestMutliplyingFactor(double factor) {
    harvest_multiplying_factor_ += factor;
}

void Land::resetHarvestMutliplyingFactor() {
    harvest_multiplying_factor_ = 1.0;
}

void Land::plantFarmLand(int square) {
    // засев сельской земли
    if (square > farm_square_) {
        square = farm_square_;
    };
    farm_square_planted_ = square;
}
    
int Land::harvest() {
    // сбор урожая
    // потеря урожая из-за промышленности
    int lost_farm_land = static_cast<int>(farm_square_sold_ * (getRandomFloatFromZeroToOne_() + 1.5) / 2);
    if (lost_farm_land > farm_square_planted_) {
        lost_farm_land = farm_square_planted_;
    }
    farm_square_harvested_ = farm_square_planted_ - lost_farm_land;
    farm_square_harvested_ *= harvest_multiplying_factor_;
    
    if (lost_farm_land > 0) {
        int harvesting_worse_trend = lost_farm_land - last_year_lost_farm_land_;
        last_year_lost_farm_land_ = lost_farm_land;
        return harvesting_worse_trend;
    }
    return 0;
}

// общие данные
int farm_square_ = 1000;
int forest_square_ = 1000;
double harvest_multiplying_factor_ = 1.0;

// данные конкретного года
int farm_square_planted_ = 0;
int farm_square_harvested_ = 0;
int farm_square_sold_ = 0;

// данные прошлого года
int last_year_lost_farm_land_;
