//
//  check_hash.cpp
//  King
//
//  Created by tkvitko on 28.06.2024.
//

#include <iostream>
#include <string>
#include "GameResult.hpp"
#include "utils.hpp"

int main(int argc, const char * argv[]) {
    
    std::cout << "Год правления: " << std::endl;
    int year = get_valid_integer_input(0, 8, true);
    
    std::cout << "Казна: " << std::endl;
    int balance = get_valid_integer_input(1, 500000, true);
    
    std::cout << "Население: " << std::endl;
    int countrymen = get_valid_integer_input(1, 2000, true);
    
    GameResult result = GameResult{year, balance, countrymen};
    std::hash<GameResult> hashFunction;
    auto year_hash = hashFunction(result);
    std::cout << "Хеш: " << year_hash << std::endl;
}
