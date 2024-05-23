//
//  Account.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include "Account.hpp"

class Account {
public:
    Account() {};
    Account(int balance) : balance_(balance) {};
    
    // получение данных
    int getBalance() { return balance_;}
    bool isBalanceEnoughToPlantLand(int square) { return square * price_of_planting_land_ < balance_; }
    
    int getCountOfPeopleHasEnoughtToLive() {
        return spent_on_countrymen_ / price_of_living_;
    }
    
    int getPriceOfSellingLand() { return price_of_selling_land_; }
    int getPriceOfPlantingLand() { return price_of_planting_land_; }
    int getAmountSpentOnPollutonControl() {return spent_on_pollution_control_;}
//    int getAmountSpentOnCountrymen() {return spent_on_countrymen_;}
//    int getPriceOfLiving() {return price_of_living_;}
    
    // изменение данных
    
    // установка цен
    void setPrices(int price_of_selling_land, int price_of_planting_land,
                   int price_for_cutting_down_forest, int price_of_living) {
        price_of_selling_land_ = price_of_selling_land;
        price_of_planting_land_ = price_of_planting_land;
        price_for_cutting_down_forest_ = price_for_cutting_down_forest;
        price_of_living_ = price_of_living;
    }
    
    void sellFarmToIndustry(int square) {
        // учет дохода от продажи земли
        balance_ += square * price_of_selling_land_;
    }
    
    void spendMoneyToCountryman(int amount) {
        // распределение денег населению
        balance_ -= amount;
        spent_on_countrymen_ = amount;
    }
    
    void spendMoneyToPollutionControl(int amount) {
        // распределение денег населению
        balance_ -= amount;
        spent_on_pollution_control_ = amount;
    }
    
    void plantFarmLand(int square) {
        balance_ -= square * price_of_planting_land_;
    }
    
    void cutDownForest(int square) {
        balance_ -= square * price_for_cutting_down_forest_;
    }
    
    void spendMoneyToFuneral(int amount, Land land, Account account) {
        if (amount < balance_) {
            balance_ -= amount;
        } else {
            int diff = amount - balance_;
            balance_ = 0;
            land.sellFarmToIndustry(diff / account.getPriceOfSellingLand())
        }
    }
    
    void add_money(int amount) {
        balance_ += amount;
    }
    
private:
    int balance_ = 60000;
    
    // данные конкретного года
    
    // цены
    int price_of_selling_land_ = 0;
    int price_of_planting_land_ = 0;
    int price_for_cutting_down_forest_ = 0;
    int price_of_living_ = 0;
    
    // решения
    int spent_on_countrymen_  = 0;
    int spent_on_pollution_control_ = 0;
};
