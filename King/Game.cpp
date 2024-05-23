//
//  Game.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include "Game.hpp"

#include "constants.h"


#ifndef land_cpp
#define land_cpp
#include "Land.cpp"
#endif /* land_cpp */

#include "People.cpp"
#include "Account.cpp"
#include "utils.hpp"
#include "strings.hpp"


class NextYearParams {
public:
    NextYearParams() {};
    
    int getCountrymanChange() { return countrymen_change_; }
    
    void setCountrymanChange(int count) {
        countrymen_change_ = count;
    }
    
private:
    int countrymen_change_ = 0;
};


struct GameResult {
    int years;
    int balance;
    int countrymen;
};


template <>
struct std::hash<GameResult> {
    size_t operator()(const GameResult& obj) const {
        size_t hashValue = (
                            std::hash<int>()(obj.years) << 1)
                            ^ (std::hash<int>()(obj.balance) << 2)
                            ^ ((std::hash<int>()(obj.countrymen)) << 3)
                            ^ (std::hash<std::string>()("dialas")
                               );
        return hashValue;
    }
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
    int PRICE_OF_CUTTING_DONW_FOREST = 10;

    int PRICE_OF_LIVING = 100;
    int PRICE_OF_FUNERAL = 9;
    int POLLUTION_CONTROL_FACTOR = 25;
    
    Game() {
        // инициализация новой игры
        
        land_ = Land(1000, 1000);
        people_ = People(get_random_short_from_range(START_COUNTRYMAN_MIN, START_COUNTRYMAN_MAX));
        account_ = Account(get_random_short_from_range(START_BALANCE_MIN, START_BALANCE_MAX));
    };
    
    void printHeader() {
        // вывести приветствие
        
        std::cout << "\n### СРОЧНО НУЖЕН ПРЕМЬЕР-МИНИСТР! ###\n" << std::endl;
        std::cout << "(на основе игры The King, опубликованной в Basic Computer Games в 1978)" << std::endl;
        std::cout << "Author: @taraskvitko" << std::endl;
        std::cout << "Powered by Dialas" << std::endl;
        std::cout << "Version 1.6.5\n\n\n" << std::endl;
    }
    
    void printState() {
        // вывести состояние игры
        
        std::cout << year_ << " год правления" << std::endl;
        std::cout << "В казне " << account_.getBalance() << " ролодов" << std::endl;
        std::cout << "В стране проживает " << people_.getCountrymen() << " жителей";
        if (people_.getForeigners() > 0) {
            std::cout << " и " << people_.getForeigners() << " иностранных рабочих" << std::endl;
        } else {
            std::cout << std::endl;
        };
        std::cout << "У нас " << land_.getTotalSquare() << " квадратных миль земли: " << land_.getFarmSquare() << " сельхоз и " << land_.getForestSquare() << " леса" << std::endl;
        std::cout << "В этом году промышленность готова платить " << account_.getPriceOfSellingLand() << " роллодов за квадратную милю, а стоимость засева одной квадратной мили равна " << account_.getPriceOfPlantingLand() << std::endl ;
    }
    
    void getYearDecisionsFromUser() {
        // задать пользователю вопросы и применить ответы
        
        this->sellLandToIndustry_();
        this->distributeMoneyToCountryman_();
        this->plantFarmLand_();
        this->spentMoneyToPollutionControl_();
        this->cutDownForest_();
    }
    
    void processYear() {
        // начать новый год
        
        year_ += 1;
        account_.setPrices(get_random_short_from_range(PRICE_OF_SELLING_LAND_MIN, PRICE_OF_SELLING_LAND_MAX),
                           get_random_short_from_range(PRICE_OF_PLANTING_LAND_MIN, PRICE_OF_PLANTING_LAND_MAX),
                           PRICE_OF_CUTTING_DONW_FOREST,
                           PRICE_OF_LIVING);
        
        printState();
        getYearDecisionsFromUser();

        countDeaths_();
        countPeople_();
        countHarvest_();
        countTourists_();
        
        ShowCurrentState_();
        checkIfGameOver_();
        
        // применить изменение населения на следующий год
        people_.increase(next_year_params_.getCountrymanChange());
        
        next_year_params_.setCountrymanChange(0);
        account_.setPriceOfLivingMultiplyingFactor(1.0);
    }
    
    
    
private:
    
    void sellLandToIndustry_() {
        // процесс продажи земли иностранной промышленности
        std::cout << "Сколько квадратных миль земли вы хотите продать под промышленность? ";
        int square = get_valid_integer_input(0, land_.getFarmSquare(), true);
        land_.sellFarmToIndustry(square);       // изменения земель
        account_.sellFarmToIndustry(square);    // изменения бюджета
    }
    
    void distributeMoneyToCountryman_() {
        // процесс распределения денег по жителям
        std::cout << "Сколько роллодов вы отдадите своим жителям? ";
        int amount = get_valid_integer_input(0, account_.getBalance(), true);
        account_.spendMoneyToCountryman(amount);   // изменения бюджета
    }
    
