//
//  utils.hpp
//  King
//
//  Created by tkvitko on 06.05.2024.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <exception>

short get_random_choise(short choises_number, short probability);
int get_random_short_from_range(const int min_value, const int max_value);
int get_valid_integer_input(int min, int max, bool show_tip);
double getRandomFloatFromZeroToOne_();

class RestartGame : public std::exception {
public:
    const char* what() const noexcept override;
};

class ExitGame : public std::exception {
public:
    const char* what() const noexcept override;
};

#endif /* utils_hpp */
