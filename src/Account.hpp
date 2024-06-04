//
//  Account.hpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#ifndef Account_hpp
#define Account_hpp

#ifndef land_cpp
#define land_cpp
#include "Land.hpp"
#endif /* land_hpp */

#include <stdio.h>

class Account {
public:
    Account();
    Account(int balance);
    
    // получение данных
    int getBalance();
    bool isBalanceEnoughToPlantLand(int square);
    int getCountOfPeopleHasEnoughtToLive();
    int getPriceOfSellingLand();
    int getPriceOfPlantingLand();
    int getAmountSpentOnPollutonControl();
    double getPriceOfLivingMultiplyingFactor();
    int getPriceOfLiving();
    int getMaxCuttingDownSquareOfForest();
    
    // изменение данных
    
    // установка цен
    void setPrices(int price_of_selling_land, int price_of_planting_land,
                   int price_for_cutting_down_forest, int price_of_living);
    
    // учет дохода от продажи земли
    void sellFarmToIndustry(int square);
    
    // распределение денег населению
    bool spendMoneyToCountryman(int amount);
    
    // распределение денег на контроль загрязнений
    bool spendMoneyToPollutionControl(int amount);
    
    bool plantFarmLand(int square);
    bool cutDownForest(int square);
    void spendMoneyToFuneral(int amount, Land& land);
    void add_money(int amount);
    void increasePriceOfLivingMultiplyingFactor(double factor);
    void resetPriceOfLivingMultiplyingFactor();
    
private:
    unsigned int balance_ = 60000;
    
    // данные конкретного года
    
    // цены
    int price_of_selling_land_ = 0;
    int price_of_planting_land_ = 0;
    int price_for_cutting_down_forest_ = 0;
    int price_of_living_ = 0;
    double price_of_living_multiplying_factor_ = 1.0;
    
    // решения
    int spent_on_countrymen_  = 0;
    int spent_on_pollution_control_ = 0;
    
    bool decrease_balance_(int amount);
};

#endif /* Account_hpp */
