//
//  Land.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include <iostream>
#include "Land.hpp"
#include "utils.hpp"
#include "exceptions.hpp"


class Land {
public:
    Land() {};
    Land(int farm_square, int forest_square) : farm_square_(farm_square), forest_square_(forest_square) {};

    // получение данных
    int getFarmSquare() { return farm_square_; }
    int getForestSquare() { return forest_square_; }
    int getTotalSquare() { return farm_square_ + forest_square_; }
    int getPlantedSquare() { return farm_square_planted_; }
    int getSoldSquare() { return farm_square_sold_; }
    int getHarvestedSquare() { return farm_square_harvested_; }
    double getHarvestMultiplyingFactor() { return harvest_multiplying_factor_; }

    // изменение данных
    void cutDownForest(int square) {
        // вырубка леса
        forest_square_ -= square;
        farm_square_ += square;
    };

    void sellFarmToIndustry(int square) {
        // продажа сельской земли под промышленность
        if (square > farm_square_) {
            std::cout << "Не хватает земли для продажи." << std::endl;
            throw GameOver();
        } else {
            farm_square_ -= square;
            farm_square_sold_ += square;
        }
    }
    
    void increaseHarvestMutliplyingFactor(double factor) {
        harvest_multiplying_factor_ += factor;
    }
    
    void resetHarvestMutliplyingFactor() {
        harvest_multiplying_factor_ = 1.0;
    }
    
    void plantFarmLand(int square) {
        // засев сельской земли
        if (square > farm_square_) {
            square = farm_square_;
        };
        farm_square_planted_ = square;
    }
        
    void harvest() {
        // сбор урожая
        // потеря урожая из-за промышленности
        short lost_farm_land = static_cast<int>(farm_square_sold_ * (getRandomFloatFromZeroToOne_() + 1.5) / 2);
        if (lost_farm_land > farm_square_planted_) {
            lost_farm_land = farm_square_planted_;
        }
        farm_square_harvested_ = farm_square_planted_ - lost_farm_land;
        farm_square_harvested_ *= harvest_multiplying_factor_;
    }
    
//    void resetPlanted() {
//        farm_square_planted_ = 0;
//        farm_square_harvested_ = 0;
//    }

private:
    // общие данные
    int farm_square_ = 1000;
    int forest_square_ = 1000;
    double harvest_multiplying_factor_ = 1.0;
    
    // данные конкретного года
    int farm_square_planted_ = 0;
    int farm_square_harvested_ = 0;
    int farm_square_sold_ = 0;
};
