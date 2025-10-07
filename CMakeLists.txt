cmake_minimum_required(VERSION 3.10)
project(SnakeGame)

# Find GoogleTest
find_package(GTest REQUIRED)

add_executable(snake_game src/main.cpp src/snake.cpp)

add_executable(snake_tests tests/snake_test.cpp tests/snake_input_test.cpp tests/test_main.cpp src/snake.cpp)

target_link_libraries(snake_tests GTest::GTest GTest::Main pthread)