    void plantFarmLand_() {
        // процесс засева сельхоз земель
        std::cout << "Сколько квадратных миль земли вы хотите засеять? ";
        
        std::vector<int> squares = {land_.getFarmSquare(),
            people_.getCountrymen() * people_.getSquareCountrymanCanPlant(),
            account_.getBalance() / account_.getPriceOfPlantingLand()};
        std::vector<int>::iterator result;
        result = std::max_element(squares.begin(), squares.end());
        
        int max_square_to_plant = *result;
        int square = get_valid_integer_input(0, max_square_to_plant, true);
        account_.plantFarmLand(square);  // изменения бюджета
        land_.plantFarmLand(square);     // вспашка и сбор урожая
    }
    
    void spentMoneyToPollutionControl_() {
        // процесс вкладывания денег в контроль загрязнений
        std::cout << "Сколько роллодов вы хотите потратить на контроль загрязнений? ";
        int amount = get_valid_integer_input(0, account_.getBalance(), true);
        account_.spendMoneyToPollutionControl(amount);
    }
    
    void cutDownForest_() {
        // процесс вырубки леса
        std::cout << "Сколько квадратных миль леса вы хотите вырубить? ";
        int square = get_valid_integer_input(0, land_.getForestSquare(), true);
        
        if (square != 0) {
            land_.cutDownForest(square);
            account_.cutDownForest(square);
            short deaths_when_cutting = get_random_choise(2, 50);
            if (deaths_when_cutting == 1) {
                int dead_count = people_.getCountrymen() * 0.1;
                people_.decrease(dead_count);
                std::cout << "Случилось несчастье при рубке! " << dead_count << " жителей завалило деревьями." << std::endl;
            }
        }
    }
    
    void countDeaths_() {
        
        // погибшие от голода
        short died_because_of_no_money = people_.getCountrymen() - account_.getCountOfPeopleHasEnoughtToLive();
        if (died_because_of_no_money > 0) {
            std::cout << died_because_of_no_money << " жителей умерло от голода" << std::endl;
        } else {
            // если жиетелей по какой-то причине стало меньше, чем было в начале года, starved будет отрицательным
            died_because_of_no_money = 0;
        };
        
        // погибшие от загрязнений
        // базовое значение - процент от площади земли, проданной под промышленность
        died_because_of_pollution_ = static_cast<int>(getRandomFloatFromZeroToOne_() * (land_.getSoldSquare()));
        // уменьшение базового значения, если на контроль загрязнений было выделено больше порога (фактора)
        if (account_.getAmountSpentOnPollutonControl() >= POLLUTION_CONTROL_FACTOR) {
            died_because_of_pollution_ = static_cast<int>(died_because_of_pollution_ /
                                                         (account_.getAmountSpentOnPollutonControl() / POLLUTION_CONTROL_FACTOR));
        }
        
        // если от загрязнений погибло больше, чем осталось, считаем, что погибли все оставшиеся
        if (died_because_of_pollution_ > people_.getCountrymen() - died_because_of_no_money) {
            died_because_of_pollution_ = people_.getCountrymen() - died_because_of_no_money;
        }
        if (died_because_of_pollution_ > 0) {
            std::cout << died_because_of_pollution_ << " жителей погибло из-за углеродного и пылевого загрязнения" << std::endl;
        }
            
        // затраты на похороны
        died_count_ = died_because_of_no_money + died_because_of_pollution_;
        if (this->died_count_ > 0) {
            int funeral_cost = died_count_ * PRICE_OF_FUNERAL;
            std::cout << "Пришлось потратить " << funeral_cost << " роллодов на похороны" << std::endl;
            account_.spendMoneyToFuneral(funeral_cost, land_);
        }
        people_.decrease(died_count_);
    }
    
    void countPeople_() {
        
        // иностранные рабочие
        if (land_.getSoldSquare() != 0) {
            people_.setForeignersChange(static_cast<int>(land_.getSoldSquare() + getRandomFloatFromZeroToOne_() * 10 - getRandomFloatFromZeroToOne_() * 20));
            if (people_.getForeigners() < 0) {
                people_.setForeignersChange(people_.getForeignersChange() + 20);
            }
            std::cout << people_.getForeignersChange() << " иностранных рабочих приехало в страну"  << std::endl;
        }
        
        // новые жители
        long over_disctibuted_money = account_.getCountOfPeopleHasEnoughtToLive() - people_.getCountrymen();
        people_.setPopulationChange(static_cast<int>(over_disctibuted_money / 10
                                                   + account_.getAmountSpentOnPollutonControl() / POLLUTION_CONTROL_FACTOR
                                                   - (land_.getSoldSquare() / 50 )
                                                   - this->died_count_ / 2 ));
        
        if (people_.getPopulationChange() != 0) {
            std::cout << "Население изменилось: ";
            if (people_.getPopulationChange() < 0) {
                std::cout << -people_.getPopulationChange() << " человек покинуло страну" << std::endl;
            } else {
                std::cout << people_.getPopulationChange() << " человек прибыло в страну" << std::endl;
            }
        };
    }
    
