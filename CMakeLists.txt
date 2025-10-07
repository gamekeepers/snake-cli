cmake_minimum_required(VERSION 3.10)
project(SnakeGame)

# Add sources
set(GAME_SOURCES main.cpp snake.cpp)
set(TEST_SOURCES snake_test.cpp snake_input_test.cpp snake.cpp test_main.cpp)

# Find GoogleTest
find_package(GTest REQUIRED)

# Create game executable
add_executable(snake_game ${GAME_SOURCES})

# Create test executable
add_executable(snake_tests ${TEST_SOURCES})
target_link_libraries(snake_tests GTest::GTest GTest::Main pthread)

# Add test
include(GoogleTest)
gtest_discover_tests(snake_tests)