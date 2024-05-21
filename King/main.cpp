//
//  main.cpp
//  King
//
//  Created by tkvitko on 14.04.2024.
//

#include <iostream>
#include <fstream>
#include <random>
#include <functional>
#include "config_file.h"
#include "utils.hpp"
#include "strings.hpp"
#include "constants.h"
#include "events.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif


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

class GameState {
private:
    
    // константы
    int cost_of_living = 100;
    int cost_of_funeral = 9;
    int pollution_control_factor = 25;
    int square_countryman_can_plant = 2;
    int cost_of_planting_land_min = 10;
    int cost_of_planting_land_max = 15;
    int price_of_selling_land_min = 95;
    int price_of_selling_land_max = 105;
    int start_balance_min = 59000;
    int start_balance_max = 61000;
    int start_contrymen_min = 490;
    int start_contrymen_max = 510;
    int price_for_cutting_down_forest = 1;
    
    // режим
    bool custom_game = false;    // обычный режим игры без ввода стартовых данных пользователем
    bool random_events_enabled = false; // игра без случайных ивентов
    
    // время
    unsigned short years = 0;  // лет правления
    
    // финансы
    int balance = 0;  // количество денег в казне
    
    // люди
    int countrymen = 0;     // количество жителей
    int foreigners = 0;     // количество иностранных рабочих
    
    // земли
    int farm_land = 1000;   // площадь сельхоз земли
    int forest_land = 1000; // площадь леса

    // параметры текущего года
    int died_count = 0;                  // общее количество погибших жителей
    int died_because_of_pollution = 0;   // количество погибших жителей по причине загрязнения
    int population_change = 0;           // изменение населения
    
    // экономика текущего года
    int cost_of_planting_land = 0;       // стоимость засева земли
    int price_of_selling_land = 0;       // цена продажи земли
    int imcome_from_tourism = 0;         // доход от туризма
    double tourism_multiplying_factor = 1.0;  // повышающий коээфициент для дохода с туризма
    double harvest_multiplying_factor = 1.0;  // повышающий коээфициент для дохода с урожая
    
    // решения текущего года
    int sold_square = 0;
    int distributed_money = 0;
    int planted_square = 0;
    int money_spent_for_pollution_control = 0;
    
    // экономика предыдущего года
    short last_year_lost_farm_land = 0;     // потери урожая
    int last_year_tourists_revenue = 0;   // заработок на туристах
    
    // экономика следующего года
    int next_year_countrymen = 0;   // изменение населения в следующем году
    double next_year_cost_of_life_multiplying_factor = 1.0;  // изменение стоимости жизни в следующем году
    
    Events events = Events();
    Quizes quizes = Quizes();
    
    short getSettled_() {
        return this->countrymen - this->population_change; // оседлые жители
    }
    
    void readConfig_() {
        // чтение конфигурации
        std::vector<std::string> ln = {
            "farm_land",
            "forest_land",
            "cost_of_living",
            "cost_of_funeral",
            "pollution_control_factor",
            "square_countryman_can_plant",
            "cost_of_planting_land_min",
            "cost_of_planting_land_max",
            "price_of_selling_land_min",
            "price_of_selling_land_max"
        };
        
        std::ifstream f_in;
//        f_in.open("config.txt");
        f_in.open("/Users/tkvitko/PycharmProjects/basic-computer-games/53_King/cpp/King/King/config.txt");
        if(! f_in) {
        } else {
            CFG::ReadFile(f_in, ln,
                          this->farm_land,
                          this->forest_land,
                          this->cost_of_living,
                          this->cost_of_funeral,
                          this->pollution_control_factor,
                          this->square_countryman_can_plant,
                          this->cost_of_planting_land_min,
                          this->cost_of_planting_land_max,
                          this->price_of_selling_land_min,
                          this->price_of_selling_land_max);
            f_in.close();
        }
    }
    
