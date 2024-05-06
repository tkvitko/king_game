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
    int initial_land = 2000;
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
    
    // режим
    bool custom_game = false;    // обычный режим игры без ввода стартовых данных пользователем
    
    // время
    unsigned short years = 0;  // лет правления
    
    // финансы
    int balance = 0;  // количество денег в казне
    
    // люди
    int countrymen = 0;  // количество жителей
    int foreigners = 0; // количество иностранных рабочих
    
    // земли
    unsigned int total_land = initial_land;  // общая площадь земли
    unsigned int forest_land = 1000; // площадь леса

    // параметры текущего года
    int died_count = 0;                  // общее количество погибших жителей
    int died_because_of_pollution = 0;   // количество погибших жителей по причине загрязнения
    int population_change = 0;                    // изменение населения
    
    // экономика текущего года
    unsigned int cost_of_planting_land = 0;       // стоимость засева земли
    unsigned int price_of_selling_land = 0;       // цена продажи земли
    unsigned int imcome_from_tourism = 0;         // доход от туризма
    unsigned int settled = this->countrymen - this->population_change; // оседлые жители
    
    // решения текущего года
    int sold_square = 0;
    int distributed_money = 0;
    int planted_square = 0;
    int money_spent_for_pollution_control = 0;
    
    // экономика предыдущего года
    short last_year_lost_farm_land = 0;     // потери урожая
    int last_year_tourists_revenue = 0;   // заработок на туристах
    
    // приватные методы для внутренней работы
    template <class T>
    T _get_random_int_from_range(const T min_value, const T max_value) {
        std::default_random_engine generator;
        std::uniform_int_distribution<T> distribution(min_value, max_value);
        return distribution(generator);
    };
    
    void read_config() {
        // чтение конфигурации
        std::vector<std::string> ln = {
            "initial_land",
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
                          this->total_land,
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
    
    unsigned short _get_countryman_count() { return this->countrymen;  }
    unsigned short _get_farm_land_square() { return this->total_land - this->forest_land;  }
    long _add_money(int amount) {return this->balance += amount; }
    long _spend_money(const int amount) {
        if (amount <= this->balance) {
            return this->balance -= amount;
        } return -1;
    }
    
    void _set_prices_for_land() {
        // установка цен на продажу земли и посев на земле
        this->cost_of_planting_land = this->_get_random_int_from_range(cost_of_planting_land_min, cost_of_planting_land_max);
        this->price_of_selling_land = this->_get_random_int_from_range(price_of_selling_land_min, price_of_selling_land_max);
    }

    void _sell_land_to_industry() {
        // процесс продажи земли иностранной промышленности
        short square_to_sell = 0;
        while (true) {
            square_to_sell = this->_request_int_value("Сколько квадратных миль земли вы хотите продать под промышленность? ");
            if (square_to_sell < this->_get_farm_land_square()) {
                break;
            } else {
                std::cout << "Подумайте ещё раз. У вас есть всего " << this->_get_farm_land_square() << " квадратных миль сельскохозяйственной земли." << std::endl;
            }
        };
        this->total_land -= square_to_sell;
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
            if (square_to_plant > this->_get_farm_land_square()) {
                std::cout << "Увы, у вас есть только " << this->_get_farm_land_square() << " квадратных миль сельскохозяйственных земель" << std::endl;
            } else if (square_to_plant > this->_get_countryman_count() * square_countryman_can_plant) {
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
    
    void _count_deaths() {
        // подсчет погибших
        
        // погибшие от голода
        short starved = this->countrymen - this->distributed_money / this->cost_of_living;
        if (starved > 0) {
            std::cout << starved << " жителей умерло от голода" << std::endl;
        };
        
        // погибшие от загрязнений
        // базовое значение - процент от площади земли, проданной под промышленность
        this->died_because_of_pollution = static_cast<int>(this->_get_random_float_from_zero_to_one() * (this->initial_land - this->total_land));
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
                this->total_land += this->balance / this->price_of_selling_land;    // компенсация отрицательного баланса за счет продажи земли
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
                                                   - ((this->initial_land - this->total_land) / 50 )
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
        short lost_farm_land = static_cast<int>((this->initial_land - this->total_land) * (this->_get_random_float_from_zero_to_one() + 1.5) / 2);
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
        
        short koef_1 = static_cast<short>(this->settled * 22 + this->_get_random_float_from_zero_to_one() * 500);
        short koef_2 = static_cast<short>(( this->initial_land - this->total_land) * 15);
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
        std::cout << "Version 1.2.1\n\n\n" << std::endl;
    }
    
    void print_intro() {
        // вывести интро
        
        std::cout << "Поздравляем! Вас только что избрали премьер-министром Сетац Детину - маленького коммунистического острова размером 30 на 70 миль. Ваша задача - управлять бюджетом страны и распределять деньги из общественной казны. Денежная единица - роллод, и каждому жителю нужно " << this->cost_of_living << " роллодов в год чтобы выжить. Доход поступает за счет сельского хозяйства и туристов, посещающих ваши великолепные леса для охоты, рыбалки и просто прогулок. Часть вашей земли - сельскохозяйственная, но она так же богата полезными ископаемыми и может быть продана иностранной промышленности, которая привезет своих собственных рабочих. Засев квадратной мили сельскохозяйственной земли стоит 10-15 роллодов в год. Ваша цель - завершить ваш 8-летний срок правления. Удачи!\n\n" << std::endl;
    }
    
    void print_state() {
        // вывести состояние игры
        
        std::cout << "В казне " << this->balance << " ролодов" << std::endl;
        std::cout << "В стране проживает " << this->countrymen << " жителей";
        if (this->foreigners > 0) {
            std::cout << " и " << this->foreigners << " иностранных рабочих" << std::endl;
        } else {
            std::cout << std::endl;
        };
        std::cout << "У нас " << this->total_land << " квадратных миль земли: " << this->_get_farm_land_square() << " сельхоз и " << this->forest_land << " леса" << std::endl;
        std::cout << "В этом году промышленность готова платить " << this->price_of_selling_land << " роллодов за квадратную милю, а стоимость засева одной квадратной мили равна " << this->cost_of_planting_land << std::endl ;
    }
    
    void init_new_year() {
        // начать новый год правления
        
        this->years += 1;
        if (this->years == 1 && !this->custom_game) {
            this->balance = this->_get_random_int_from_range(start_balance_min, start_balance_max);
            this->countrymen = this->_get_random_int_from_range(start_contrymen_min, start_contrymen_max);
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
        this->total_land = this->forest_land + farm_land;
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
        
        // если слишком много погибших
        if (this->died_count > 200) {
            std::cout << this->died_count << " жителей умерло за год! И-за такого ужасного управления вас не только лишили должности и сняли с занимаемого поста, ";
            short reason = get_random_choise(3, 33);
            if (reason == 1) {
                std::cout << "но и выбили вам левый глаз!";
            } else if (reason == 2) {
                std::cout << "еще вы заработали очень плохую репутацию.";
            } else {
                std::cout << "но и назвали предателем родины.";
            }
            std::cout << std::endl;
            return true;
        }
        
        // если слишком мало жителей
        if (this->countrymen < 343) {
            std::cout << "Больше трети населения умерло с тех пор как вы были избраны. Население (оставшееся) ненавидит вас." << std::endl;
            return true;
        }
        
        // если слишком много денег осталось в казне
        if (this->balance / 100 > 5 && this->died_count - this->died_because_of_pollution >= 2) {
            std::cout << "В казне остались не потраченные деньги. В результате некоторые жители умерли от голода. Население в ярости. Вы должны или уйти в отставку, или покончить собой. Выбор за вами. Если выберете второе, просьба выключить компьютер перед тем, как приступите." << std::endl;
            return true;
        }
        
        // если слишком много иностранцев
        if (this->foreigners > this->countrymen) {
            std::cout << "Число иностранных рабочих превысило число жителей. Будучи в меньшинстве, они восстали и захватили власть в стране" << std::endl;
            return true;
        }
        
        // победа в игре
        if (this->years == 7) {
            std::cout << "Поздравляем! Вы успешно завершили ваш 8-летний срок на занимаемой должности. Конечно, вам чрезвычайно повезло. Тем не менее, это огромное достижение. Прощайте и удачи! Она, вероятно, вам пригодится, если вы из тех людей, которые играют в эту игру.";
            return true;
        }
        
        // старт следующего года
        this->init_new_year();
        return false;
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
    std::cout << "Показать инструкцию? (y/n) ";
    std::cin >> show_intro;
    if (show_intro == "y") {
        game.print_intro();
    }

    std::string resume = "";
    std::cout << "Хотите сыграть обычную игру (1) или настроить вручную стартовые параметры (2)? ";
    std::cin >> resume;
    if (resume == "2") {
        game.get_resume_data();
    }

    while (true) {
        game.init_new_year();
        game.print_state();
        game.get_gamer_decisions();
        game.process_year();
        bool is_game_over = game.get_year_results();
//        game.print_state();
        if (is_game_over) {
            return 0;
        }
    }
}
