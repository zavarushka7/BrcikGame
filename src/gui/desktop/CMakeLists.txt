cmake_minimum_required(VERSION 3.5)

project(BrickGameApp)

add_definitions(-DQT_NO_NCURSES_MACROS)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp  
    mainwindow.cpp
    ../../brick_game/tetris/backend.c
    ../../brick_game/snake/snakeModel.cpp
    ../../brick_game/snake/snakeController.cpp
)

set(HEADERS
    mainwindow.h
    ../../brick_game/brick_game.h
    ../../brick_game/tetris/backend.h
    ../../brick_game/snake/snakeModel.h
    ../../brick_game/snake/snakeController.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../brick_game
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../brick_game/tetris
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../brick_game/snake
)