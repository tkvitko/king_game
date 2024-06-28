//
//  Game.cpp
//  King
//
//  Created by tkvitko on 22.05.2024.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include "Game.hpp"


NextYearParams::NextYearParams() {};
int NextYearParams::getCountrymanChange() { return countrymen_change_; }

void NextYearParams::increaseCountrymanChange(int count) {
    countrymen_change_ += count;
}

void NextYearParams::resetCountrymanChange() {
    countrymen_change_ = 0;
}


    
Game::Game(bool custom_game, bool random_events_enabled, bool competition) {
    // инициализация новой игры
    
    custom_game_ = custom_game;
    random_events_enabled_ = random_events_enabled;
    competition_ = competition;
    
    if (competition) {
        land_ = Land(1000, 1000);
        people_ = People(START_COUNTRYMAN_COMPETITION);
        account_ = Account(START_BALANCE_COMPETITION);
        
    } else if (!custom_game_) {
        land_ = Land(1000, 1000);
        people_ = People(get_random_short_from_range(START_COUNTRYMAN_MIN, START_COUNTRYMAN_MAX));
        account_ = Account(get_random_short_from_range(START_BALANCE_MIN, START_BALANCE_MAX));
        
    } else {
        // задать пользователю вопросы по старту игры и запустить игру
        
        std::cout << "С какого года правления вы хотите начать игру? ";
        int years = get_valid_integer_input(0, 8, false);
        std::cout << "Сколько у вас в казне? ";
        int balance = get_valid_integer_input(0, 500000, false);
        std::cout << "Сколько жителей? ";
        int countrymen = get_valid_integer_input(1, 2000, false);
        std::cout << "Сколько иностранных рабочих? ";
        int foreigners = get_valid_integer_input(0, 2000, false);
        std::cout << "Сколько квадратных миль сельхоз земли? ";
        int farm_land = get_valid_integer_input(0, 2000, false);
        std::cout << "Сколько квадратных миль леса? ";
        int forest_land = get_valid_integer_input(0, 2000, false);
        
        land_ = Land(farm_land, forest_land);
        people_ = People(countrymen);
        account_ = Account(balance);
        year_ = years - 1;
        people_.setForeigners(foreigners);
    }
    
};

