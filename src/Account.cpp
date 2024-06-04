//
//  Account.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include <iostream>
#include "Account.hpp"

Account::Account() {};
Account::Account(int balance) : balance_(balance) {};
    
// получение данных
int Account::getBalance() { return balance_;}
bool Account::isBalanceEnoughToPlantLand(int square) { return square * price_of_planting_land_ < balance_; }

int Account::getCountOfPeopleHasEnoughtToLive() {
    return spent_on_countrymen_ / price_of_living_;
}

int Account::getPriceOfSellingLand() { return price_of_selling_land_; }
int Account::getPriceOfPlantingLand() { return price_of_planting_land_; }
int Account::getAmountSpentOnPollutonControl() {return spent_on_pollution_control_;}
double Account::getPriceOfLivingMultiplyingFactor() { return price_of_living_multiplying_factor_; }
int Account::getPriceOfLiving() {return price_of_living_;}
int Account::getMaxCuttingDownSquareOfForest() { return balance_ / price_for_cutting_down_forest_; }

// изменение данных

// установка цен
void Account::setPrices(int price_of_selling_land, int price_of_planting_land,
               int price_for_cutting_down_forest, int price_of_living) {
    price_of_selling_land_ = price_of_selling_land;
    price_of_planting_land_ = price_of_planting_land;
    price_for_cutting_down_forest_ = price_for_cutting_down_forest;
    price_of_living_ = price_of_living * price_of_living_multiplying_factor_;
    price_of_living_multiplying_factor_ = 1.0;
}

void Account::sellFarmToIndustry(int square) {
    // учет дохода от продажи земли
    balance_ += square * price_of_selling_land_;
}

bool Account::spendMoneyToCountryman(int amount) {
    // распределение денег населению
    bool success = decrease_balance_(amount);
    if (success) {
        spent_on_countrymen_ = amount;
        return true;
    }
    return false;
}

bool Account::spendMoneyToPollutionControl(int amount) {
    // распределение денег населению
    bool success = decrease_balance_(amount);
    if (success) {
        spent_on_pollution_control_ = amount;
        return true;
    }
    return false;
}

bool Account::plantFarmLand(int square) {
    bool success = decrease_balance_(square * price_of_planting_land_);
    if (success) {
        return true;
    }
    return false;
}

bool Account::cutDownForest(int square) {
    bool success = decrease_balance_(square * price_for_cutting_down_forest_);
    if (success) {
        return true;
    }
    return false;
}

void Account::spendMoneyToFuneral(int amount, Land& land) {
    if (amount < balance_) {
        decrease_balance_(amount);
    } else {
        std::cout << "В казне не хватает денег на похороны, придётся продать часть земли под промышленность." << std::endl;
        int diff = amount - balance_;
        balance_ = 0;
        land.sellFarmToIndustry(diff / getPriceOfSellingLand());
    }
}

void Account::add_money(int amount) {
    balance_ += amount;
}

void Account::increasePriceOfLivingMultiplyingFactor(double factor) {
    price_of_living_multiplying_factor_ += factor;
}

void Account::resetPriceOfLivingMultiplyingFactor() {
    price_of_living_multiplying_factor_ = 1.0;
}


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

bool Account::decrease_balance_(int amount) {
    if (amount <= balance_) {
        balance_ -= amount;
        return true;
    }
    return false;
}
