#include <gtest/gtest.h>
#include "snake.h"

TEST(SnakeTest, InitialPosition) {
    Snake snake(10);
    ASSERT_EQ(snake.get_body().size(), 1);
    EXPECT_EQ(snake.get_head().x, 0);
    EXPECT_EQ(snake.get_head().y, 0);
}

TEST(SnakeTest, MoveRight) {
    Snake snake(10);
    snake.set_direction('r');
    snake.move();
    EXPECT_EQ(snake.get_head().x, 0);
    EXPECT_EQ(snake.get_head().y, 1);
}

TEST(SnakeTest, MoveLeftWrapAround) {
    Snake snake(10);
    snake.set_direction('l');
    snake.move();
    EXPECT_EQ(snake.get_head().x, 0);
    EXPECT_EQ(snake.get_head().y, 9);
}

TEST(SnakeTest, MoveDown) {
    Snake snake(10);
    snake.set_direction('d');
    snake.move();
    EXPECT_EQ(snake.get_head().x, 1);
    EXPECT_EQ(snake.get_head().y, 0);
}

TEST(SnakeTest, MoveUpWrapAround) {
    Snake snake(10);
    snake.set_direction('u');
    snake.move();
    EXPECT_EQ(snake.get_head().x, 9);
    EXPECT_EQ(snake.get_head().y, 0);
}

TEST(SnakeTest, Grow) {
    Snake snake(10);
    snake.grow();
    ASSERT_EQ(snake.get_body().size(), 2);
    EXPECT_EQ(snake.get_head().x, 0);
    EXPECT_EQ(snake.get_head().y, 1);
}

TEST(SnakeTest, Collision) {
    Snake snake(10);
    snake.grow(); // {0,1}, {0,0}
    snake.grow(); // {0,2}, {0,1}, {0,0}
    snake.grow(); // {0,3}, {0,2}, {0,1}, {0,0}
    snake.set_direction('d');
    snake.grow(); // {1,3}, {0,3}, {0,2}, {0,1}, {0,0}
    snake.set_direction('l');
    snake.grow(); // {1,2}, {1,3}, {0,3}, {0,2}, {0,1}, {0,0}
    snake.set_direction('u');
    snake.grow(); // {0,2}, {1,2}, {1,3}, {0,3}, {0,2}, {0,1}, {0,0}
    EXPECT_TRUE(snake.check_collision());
}

TEST(FoodTest, InitialPosition) {
    Snake snake(10);
    Food food(10, snake.get_body());
    Point pos = food.get_position();
    EXPECT_GE(pos.x, 0);
    EXPECT_LT(pos.x, 10);
    EXPECT_GE(pos.y, 0);
    EXPECT_LT(pos.y, 10);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}