    double getRandomFloatFromZeroToOne_() {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator);
    }
    
    long addMoney_(int amount) {return this->balance += amount; }
    long spendMoney_(const int amount) {
        if (amount <= this->balance) {
            return this->balance -= amount;
        } return -1;
    }
    
    void setPricesForLand_() {
        // установка цен на продажу земли и посев на земле
        this->cost_of_planting_land = get_random_short_from_range(cost_of_planting_land_min, cost_of_planting_land_max);
        this->price_of_selling_land = get_random_short_from_range(price_of_selling_land_min, price_of_selling_land_max);
    }

    void sellLandToIndustry_() {
        // процесс продажи земли иностранной промышленности
        short square_to_sell = 0;
        std::cout << "Сколько квадратных миль земли вы хотите продать под промышленность? ";
        square_to_sell = get_valid_integer_input(0, this->farm_land, true);
        
        this->farm_land -= square_to_sell;
        this->addMoney_(this->price_of_selling_land * square_to_sell);
        this->sold_square = square_to_sell;
    }
    
    void distributeMoneyToCountryman_() {
        // процесс распределения денег по жителям
        int money_to_distribute = 0;
        std::cout << "Сколько роллодов вы отдадите своим жителям? ";
        money_to_distribute = get_valid_integer_input(0, this->balance, true);
        
        this->spendMoney_(money_to_distribute);
        this->distributed_money = money_to_distribute;
    }
    
    void plantFarmLand_() {
        // процесс засева сельхоз земель
        short square_to_plant = 0;
        while (true) {
            std::cout << "Сколько квадратных миль земли вы хотите засеять? ";
            square_to_plant = get_valid_integer_input(0, this->farm_land, true);
            if (square_to_plant > this->farm_land) {
                std::cout << "Увы, у вас есть только " << this->farm_land << " квадратных миль сельскохозяйственных земель" << std::endl;
            } else if (square_to_plant > this->countrymen * square_countryman_can_plant) {
                std::cout << "Увы, каждый житель может засеять только " << square_countryman_can_plant << " квадратные мили" << std::endl;
            } else if (square_to_plant * this->cost_of_planting_land > this->balance) {
                std::cout << "Подумайте ещё раз. У вас осталось лишь " << this->balance << " роллодов в казне" << std::endl;
            } else {
                break;
            };
        }
        this->spendMoney_(square_to_plant * this->cost_of_planting_land);
        this->planted_square = square_to_plant;
    }
    
    void spentMoneyToPollutionControl_() {
        // процесс вкладывания денег в контроль загрязнений
        int money_for_pollution_control = 0;
        std::cout << "Сколько роллодов вы хотите потратить на контроль загрязнений? ";
        money_for_pollution_control = get_valid_integer_input(0, this->balance, true);
        this->spendMoney_(money_for_pollution_control);
        this->money_spent_for_pollution_control = money_for_pollution_control;
    }
    
    void cutDownForest_() {
        // процесс вырубки леса
        int cut_down_square = 0;
        std::cout << "Сколько квадратных миль леса вы хотите вырубить? ";
        cut_down_square = get_valid_integer_input(0, this->forest_land, true);
        
        if (cut_down_square != 0) {
            this->forest_land -= cut_down_square;
            this->farm_land += cut_down_square;
            this->spendMoney_(price_for_cutting_down_forest * cut_down_square);
            short deaths_when_cutting = get_random_choise(2, 50);
            if (deaths_when_cutting == 1) {
                int dead_count = this->countrymen * 0.1;
                this->countrymen -= dead_count;
                std::cout << "Случилось несчастье при рубке! " << dead_count << " жителей завалило деревьями." << std::endl;
            }
        }
    }
    
    void countDeaths_() {
        // подсчет погибших
        
        // погибшие от голода
        short starved = this->countrymen - this->distributed_money / this->cost_of_living;
        if (starved > 0) {
            std::cout << starved << " жителей умерло от голода" << std::endl;
        } else {
            // если жиетелей по какой-то причине стало меньше, чем было в начале года, starved будет отрицательным
            starved = 0;
        };
        
        // погибшие от загрязнений
        // базовое значение - процент от площади земли, проданной под промышленность
        this->died_because_of_pollution = static_cast<int>(this->getRandomFloatFromZeroToOne_() * (this->sold_square));
        // уменьшение базового значения, если на контроль загрязнений было выделено больше порога (фактора)
        if (this->money_spent_for_pollution_control >= this->pollution_control_factor) {
            this->died_because_of_pollution = static_cast<int>(this->died_because_of_pollution / (this->money_spent_for_pollution_control / this->pollution_control_factor));
        }
        
        if (this->died_because_of_pollution > this->countrymen - starved) {
            this->died_because_of_pollution = this->countrymen - starved;
        }
        if (this->died_because_of_pollution > 0) {
            std::cout << this->died_because_of_pollution << " жителей погибло из-за углеродного и пылевого загрязнения" << std::endl;
        }
            
        // затраты на похороны
        this->died_count = starved + this->died_because_of_pollution;
        if (this->died_count > 0) {
            long funeral_cost = this->died_count * this->cost_of_funeral;
            std::cout << "Пришлось потратить " << funeral_cost << " роллодов на похороны" << std::endl;
            this->balance -= funeral_cost;  // вычитание из казны
            if (this->balance < 0) {
                std::cout << "В казне не хватает денег на похороны, придётся продать часть земли" << std::endl;
//                std::cout << "farm land " << this->farm_land << " balance " << this->balance << " price_of_selling_land " << this->price_of_selling_land << " division " << this->balance / this->price_of_selling_land << std::endl;
                // компенсация отрицательного баланса за счет продажи земли
                // подозрительный момент, т.к. по сути земля продается промшленности, но эта продажа уже нигде не учитывается в вопросах, связанных с промышленностью
                this->farm_land += this->balance / this->price_of_selling_land;
                this->balance = 0;
            }
        }
        this->countrymen -= this->died_count;
    }
    
    void countPeople_() {
        // подсчет изменения населения
        
        // инорстранные рабочие
        short foreigners_change = 0;
        if (this->sold_square != 0) {
            foreigners_change = static_cast<int>(this->sold_square + this->getRandomFloatFromZeroToOne_() * 10 - this->getRandomFloatFromZeroToOne_() * 20);
            if (this->foreigners < 0) {
                foreigners_change += 20;
            }
            std::cout << foreigners_change << " иностранных рабочих приехало в страну"  << std::endl;
        }
        
        // новые жители
        long over_disctibuted_money = this->distributed_money / this->cost_of_living - this->countrymen;
        short countrymen_change = static_cast<int>(over_disctibuted_money / 10
                                                   + this->money_spent_for_pollution_control / this->pollution_control_factor
                                                   - ((this->sold_square) / 50 )
                                                   - this->died_count / 2 );
        
        if (countrymen_change != 0) {
            std::cout << "Население изменилось: ";
            if (countrymen_change < 0) {
                std::cout << -countrymen_change << " человек покинуло страну" << std::endl;
            } else {
                std::cout << countrymen_change << " человек прибыло в страну" << std::endl;
            }
            this->countrymen += countrymen_change;
        };
        this->foreigners += foreigners_change;
    }
    
    void countHarvest_() {
        // подсчет урожая
        
        // потери урожая из-за загрязнений
        short lost_farm_land = static_cast<int>((this->sold_square) * (this->getRandomFloatFromZeroToOne_() + 1.5) / 2);
        if (lost_farm_land > this->planted_square) {
            lost_farm_land = this->planted_square;
        }
        short harvested = this->planted_square - lost_farm_land;
        harvested *= this->harvest_multiplying_factor;
        
        std::cout << "С " << this->planted_square << " квадратных миль засеянной земли вы собрали " << harvested << " квадратных миль урожая" << std::endl;
        
        // причины потери урожая
        short harvesting_worse_trend = lost_farm_land - this->last_year_lost_farm_land;
        if (lost_farm_land != 0) {
            std::cout << "Причина - ";
            if (harvesting_worse_trend > 2) {
                std::cout << "возросшее ";
            };
            std::cout << "загрязнение воздуха и воды из-за иностранной промышленности." << std::endl;
        }
        
        // учет дохода с урожая
        int revenue = static_cast<int>(harvested * this->price_of_selling_land / 2);
        if (revenue < 0) { revenue = 0; }
        this->balance += revenue;
        
        if (revenue > 0) {
            std::cout << "Вы заработали на урожае " << revenue << " роллодов" << std::endl;
        } else {
            std::cout << "Урожай не принёс дохода" << std::endl;
        }
        this->last_year_lost_farm_land = lost_farm_land;
        this->harvest_multiplying_factor = 1.0;
    }
    
    void countTourists_() {
        // подсчет доходов с туристов
        
        short koef_1 = static_cast<short>(this->getSettled_() * 22 + this->getRandomFloatFromZeroToOne_() * 500);
        short koef_2 = static_cast<short>(( this->sold_square) * 15);
        short revenue = 0;
        
        if (koef_1 > koef_2) {
            revenue = koef_1 - koef_2;
        }
        
        revenue *= this->tourism_multiplying_factor;
        
        if (revenue > 0) {
            std::cout << "Вы заработали " << revenue << " роллодов на туристах" << std::endl;
        } else {
            std::cout << "Туристы не принесли дохода" << std::endl;
        }
        if (koef_2 != 0 && !(koef_1 - koef_2 >= this->last_year_tourists_revenue)) {
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
        this->last_year_tourists_revenue = abs(koef_1 - koef_2);
        this->balance += revenue;
        this->tourism_multiplying_factor = 1.0;
    }
    
    void processEvent_(Event event) {
        std::cout << event.text;
//        std::cin.get();
        this->balance += event.change_balance;
        this->balance += event.change_balance_by_koef_per_countryman * this->countrymen;
        this->countrymen += event.change_countryman;
        this->countrymen += event.change_countryman_by_koef_per_countrymen * this->countrymen / 100;
        this->next_year_countrymen += event.change_countryman_next_year;
        
        this->tourism_multiplying_factor += static_cast<double>(event.change_tourism_percentage) / 100;
        this->harvest_multiplying_factor += static_cast<double>(event.change_harvest_percentage) / 100;
        this->next_year_cost_of_life_multiplying_factor += static_cast<double>(event.change_cost_of_life_percentage) / 100;
        
        std::cout << "После этого события:" << "\nКазна: " << this->balance << "\nНаселение: " << this->countrymen << "\nКоэффициент туризма в следующем году: " << this->tourism_multiplying_factor << "\nКоэффициент урожая в следующем году: " << this->harvest_multiplying_factor << "\nКоэффициент стоимости жизни в следующем году: " << this->next_year_cost_of_life_multiplying_factor << "\nИзменение населения в следующем году: " << this->next_year_countrymen << std::endl;
        
    }
    
    void playQuiz_(Quiz quize) {
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
    
//    void _process_good_random_event() {
//        Event event = events.get_random_event(EventType::good);
//        this->_process_event(event);
//    }
    
//    void _process_bad_random_event() {
//        Event event = events.get_random_event(EventType::bad);
//        this->_process_event(event);
//    }
    
//    void _process_quiz() {
//        Quiz quiz = this->quizes.get_random_quiz();
//        this->_play_quiz(quiz);
//    }
    
public:
    
    GameState() {
        this->readConfig_();
    }
    
    void printHeader() {
        // вывести приветствие
        
        std::cout << "\n### СРОЧНО НУЖЕН ПРЕМЬЕР-МИНИСТР! ###\n" << std::endl;
        std::cout << "(на основе игры The King, опубликованной в Basic Computer Games в 1978)" << std::endl;
        std::cout << "Author: @taraskvitko" << std::endl;
        std::cout << "Powered by Dialas" << std::endl;
        std::cout << "Version 1.6.5\n\n\n" << std::endl;
    }
    
    void printIntro() {
        // вывести интро
        std::cout << INTRO_PART_1 << this->cost_of_living << INTRO_PART_2 << std::endl;
    }
    
    void printState() {
        // вывести состояние игры
        
        std::cout << this->years << " год правления" << std::endl;
        std::cout << "В казне " << this->balance << " ролодов" << std::endl;
        std::cout << "В стране проживает " << this->countrymen << " жителей";
        if (this->foreigners > 0) {
            std::cout << " и " << this->foreigners << " иностранных рабочих" << std::endl;
        } else {
            std::cout << std::endl;
        };
        std::cout << "У нас " << this->farm_land + this->forest_land << " квадратных миль земли: " << this->farm_land << " сельхоз и " << this->forest_land << " леса" << std::endl;
        std::cout << "В этом году промышленность готова платить " << this->price_of_selling_land << " роллодов за квадратную милю, а стоимость засева одной квадратной мили равна " << this->cost_of_planting_land << std::endl ;
    }
    
    void initNewYear() {
        // начать новый год правления
        
        this->years += 1;
        if (this->years == 1 && !this->custom_game) {
            this->balance = get_random_short_from_range(start_balance_min, start_balance_max);
            this->countrymen = get_random_short_from_range(start_contrymen_min, start_contrymen_max);
        };
        this->setPricesForLand_();
        this->died_count = 0;
    }
    
    void enableRandomEvents() {
        this->random_events_enabled = true;
    }
    
    void initSpeciphicYear(short years, int balance, int countrymen, int foreigners, int farm_land, int forest_land) {
        // начать год правления с конкретными вводными
        
        this->custom_game = true;
        this->years = years;
        this->balance = balance;
        this->countrymen = countrymen;
        this->foreigners = foreigners;
        this->farm_land = farm_land;
        this->forest_land = forest_land;
    }
    
    void getResumeDataFromUser() {
        // задать пользователю вопросы по старту игры и запустить игру
        
        int max_user_value = 500000;
        
        std::cout << "С какого года правления вы хотите начать игру? ";
        int years = get_valid_integer_input(0, 8, false);
        std::cout << "Сколько у вас в казне? ";
        int balance = get_valid_integer_input(0, max_user_value, false);
        std::cout << "Сколько жителей? ";
        int countrymen = get_valid_integer_input(1, max_user_value, false);
        std::cout << "Сколько иностранных рабочих? ";
        int foreigners = get_valid_integer_input(0, max_user_value, false);
        std::cout << "Сколько квадратных миль сельхоз земли? ";
        int farm_land = get_valid_integer_input(1, max_user_value, false);
        std::cout << "Сколько квадратных миль леса? ";
        int forest_land = get_valid_integer_input(1, max_user_value, false);
        
        initSpeciphicYear(years, balance, countrymen, foreigners, farm_land, forest_land);
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
        
        // ежемесячные случайные события
        if (this->random_events_enabled) {
            YearEvents year_events = YearEvents();
            for (size_t i = 0; i < 12; ++i) {
                std::cout << "...идёт месяц " << i + 1 << std::endl;
//                game.process_random_event();
                monthType month_type = year_events.monthes[i];
                
                switch(month_type) {
                    case monthType::GOOD:{
                        Event event = this->events.popLastEvent(EventType::good);
                        this->processEvent_(event);}
                        break;
                    case monthType::BAD:{
                        Event event = this->events.popLastEvent(EventType::bad);
                        this->processEvent_(event);}
                        break;
                    case monthType::QUIZ:{
                        Quiz quiz = this->quizes.pop_last_quiz();
                        this->playQuiz_(quiz);}
                        break;
                    case monthType::EMPTY:
                        break;
                }
            }
        }
        
        // произвести вычисления
        
        this->countDeaths_();
        this->countPeople_();
        this->countHarvest_();
        this->countTourists_();
        
        // применить изменение населения на следующий год
        this->countrymen += this->next_year_countrymen;
        this->next_year_countrymen = 0;
        
        this->cost_of_living *= this->next_year_cost_of_life_multiplying_factor;
        this->next_year_cost_of_life_multiplying_factor = 1.0;
    }
    
    bool countYearResults() {
        // вычислить результаты года
        
        std::cout << "\nТекущее состояние игры: " << this->years << " год правления, " << this->balance << " роллодов в казне, " << this->countrymen << " жителей" << std::endl;
        GameResult resut = GameResult{
            this->years,
            this->balance,
            this->countrymen};
        std::hash<GameResult> hashFunction;

        auto year_hash = hashFunction(resut);
        std::cout << "Хеш вашего результата:" << std::endl;
        std::cout << year_hash << "\n" << std::endl;
        
        // если не хватило земли по итогам продажи для компенсации затрат на похороны:
        if (this->farm_land < 0) {
            std::cout << "Закончилась сельхоз земля" << std::endl;
            return true;
        }
        
        // если слишком много погибших
        if (this->died_count > 200) {
            std::cout << this->died_count << MESSAGE_ABOUT_DIES;
            short reason = get_random_choise(3, 33);
            if (reason == 1) {
                std::cout << MESSAGE_ABOUT_DIES_END_1;
            } else if (reason == 2) {
                std::cout << MESSAGE_ABOUT_DIES_END_2;
            } else {
                std::cout << MESSAGE_ABOUT_DIES_END_3;
            }
            std::cout << std::endl;
            return true;
        }
        
        // если слишком мало жителей
        if (this->countrymen < 343) {
            std::cout << GAME_OVER_BECAUSE_DIES << std::endl;
            return true;
        }
        
        // если слишком много денег осталось в казне
        if (this->balance / 100 > 5 && this->died_count - this->died_because_of_pollution >= 2) {
            std::cout << GAME_OVER_BECAUSE_BALANCE << std::endl;
            return true;
        }
        
        // если слишком много иностранцев
        if (this->foreigners > this->countrymen) {
            std::cout << GAME_OVER_BECAUSE_FOREINERS << std::endl;
            return true;
        }
        
        // победа в игре
        if (this->years == 7) {
            std::cout << GAME_OVER_SUCCESS;
            return true;
        }

        return false;
    }
    
//    void process_random_event() {
//        short event_type = get_random_choise(3, 10);
////        std::cout << "DEBUG random event " << event_type << std::endl;
//        // 1 - позитивное, 2 - негативное, 3 - викторина
//        if (event_type == 1) {
//            this->_process_good_random_event();
//        } else if (event_type == 2) {
//            this->_process_bad_random_event();
//        } else if (event_type == 3) {
//            this->_process_quiz();
//        }
//        event_type = 0;
//    }
};

int main(int argc, const char * argv[]) {
    
    #ifdef _WIN32
     SetConsoleOutputCP(CP_UTF8);
    #else
     setlocale(LC_CTYPE, "rus");
    #endif

    GameState game = GameState();
    game.printHeader();

    // показывать ли инструкцию
    std::cout << QUESTION_ABOUT_INTRO;
    int choise_intro = get_valid_integer_input(1, 2, false);
    if (choise_intro == 1) {
        game.printIntro();
    }

    // игра с нуля или вводим стартовые значения
    std::cout << QUESTION_ABOUT_GAME_MODE;
    int choise_mode = get_valid_integer_input(1, 2, false);
    if (choise_mode == 2) {
        game.getResumeDataFromUser();
    }

    // игра со случайными событиями или без
    std::cout << QUESTION_ABOUT_RANDOM_EVENTS;
    int choise_random = get_valid_integer_input(1, 2, false);
    if (choise_random == 1) {
        game.enableRandomEvents();
    }

    while (true) {
        game.initNewYear();
        game.printState();
        game.getYearDecisionsFromUser();

        game.processYear();
        bool is_game_over = game.countYearResults();
//        game.print_state();
        if (is_game_over) {
            std::cout << "Нажмите любоую клавишу, чтобы закрыть окно." << std::endl;
            std::cin.get();
            return 0;
        }
    }
}