    void countHarvest_() {
        // подсчет урожая
        std::cout << "С " << land_.getPlantedSquare() << " квадратных миль засеянной земли вы собрали " << land_.getHarvestedSquare() << " квадратных миль урожая" << std::endl;
        
        // причины потери урожая
//        short harvesting_worse_trend = lost_farm_land - this->last_year_lost_farm_land;
//        if (lost_farm_land != 0) {
//            std::cout << "Причина - ";
//            if (harvesting_worse_trend > 2) {
//                std::cout << "возросшее ";
//            };
//            std::cout << "загрязнение воздуха и воды из-за иностранной промышленности." << std::endl;
//        }
        
        // учет дохода с урожая
        int revenue = static_cast<int>(land_.getHarvestedSquare() * account_.getPriceOfSellingLand() / 2);
        if (revenue < 0) { revenue = 0; }
        account_.add_money(revenue);
        
        if (revenue > 0) {
            std::cout << "Вы заработали на урожае " << revenue << " роллодов" << std::endl;
        } else {
            std::cout << "Урожай не принёс дохода" << std::endl;
        }
//        this->last_year_lost_farm_land = lost_farm_land;
        land_.setHarvestMutliplyingFactor(1.0);
    }
    
    void countTourists_() {
        // подсчет доходов с туристов
        
        short koef_1 = static_cast<short>(people_.getSettled() * 22 + getRandomFloatFromZeroToOne_() * 500);
        short koef_2 = static_cast<short>(land_.getSoldSquare() * 15);
        short revenue = 0;
        
        if (koef_1 > koef_2) {
            revenue = koef_1 - koef_2;
        }
        
        revenue *= people_.getTourismMultiplyingFactor();
        
        if (revenue > 0) {
            std::cout << "Вы заработали " << revenue << " роллодов на туристах" << std::endl;
        } else {
            std::cout << "Туристы не принесли дохода" << std::endl;
        }
        if (koef_2 != 0 && !(koef_1 - koef_2 >= last_year_tourists_revenue_)) {
            std::cout << "Поток туристов уменьшился, потому что ";
            short reason = get_random_choise(5, 20);
            if (reason == 1) {
                std::cout << "в озёрах стало меньше рыбы из-за загрязнения воды";
            } else if (reason == 2) {
                std::cout << "в небе стало меньше птиц из-за загрязнения воздуха";
            } else if (reason == 3) {
                std::cout << "минеральные ванны портятся из-за загрязнения воды";
            } else if (reason == 4) {
                std::cout << "из-за неприятного смога не понежиться на солнышке";
            } else {
                std::cout << "отели потеряли популярность из-за сильного смога";
            }
            std::cout << std::endl;
        }
        last_year_tourists_revenue_ = revenue;
        account_.add_money(revenue);
        people_.setTourismMultiplyingFactor(1.0);
    }
    
    void ShowCurrentState_() {
        std::cout << "\nТекущее состояние игры: " << year_ << " год правления, " << account_.getBalance() << " роллодов в казне, " << people_.getCountrymen() << " жителей" << std::endl;
        GameResult resut = GameResult{
            year_,
            account_.getBalance(),
            people_.getCountrymen()};
        std::hash<GameResult> hashFunction;

        auto year_hash = hashFunction(resut);
        std::cout << "Текст для копирования на сайт:\n***\n" << year_hash << "," << year_ << "," << account_.getBalance() << "," << people_.getCountrymen() << "\n***" << std::endl;
    }
    
    void checkIfGameOver_() {
        // вычислить результаты года
        
        // если слишком много погибших
        if (this->died_count_ > 200) {
            std::cout << this->died_count_ << MESSAGE_ABOUT_DIES;
            short reason = get_random_choise(3, 33);
            if (reason == 1) {
                std::cout << MESSAGE_ABOUT_DIES_END_1;
            } else if (reason == 2) {
                std::cout << MESSAGE_ABOUT_DIES_END_2;
            } else {
                std::cout << MESSAGE_ABOUT_DIES_END_3;
            }
            std::cout << std::endl;
            throw GameOver();
        }
        
        // если слишком мало жителей
        if (people_.getCountrymen() < 343) {
            std::cout << GAME_OVER_BECAUSE_DIES << std::endl;
            throw GameOver();
        }
        
        // если слишком много денег осталось в казне
        if (account_.getBalance() / 100 > 5 && this->died_count_ - this->died_because_of_pollution_ >= 2) {
            std::cout << GAME_OVER_BECAUSE_BALANCE << std::endl;
            throw GameOver();
        }
        
        // если слишком много иностранцев
        if (people_.getForeigners() > people_.getCountrymen()) {
            std::cout << GAME_OVER_BECAUSE_FOREINERS << std::endl;
            throw GameOver();
        }
        
        // победа в игре
        if (year_ == 7) {
            std::cout << GAME_OVER_SUCCESS;
            throw GameOver();
        }
    }
    
    short year_ = 0;
    Land land_;
    People people_;
    Account account_;
    NextYearParams next_year_params_;
    int died_count_ = 0;    // в другой класс?
    int died_because_of_pollution_ = 0;
    int last_year_tourists_revenue_ = 0;
};
