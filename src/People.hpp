//
//  People.hpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#ifndef People_hpp
#define People_hpp

class People {
public:
    People();
    People(int countrymen);
    
    // получение данных
    int getCountrymen();
    int getForeigners();
    bool isPeopleEnoughToPlantLand(int square);
    int getSquareCountrymanCanPlant();
    int getPopulationChange();
    int getForeignersChange();
    int getSettled();
    double getTourismMultiplyingFactor();
    int getStartCountrymen();
    
    // изменение данных
    void decrease(int count);
    void increase(int count);
    void increaseForeigners(int count);
    void setSquareCountrymanCanPlant(int square);
    void setPopulationChange(int count);
    void setForeigners(int count);
    void setForeignersChange(int count);
    void increaseTourismMultiplyingFactor(double value);
    void resetTourismMultiplyingFactor();

    
private:
    int countrymen_ = 500;
    int start_countrymen_ = 500;
    int foreigners_ = 0;
    int population_change_ = 0;
    int foreigners_change_ = 0;
    
    int square_countryman_can_plant_ = 2;
    double tourism_multiplying_factor_ = 1.0;
};


#include <stdio.h>

#endif /* People_hpp */
