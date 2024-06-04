//
//  Game.hpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include "GameResult.hpp"
#include "events.hpp"
#include "constants.h"
#include "exceptions.hpp"

#ifndef land_hpp
#define land_hpp
#include "Land.hpp"
#endif /* land_hpp */

#include "People.hpp"
#include "Account.hpp"
#include "utils.hpp"
#include "strings.hpp"

class NextYearParams {
public:
    NextYearParams();
    int getCountrymanChange();
    void increaseCountrymanChange(int count);
    void resetCountrymanChange();
    
private:
    int countrymen_change_ = 0;
};


class Game {
public:
    
    int START_BALANCE_MIN = 59000;
    int START_BALANCE_MAX = 61000;
    int START_COUNTRYMAN_MIN = 490;
    int START_COUNTRYMAN_MAX = 510;

    int PRICE_OF_SELLING_LAND_MIN = 95;
    int PRICE_OF_SELLING_LAND_MAX = 105;
    int PRICE_OF_PLANTING_LAND_MIN = 10;
    int PRICE_OF_PLANTING_LAND_MAX = 15;
    int PRICE_OF_CUTTING_DOWN_FOREST = 10;

    int PRICE_OF_LIVING = 100;
    int PRICE_OF_FUNERAL = 9;
    int POLLUTION_CONTROL_FACTOR = 25;
    
    Game(bool custom_game, bool random_events_enabled, bool competition);
    void showStateOnYearStart();
    void getYearDecisionsFromUser();
    void processYear();
    
private:
    
    void sellLandToIndustry_();
    void distributeMoneyToCountryman_();
    void plantFarmLand_();
    void spentMoneyToPollutionControl_();
    void cutDownForest_();
    void countDeaths_();
    void countPeople_();
    void countHarvest_();
    void countTourists_();
    void ShowStateOnYearEnd();
    void checkIfGameOver_();
    void process_random_events_();
    void processEvent_(Event event);
    void playQuiz_(Quiz quize);
    
    bool custom_game_;
    bool random_events_enabled_ = false;
    bool competition_ = false;
    
    short year_ = 0;
    Land land_;
    People people_;
    Account account_;
    NextYearParams next_year_params_;
    Events events = Events();
    Quizes quizes = Quizes();
    
    int died_count_ = 0;    // в другой класс?
    int died_because_of_pollution_ = 0;
    int last_year_tourists_revenue_ = 0;
};

#endif /* Game_hpp */
