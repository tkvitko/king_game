cmake_minimum_required(VERSION 3.20)
set (PROJECT_NAME premier)
project(${PROJECT_NAME})

if(MSVC)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

set (SOURCES 
    main.cpp
    Account.cpp
    Game.cpp
    Land.cpp
    People.cpp

    exceptions.cpp
    utils.cpp
)

set (HEADERS
    Account.hpp
    Game.hpp
    GameResult.hpp
    Land.hpp
    People.hpp

    constants.h
    events.hpp
    exceptions.hpp
    strings.hpp
    utils.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
