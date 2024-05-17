//
//  utils.cpp
//  King
//
//  Created by tkvitko on 06.05.2024.
//

#include <random>
#include <ctime>
#include <iostream>
#include <chrono>
#include <thread>
#include "utils.hpp"

short get_random_choise(short choises_number, short probability) {
    // Функция вернет выпавший вариант в соответствии с вероятностью выпадения
    
    // сон секунду на случай частых вызовов
    std::chrono::milliseconds timespan(1000);
    std::this_thread::sleep_for(timespan);
    
    // случайное значение от 0 до 99
    std::srand(std::time(0));
    short result = std::rand() % 100;
    
    // первый интервал для проверки
    short start = 0;
    short end = probability;
    
    // пока есть варианты, идем от 0 до 100 шагами, равными запрошенной вероятности
    for (size_t i = 0; i < choises_number; ++i) {
        if (start <= result && result <= end) {
            // если на текущем шаге встретилось выпавшее значение, возвращаем вариант
            return i + 1;
        };
        // иначе на следующий шаг
        start += probability;
        end += probability;
    }
    
    // если интервалы закончились, а значение не встретилось, возвращаем 0
    return 0;
}


int get_random_short_from_range(const int min_value, const int max_value) {
    int range = max_value - min_value;
    // сон секунду на случай частых вызовов
//    std::chrono::milliseconds timespan(1000);
//    std::this_thread::sleep_for(timespan);
    
    // случайное значение из range
    std::srand(std::time(0));
    int result = std::rand() % range + 1;
    return min_value + result;
}



bool is_integer_str(const std::string& text) {

    for (char symbol : text) {
            if (!isdigit(symbol)) {
                return false;
            }
        }
    return true;
}

bool is_in_limits(int num, int min, int max) {
    if (num < min || num > max) {
        return false;
    }
    return true;
}

void show_tip(int min, int max) {
    std::cout << "Введите число от " << min << " до " << max << std::endl;
}

int get_valid_integer_input(int min, int max, bool need_tip) {
    
    if (need_tip) {
        show_tip(min, max);
    }
    
    bool got = false;
    while (!got) {
        std::string text;
        std::cin >> text;
        if (text.size() < 10) {
            if (is_integer_str(text)) {
                int num = stoi(text);
                if (is_in_limits(num, min, max)) {
                    return num;
                } else {
                    show_tip(min, max);
                }
            } else {
                show_tip(min, max);
            }
        } else {
            show_tip(min, max);
        }
    };
    return 0;
}
