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
    
    // решения текущего года
    int sold_square = 0;
    int distributed_money = 0;
    int planted_square = 0;
    int money_spent_for_pollution_control = 0;
    
    // экономика предыдущего года
    short last_year_lost_farm_land = 0;     // потери урожая
    int last_year_tourists_revenue = 0;   // заработок на туристах
    
    short _get_settled() {
        return this->countrymen - this->population_change; // оседлые жители
    }
    
    void read_config() {
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
    
    double _get_random_float_from_zero_to_one() {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(generator);
    }
    
    int _request_int_value(const std::string question_text) {
        // задает вопрос и считывает числовой ответ; возвращает его или -1, если ответ не числовой
        int answer = -1;
        std::cout << question_text << " ";
        std::cin >> answer;
        return answer;
    }
    
    long _add_money(int amount) {return this->balance += amount; }
    long _spend_money(const int amount) {
        if (amount <= this->balance) {
            return this->balance -= amount;
        } return -1;
    }
    
    void _set_prices_for_land() {
        // установка цен на продажу земли и посев на земле
        this->cost_of_planting_land = get_random_short_from_range(cost_of_planting_land_min, cost_of_planting_land_max);
        this->price_of_selling_land = get_random_short_from_range(price_of_selling_land_min, price_of_selling_land_max);
    }

    void _sell_land_to_industry() {
        // процесс продажи земли иностранной промышленности
        short square_to_sell = 0;
        while (true) {
            square_to_sell = this->_request_int_value("Сколько квадратных миль земли вы хотите продать под промышленность? ");
            if (square_to_sell < this->farm_land) {
                break;
            } else {
                std::cout << "Подумайте ещё раз. У вас есть всего " << this->farm_land << " квадратных миль сельскохозяйственной земли." << std::endl;
            }
        };
        this->_add_money(this->price_of_selling_land * square_to_sell);
        this->sold_square = square_to_sell;
    }
    
    void _distribute_money_to_countryman() {
        // процесс распределения денег по жителям
        int money_to_distribute = 0;
        while (true) {
            money_to_distribute = this->_request_int_value("Сколько роллодов вы отдадите своим жителям? ");
            if (money_to_distribute < this->balance) {
                break;
            } else {
                std::cout << "Подумайте ещё раз. У вас есть всего " << this->balance << " роллодов в казне" << std::endl;
            }
        };
        this->_spend_money(money_to_distribute);
        this->distributed_money = money_to_distribute;
    }
    
    void _plant_farm_land() {
        // процесс засева сельхоз земель
        short square_to_plant = 0;
        while (true) {
            square_to_plant = this->_request_int_value("Сколько квадратных миль земли вы хотите засеять? ");
            if (square_to_plant > this->farm_land) {
                std::cout << "Увы, у вас есть только " << this->farm_land << " квадратных миль сельскохозяйственных земель" << std::endl;
            } else if (square_to_plant > this->countrymen * square_countryman_can_plant) {
                std::cout << "Увы, каждый житель может засеять только " << square_countryman_can_plant << " квадратные мили" << std::endl;
            } else if (square_to_plant * this->cost_of_planting_land > this->balance) {
                std::cout << "Подумайте еще раз. У вас осталось лишь " << this->balance << " роллодов в казне" << std::endl;
            } else {
                break;
            };
        }
        this->_spend_money(square_to_plant * this->cost_of_planting_land);
        this->planted_square = square_to_plant;
    }
    
    void _pollution_control() {
        // процесс вкладывания денег в контроль загрязнений
        int money_for_pollution_control = 0;
        while (true) {
            money_for_pollution_control = this->_request_int_value("Сколько роллодов вы хотите потратить на контроль загрязнений? ");
            if (money_for_pollution_control > this->balance) {
                std::cout << "Подумайте еще раз. У вас осталось лишь " << this->balance << " роллодов в казне" << std::endl;
            } else {
                break;
            };
        }
        this->_spend_money(money_for_pollution_control);
        this->money_spent_for_pollution_control = money_for_pollution_control;
    }
    
    void _cut_down_forest() {
        // процесс вырубки леса
        int cut_down_square = 0;
        while (true) {
            cut_down_square = this->_request_int_value("Сколько квадратных миль леса вы хотите вырубить? ");
            if (cut_down_square > this->forest_land) {
                std::cout << "Подумайте еще раз. У вас осталось лишь " << this->forest_land << " квадртаных миль леса" << std::endl;
            } else {
                break;
            };
        }
        this->forest_land -= cut_down_square;
        this->farm_land += cut_down_square;
        this->_add_money(price_for_cutting_down_forest * cut_down_square);
        short deaths_when_cutting = get_random_choise(1, 5);
        if (deaths_when_cutting == 1) {
            int dead_count = this->countrymen * 0.1;
            this->countrymen -= dead_count;
            std::cout << "Случилось несчатье при рубке! " << dead_count << " жителей завалило деревьями." << std::endl;
        }
    }
    
    void _count_deaths() {
        // подсчет погибших
        
        // погибшие от голода
        short starved = this->countrymen - this->distributed_money / this->cost_of_living;
        if (starved > 0) {
            std::cout << starved << " жителей умерло от голода" << std::endl;
        };
        
        // погибшие от загрязнений
        // базовое значение - процент от площади земли, проданной под промышленность
        this->died_because_of_pollution = static_cast<int>(this->_get_random_float_from_zero_to_one() * (this->sold_square));
        // уменьшение базового значения, если на контроль загрязнений было выделено больше порога (фактора)
        if (this->money_spent_for_pollution_control >= this->pollution_control_factor) {
            this->died_because_of_pollution = static_cast<int>(this->died_because_of_pollution / (this->money_spent_for_pollution_control / this->pollution_control_factor));
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
                std::cout << "В казне не хватает денег на похороны, придется продать часть земли" << std::endl;
                std::cout << "farm land " << this->farm_land << " balance " << this->balance << " price_of_selling_land " << this->price_of_selling_land << " division " << this->balance / this->price_of_selling_land << std::endl;
                // компенсация отрицательного баланса за счет продажи земли
                // подозрительный момент, т.к. по сути земля продается промшленности, но эта продажа уже нигде не учитывается в вопросах, связанных с промышленностью
                this->farm_land += this->balance / this->price_of_selling_land;
                this->balance = 0;
            }
        }
        this->countrymen -= this->died_count;
    }
    
    void _count_people() {
        // подсчет изменения населения
        
        // инорстранные рабочие
        short foreigners_change = 0;
        if (this->sold_square != 0) {
            foreigners_change = static_cast<int>(this->sold_square + this->_get_random_float_from_zero_to_one() * 10 - this->_get_random_float_from_zero_to_one() * 20);
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
        std::cout << "Население изменилось: " << countrymen_change << " человек";
        if (countrymen_change < 0) {
            std::cout << " покинуло страну" << std::endl;
        } else {
            std::cout << " прибыло в страну" << std::endl;
        }
        this->countrymen += countrymen_change;
        this->foreigners += foreigners_change;
    }
    
    void _count_harvest() {
        // подсчет урожая
        
        // потери урожая из-за загрязнений
        short lost_farm_land = static_cast<int>((this->sold_square) * (this->_get_random_float_from_zero_to_one() + 1.5) / 2);
        if (lost_farm_land > this->planted_square) {
            lost_farm_land = this->planted_square;
        }
        short harvested = this->planted_square - lost_farm_land;
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
        int revenue = static_cast<int>((this->planted_square - lost_farm_land) * this->price_of_selling_land / 2);
        if (revenue < 0) { revenue = 0; }
        this->balance += revenue;
        std::cout << "Вы заработали на урожае " << revenue << " роллодов" << std::endl;
        this->last_year_lost_farm_land = lost_farm_land;
    }
    
    void _count_tourists() {
        // подсчет доходов с туристов
        
        short koef_1 = static_cast<short>(this->_get_settled() * 22 + this->_get_random_float_from_zero_to_one() * 500);
        short koef_2 = static_cast<short>(( this->sold_square) * 15);
        short revenue = 0;
        
        if (koef_1 > koef_2) {
            revenue = koef_1 - koef_2;
        }
        std::cout << "Вы заработали " << revenue << " роллодов на туристах" << std::endl;
        if (koef_2 != 0 && !(koef_1 - koef_2 >= this->last_year_tourists_revenue)) {
            std::cout << "Поток туристов уменьшился, потому что ";
            short reason = get_random_choise(5, 20);
            if (reason == 1) {
                std::cout << "в озерах стало меньше рыбы из-за загрязнения воды";
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
    }
    
    void _process_good_random_event() {
        //TODO
    }
    
    void _process_bad_random_event() {
        short event_number = get_random_choise(5, 20);
        if (event_number == 1) {
            std::cout << BAD_EVENT_1_TEXT_1 << BAD_EVENT_1_AMOUNT_1 << BAD_EVENT_1_TEXT_2 << BAD_EVENT_1_AMOUNT_2 << std::endl;
            this->balance -= (BAD_EVENT_1_AMOUNT_1 + BAD_EVENT_1_AMOUNT_2);
        } else if (event_number == 2) {
            std::cout << BAD_EVENT_2_TEXT_1 << BAD_EVENT_2_AMOUNT << BAD_EVENT_2_TEXT_2 << std::endl;
            this->balance -= BAD_EVENT_2_AMOUNT;
        } else if (event_number == 3) {
            std::cout << BAD_EVENT_3_TEXT_1 << BAD_EVENT_3_AMOUNT << BAD_EVENT_3_TEXT_2 << std::endl;
            this->balance -= BAD_EVENT_3_AMOUNT;
        } else if (event_number == 4) {
            std::cout << BAD_EVENT_4_TEXT_1 << BAD_EVENT_4_AMOUNT << BAD_EVENT_4_TEXT_2 << std::endl;
            this->balance -= BAD_EVENT_4_AMOUNT;
        } else if (event_number == 5) {
            std::cout << BAD_EVENT_5_TEXT_1 << BAD_EVENT_5_AMOUNT_PER_COUNTRYMAN << BAD_EVENT_5_TEXT_2 << std::endl;
            this->balance -= BAD_EVENT_5_AMOUNT_PER_COUNTRYMAN * this->countrymen;
        }
        event_number = 0;
    }
    
    void _play_quiz(std::string question, std::string neutral_answer,
                   std::string good_text_1, std::string good_text_2, int good_amount,
                   std::string bad_text_1, std::string bad_text_2, int bad_amount,
                    int died_countryman_percent) {
        std::string desicion = "";
        std::cout << question << std::endl;
        std::cin >> desicion;
        if (desicion == "1") {
            std::cout << neutral_answer;
        } else {
            short event_type = get_random_choise(2, 50);
            if (event_type == 1) {
                // удача
                std::cout << good_text_1 << good_amount << good_text_2 << std::endl;
                this->balance += good_amount;
            } else {
                // неудача
                std::cout << bad_text_1 << bad_amount << bad_text_2 << std::endl;
                this->balance -= bad_amount;
                this->countrymen *= (100 - died_countryman_percent) / 100;
            }
            event_type = 0;
        }
    }
    
    void _process_quiz() {
        short event_number = get_random_choise(3, 33);
        if (event_number == 1) {
            this->_play_quiz(QUIZ_1_QUESTION, QUIZ_1_NEUTRAL_TEXT,
                             QUIZ_1_GOOD_TEXT_1, QUIZ_1_GOOD_TEXT_2, QUIZ_1_GOOD_AMOUNT,
                             QUIZ_1_BAD_TEXT_1, QUIZ_1_BAD_TEXT_2, QUIZ_1_BAD_AMOUNT, 0);
        } else if (event_number == 2) {
            this->_play_quiz(QUIZ_2_QUESTION, QUIZ_2_NEUTRAL_TEXT,
                             QUIZ_2_GOOD_TEXT_1, QUIZ_2_GOOD_TEXT_2, QUIZ_2_GOOD_AMOUNT,
                             QUIZ_2_BAD_TEXT_1, QUIZ_2_BAD_TEXT_2, QUIZ_2_BAD_AMOUNT, 0);
        } else if (event_number == 3) {
            this->_play_quiz(QUIZ_3_QUESTION, QUIZ_3_NEUTRAL_TEXT,
                             QUIZ_3_GOOD_TEXT_1, QUIZ_3_GOOD_TEXT_2, QUIZ_3_GOOD_AMOUNT,
                             QUIZ_3_BAD_TEXT_1, QUIZ_3_BAD_TEXT_2, QUIZ_3_BAD_AMOUNT, 5);
        }
    }
    
public:
    
    GameState() {
        this->read_config();
    }
    
    void print_header() {
        // вывести приветствие
        
        std::cout << "KING" << std::endl;
        std::cout << "Was ppublished in Basic Computer Games (1978)" << std::endl;
        std::cout << "Author: @taraskvitko" << std::endl;
        std::cout << "Powered by Dialas" << std::endl;
        std::cout << "Version 1.3.1\n\n\n" << std::endl;
    }
    
    void print_intro() {
        // вывести интро
        std::cout << INTRO_PART_1 << this->cost_of_living << INTRO_PART_2 << std::endl;
    }
    
    void print_state() {
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
    
    void init_new_year() {
        // начать новый год правления
        
        this->years += 1;
        if (this->years == 1 && !this->custom_game) {
            this->balance = get_random_short_from_range(start_balance_min, start_balance_max);
            this->countrymen = get_random_short_from_range(start_contrymen_min, start_contrymen_max);
        };
        this->_set_prices_for_land();
        this->died_count = 0;
    }
    
    void init_speciphic_year(short years, int balance, int countrymen, int foreigners, int farm_land) {
        // начать год правления с конкретными вводными
        
        this->custom_game = true;
        this->years = years;
        this->balance = balance;
        this->countrymen = countrymen;
        this->foreigners = foreigners;
        this->farm_land = farm_land;
    }
    
    void get_resume_data() {
        // задать пользователю вопросы по старту игры и запустить игру
        short years = -1;
        int balance = -1;
        int countrymen = -1;
        int foreigners = -1;
        int farm_land = -1;
        
        while (years < 0) {
            std::cout << "Сколько лет вы занимали пост перед тем, как покинули его? ";
            std::cin >> years;
        }
        while (balance < 0) {
            std::cout << "Сколько у вас в казне? ";
            std::cin >> balance;
        }
        while (countrymen < 0) {
            std::cout << "Сколько жителей? ";
            std::cin >> countrymen;
        }
        while (foreigners < 0) {
            std::cout << "Сколько иностранных рабочих? ";
            std::cin >> foreigners;
        }
        while (farm_land < 0) {
            std::cout << "Сколько квадратных миль сельхоз земли? ";
            std::cin >> farm_land;
        }
        init_speciphic_year(years, balance, countrymen, foreigners, farm_land);
    }
    
    void get_gamer_decisions() {
        // задать пользователю вопросы и применить ответы
        
        this->_sell_land_to_industry();
        this->_distribute_money_to_countryman();
        this->_plant_farm_land();
        this->_pollution_control();
        this->_cut_down_forest();
    }
    
    void process_year() {
        // произвести вычисления
        
        this->_count_deaths();
        this->_count_people();
        this->_count_harvest();
        this->_count_tourists();
    }
    
    bool get_year_results() {
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
    
    void process_random_evet() {
        short event_type = get_random_choise(3, 10);
        // 1 - позитивное, 2 - негативное, 3 - викторина
        if (event_type == 1) {
            this->_process_good_random_event();
        } else if (event_type == 2) {
            this->_process_bad_random_event();
        } else if (event_type == 3) {
            this->_process_quiz();
        }
        event_type = 0;
    }
};

int main(int argc, const char * argv[]) {
    
    #ifdef _WIN32
     SetConsoleOutputCP(CP_UTF8);
    #else
     setlocale(LC_CTYPE, "rus");
    #endif

    GameState game = GameState();
    game.print_header();

    std::string show_intro = "";
    std::cout << QUESTION_ABOUT_INTRO;
    std::cin >> show_intro;
    if (show_intro == "y") {
        game.print_intro();
    }

    std::string resume = "";
    std::cout << QUESTION_ABOUT_GAME_MODE;
    std::cin >> resume;
    if (resume == "2") {
        game.get_resume_data();
    }

    while (true) {
        game.init_new_year();
        game.print_state();
        game.get_gamer_decisions();

        // ежемесячные случайные события
//        for (size_t i = 0; i < 12; ++i) {
//            std::cout << "...идет месяц " << i + 1 << std::endl;
//            game.process_random_evet();
//        }

        game.process_year();
        bool is_game_over = game.get_year_results();
//        game.print_state();
        if (is_game_over) {
            return 0;
        }
    }
}
