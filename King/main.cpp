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
#include "utils.hpp"
#include "strings.hpp"
#include "exceptions.hpp"
#include "Game.cpp"

#ifdef _WIN32
#include <Windows.h>
#endif


int main(int argc, const char * argv[]) {
    
    #ifdef _WIN32
     SetConsoleOutputCP(CP_UTF8);
    #else
     setlocale(LC_CTYPE, "rus");
    #endif

    while (true) {
        try {
            std::cout << HEADER << std::endl;
            
            // показывать ли инструкцию
            std::cout << QUESTION_ABOUT_INTRO;
            int choise_intro = get_valid_integer_input(1, 2, false);
            if (choise_intro == 1) {
                std::cout << INTRO_PART_1 << 100 << INTRO_PART_2 << std::endl;
            }
            
            // игра с нуля или вводим стартовые значения
            bool custom_game = false;
            std::cout << QUESTION_ABOUT_GAME_MODE;
            int choise_mode = get_valid_integer_input(1, 2, false);
            if (choise_mode == 2) {
                custom_game = true;
            }

            // игра со случайными событиями или без
            bool random_events_enabled = false;
            std::cout << QUESTION_ABOUT_RANDOM_EVENTS;
            int choise_random = get_valid_integer_input(1, 2, false);
            if (choise_random == 1) {
                random_events_enabled = true;
            }
            
            // игра с текстом для конкурса или нет
            bool competition = false;
            std::cout << QUESTION_ABOUT_COMPETITION;
            int choise_competition = get_valid_integer_input(1, 2, false);
            if (choise_competition == 1) {
                competition = true;
            }
            
            Game game = Game(custom_game, random_events_enabled, competition);
            while (true) {
                game.processYear();
            }

        } catch (ExitGame& e) {
            return 0;
        } catch (RestartGame& e) {
            continue;
        } catch (GameOver& e) {
            std::cout << "Game Over" << std::endl;
//            system("pause");
            return 0;
        }
    }
}
