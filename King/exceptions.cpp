//
//  exceptions.cpp
//  King
//
//  Created by tkvitko on 23.05.2024.
//

#include "exceptions.hpp"

const char* RestartGame::what() const noexcept {
    return "Перезапуск игры";
};

const char* ExitGame::what() const noexcept {
    return "Выход из игры";
};

const char* GameOver::what() const noexcept {
    return "Вы проиграли";
};