void Game::showStateOnYearStart() {
    // вывести состояние игры
    
    std::cout << "\n" << year_ << " год правления" << std::endl;
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

void Game::getYearDecisionsFromUser() {
    // задать пользователю вопросы и применить ответы
    
    this->sellLandToIndustry_();
    this->distributeMoneyToCountryman_();
    this->plantFarmLand_();
    this->spentMoneyToPollutionControl_();
    this->cutDownForest_();
}

void Game::processYear() {
    // провести новый год
    
    year_ += 1;
    // установка новых цен
    account_.setPrices(get_random_short_from_range(PRICE_OF_SELLING_LAND_MIN, PRICE_OF_SELLING_LAND_MAX),
                       get_random_short_from_range(PRICE_OF_PLANTING_LAND_MIN, PRICE_OF_PLANTING_LAND_MAX),
                       PRICE_OF_CUTTING_DOWN_FOREST,
                       PRICE_OF_LIVING);
    
    // статус и получение ответов игрока
    showStateOnYearStart();
    getYearDecisionsFromUser();
    
    // случайные события
    if (random_events_enabled_) { process_random_events_(); }
    
    // рассчеты по итогам решений игрока и случайных событий
    countDeaths_();
    countPeople_();
    countHarvest_();
    countTourists_();
    
    // проверка, не окончена ли игра
    if (competition_) { ShowStateOnYearEnd(); }
    checkIfGameOver_();
    
    // применить изменения на следующий год
    people_.increase(next_year_params_.getCountrymanChange());
    next_year_params_.resetCountrymanChange();
}

void Game::sellLandToIndustry_() {
    // процесс продажи земли иностранной промышленности
    std::cout << "Сколько квадратных миль земли вы хотите продать под промышленность? ";
    int square = get_valid_integer_input(0, land_.getFarmSquare(), true);
    land_.sellFarmToIndustry(square);       // изменения земель
    account_.sellFarmToIndustry(square);    // изменения бюджета
}

void Game::distributeMoneyToCountryman_() {
    // процесс распределения денег по жителям
    
    if (account_.getBalance() > 0) {
        std::cout << "Сколько роллодов вы отдадите своим жителям? ";
        int amount = get_valid_integer_input(0, account_.getBalance(), true);
        bool success = account_.spendMoneyToCountryman(amount);   // изменения бюджета
        if (!success) {
            std::cout << "Не хватает денег в казне";
        }
    } else {
        std::cout << "В казне не осталось денег для распределения жителям" << std::endl;
    }
}

void Game::plantFarmLand_() {
    // процесс засева сельхоз земель
    
    if (land_.getFarmSquare() == 0) {
        std::cout << "На острове не осталось сельскохозяйственной земли" << std::endl;
    } else if (account_.getBalance() / account_.getPriceOfPlantingLand() == 0) {
        std::cout << "В казне не осталось денег на то, чтобы засеять землю" << std::endl;
    } else {

        std::vector<int> squares = {land_.getFarmSquare(),
            people_.getCountrymen() * people_.getSquareCountrymanCanPlant(),
            account_.getBalance() / account_.getPriceOfPlantingLand()};
        std::vector<int>::iterator result;
        result = std::min_element(squares.begin(), squares.end());
        
        int max_square_to_plant = *result;
        if (max_square_to_plant > 0) {
            std::cout << "Сколько квадратных миль земли вы хотите засеять? ";
            int square = get_valid_integer_input(0, max_square_to_plant, true);
            bool success = account_.plantFarmLand(square);  // изменения бюджета
            if (!success) {
                std::cout << "Не хватает денег в казне";
            } else {
                land_.plantFarmLand(square);     // вспашка и сбор урожая
            }
        }
    }
}

void Game::spentMoneyToPollutionControl_() {
    // процесс вкладывания денег в контроль загрязнений
    
    if (account_.getBalance() > 0) {
        std::cout << "Сколько роллодов вы хотите потратить на контроль загрязнений? ";
        int amount = get_valid_integer_input(0, account_.getBalance(), true);
        bool success = account_.spendMoneyToPollutionControl(amount);
        if (!success) {
            std::cout << "Не хватает денег в казне";
        }
    } else {
        std::cout << "В казне не осталось денег для контроля загрязнений" << std::endl;
    }
}

void Game::cutDownForest_() {
    // процесс вырубки леса
    
    if (land_.getForestSquare() > 0 && account_.getMaxCuttingDownSquareOfForest() > 0) {
        
        if (account_.getMaxCuttingDownSquareOfForest() > 0) {
            std::cout << "Сколько квадратных миль леса вы хотите вырубить? ";
            int square = get_valid_integer_input(0,
                                                 std::min(land_.getForestSquare(), account_.getMaxCuttingDownSquareOfForest()),
                                                 true);
            
            if (square != 0) {
                bool success = account_.cutDownForest(square);
                if (!success) {
                    std::cout << "Не хватает денег в казне";
                } else {
                    land_.cutDownForest(square);
                    short deaths_when_cutting = get_random_choise(2, 50);
                    if (deaths_when_cutting == 1) {
                        int dead_count = people_.getCountrymen() * 0.1;
                        people_.decrease(dead_count);
                        std::cout << "Случилось несчастье при рубке! " << dead_count << " жителей завалило деревьями." << std::endl;
                    }
                }
            }
        } else {
            std::cout << "В казне не хватает денег на вырубку леса" << std::endl;
        }
    }
}

void Game::countDeaths_() {
    
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

void Game::countPeople_() {
    
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

void Game::countHarvest_() {
    // подсчет урожая
    int harvesting_worse_trend = land_.harvest();
    std::cout << "С " << land_.getPlantedSquare() << " квадратных миль засеянной земли вы собрали " << land_.getHarvestedSquare() << " квадратных миль урожая" << std::endl;
    
    // причины потери урожая
    if (harvesting_worse_trend != 0) {
        std::cout << "Причина потери урожая - ";
        if (harvesting_worse_trend > 2) {
            std::cout << "возросшее ";
        };
        std::cout << "загрязнение воздуха и воды из-за иностранной промышленности." << std::endl;
    }
    
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
    land_.resetHarvestMutliplyingFactor();
}

void Game::countTourists_() {
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
    if (koef_2 != 0 && !(koef_1 - koef_2 >= last_year_tourists_revenue_) && land_.getSoldSquare() > 0) {
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
    people_.resetTourismMultiplyingFactor();
}

void Game::ShowStateOnYearEnd() {
    std::cout << "\nТекущее состояние игры: " << year_ << " год правления, " << account_.getBalance() << " роллодов в казне, " << people_.getCountrymen() << " жителей, " << people_.getForeigners() << " иностранных рабочих" << std::endl;
    
    GameResult resut = GameResult{
        year_,
        account_.getBalance(),
        people_.getCountrymen()};
    std::hash<GameResult> hashFunction;

    auto year_hash = hashFunction(resut);
    std::cout << "Текст для копирования на сайт:\n***\n" << year_hash << "," << year_ << "," << account_.getBalance() << "," << people_.getCountrymen() << "\n***" << std::endl;
}

void Game::checkIfGameOver_() {
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
    if (people_.getCountrymen() < people_.getStartCountrymen() / 3) {
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

void Game::process_random_events_() {
    // ежемесячные случайные события
        YearEvents year_events = YearEvents();
        for (size_t i = 0; i < 12; ++i) {
            std::cout << "...идёт месяц " << i + 1 << std::endl;
            monthType month_type = year_events.monthes[i];
            
            switch(month_type) {
                case monthType::GOOD:{
                    Event event = events.popLastEvent(EventType::good);
                    processEvent_(event);}
                    break;
                case monthType::BAD:{
                    Event event = events.popLastEvent(EventType::bad);
                    processEvent_(event);}
                    break;
                case monthType::QUIZ:{
                    Quiz quiz = quizes.pop_last_quiz();
                    playQuiz_(quiz);}
                    break;
                case monthType::EMPTY:
                    break;
            }
        }

}

void Game::processEvent_(Event event) {
    std::cout << event.text;
    std::cout << "\n\n" << std::endl;
    std::chrono::milliseconds timespan(2000);
    std::this_thread::sleep_for(timespan);
    
    account_.add_money(event.change_balance);
    account_.add_money(event.change_balance_by_koef_per_countryman * people_.getCountrymen());
    people_.increase(event.change_countryman);
    people_.increase(event.change_countryman_by_koef_per_countrymen * people_.getCountrymen() / 100);
    next_year_params_.increaseCountrymanChange(event.change_countryman_next_year);
    
    people_.increaseTourismMultiplyingFactor(static_cast<double>(event.change_tourism_percentage) / 100);
    land_.increaseHarvestMutliplyingFactor(static_cast<double>(event.change_harvest_percentage) / 100);
    account_.increasePriceOfLivingMultiplyingFactor(static_cast<double>(event.change_cost_of_life_percentage) / 100);

//        std::cout << "После этого события:" << "\nКазна: " << account_.getBalance() << "\nНаселение: " << people_.getCountrymen() << "\nКоэффициент туризма в этом году: " << people_.getTourismMultiplyingFactor() << "\nКоэффициент урожая в этом году: " << land_.getHarvestMultiplyingFactor() << "\nКоэффициент стоимости жизни в следующем году: " << account_.getPriceOfLivingMultiplyingFactor() << "\nИзменение населения в следующем году: " << next_year_params_.getCountrymanChange() << std::endl;
    
}

void Game::playQuiz_(Quiz quize) {
    std::cout << quize.question << std::endl;
    int decision = get_valid_integer_input(1, 2, false);
    if (decision == 1) {
        std::cout << quize.neutral_answer;
    } else {
        short event_type = get_random_choise(2, 50);
        if (event_type == 1) {
            this->processEvent_(quize.good_event);
        } else {
            this->processEvent_(quize.bad_event);
        }
        event_type = 0;
    }
}
