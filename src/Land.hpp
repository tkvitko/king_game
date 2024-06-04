//
//  Land.hpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#ifndef Land_hpp
#define Land_hpp

#include <stdio.h>
#include "exceptions.hpp"

class Land {
public:
    Land();
    Land(int farm_square, int forest_square);

    // получение данных
    int getFarmSquare();
    int getForestSquare();
    int getTotalSquare();
    int getPlantedSquare();
    int getSoldSquare();
    int getHarvestedSquare();
    double getHarvestMultiplyingFactor();

    // изменение данных
    void cutDownForest(int square);
    void sellFarmToIndustry(int square);
    void increaseHarvestMutliplyingFactor(double factor);
    void resetHarvestMutliplyingFactor();
    void plantFarmLand(int square);
    int harvest();

private:
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
};

#endif /* Land_hpp */
