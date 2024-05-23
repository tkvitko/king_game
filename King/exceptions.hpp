//
//  exceptions.hpp
//  King
//
//  Created by tkvitko on 23.05.2024.
//

#ifndef exceptions_hpp
#define exceptions_hpp

#include <stdio.h>
#include <exception>

class RestartGame : public std::exception {
public:
    const char* what() const noexcept override;
};

class ExitGame : public std::exception {
public:
    const char* what() const noexcept override;
};

class GameOver : public std::exception {
public:
    const char* what() const noexcept override;
};

#endif /* exceptions_hpp */
