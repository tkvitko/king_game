//
//  GameResult.cpp
//  King
//
//  Created by tkvitko on 24.05.2024.
//

#include <algorithm>
#include <vector>
#include <string>
#include "GameResult.hpp"

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
