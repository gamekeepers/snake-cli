#include <gtest/gtest.h>
#include "snake.h"

TEST(SnakeInput, ChangeDirection) {
    shared_ptr<MockInputManager> input_manager = make_shared<MockInputManager>();
    
    Game game1(input_manager);
    input_manager->setNextInput('w');
    handle_single_input(game1);
    EXPECT_EQ(game1.snake.get_direction(), 'u');

    Game game2(input_manager);
    input_manager->setNextInput('s');
    handle_single_input(game2);
    EXPECT_EQ(game2.snake.get_direction(), 'd');

    Game game3(input_manager);
    input_manager->setNextInput('a');
    handle_single_input(game3);
    EXPECT_EQ(game3.snake.get_direction(), 'l');

    Game game4(input_manager);
    input_manager->setNextInput('d');
    handle_single_input(game4);
    EXPECT_EQ(game4.snake.get_direction(), 'r');
}


