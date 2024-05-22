//
//  People.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include "People.hpp"

class People {
public:
    People(int countrymen) : countrymen_(countrymen) {};
    
    // получение данных
    int getCountrymen() { return countrymen_;}
    int getForeigners() { return foreigners_;}
    bool isPeopleEnoughToPlantLand(int square) { return square * square_countryman_can_plant_ < countrymen_; }
    
    // изменение данных
    void decrease(int count) { countrymen_ -= count; }
    void increase(int count) { countrymen_ += count; }
    
    void setSquareCountrymanCanPlant(int square) {
        square_countryman_can_plant_ = square;
    }
    

    
private:
    int countrymen_ = 0;
    int foreigners_ = 0;
    
    int square_countryman_can_plant_ = 0;
};
