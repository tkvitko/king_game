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
