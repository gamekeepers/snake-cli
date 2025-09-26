#include "snake.h"
#include <gtest/gtest.h>

char direction = 'r';

TEST(SnakeTest, MoveRight)
{
    pair<int, int> pos = {2, 3};
    auto next = get_next_head(pos, 'r');
    EXPECT_EQ(next, make_pair(2, 4));
}

TEST(SnakeTest, MoveLeft)
{
    pair<int, int> pos = {2, 3};
    auto next = get_next_head(pos, 'l');
    EXPECT_EQ(next, make_pair(2, 2));
}

TEST(SnakeTest, MoveDown)
{
    pair<int, int> pos = {2, 3};
    auto next = get_next_head(pos, 'd');
    EXPECT_EQ(next, make_pair(3, 3));
}

TEST(SnakeTest, MoveUp)
{
    pair<int, int> pos = {2, 3};
    auto next = get_next_head(pos, 'u');
    EXPECT_EQ(next, make_pair(1, 3));
}

TEST(SnakeTest, WrapRightEdge)
{
    pair<int, int> pos = {1, BOARD_SIZE - 1};
    auto next = get_next_head(pos, 'r');
    EXPECT_EQ(next, make_pair(1, 0));
}

TEST(SnakeTest, WrapLeftEdge)
{
    pair<int, int> pos = {1, 0};
    auto next = get_next_head(pos, 'l');
    EXPECT_EQ(next, make_pair(1, BOARD_SIZE - 1));
}

TEST(SnakeTest, WrapBottomEdge)
{
    pair<int, int> pos = {BOARD_SIZE - 1, 2};
    auto next = get_next_head(pos, 'd');
    EXPECT_EQ(next, make_pair(0, 2));
}

TEST(SnakeTest, WrapTopEdge)
{
    pair<int, int> pos = {0, 2};
    auto next = get_next_head(pos, 'u');
    EXPECT_EQ(next, make_pair(BOARD_SIZE - 1, 2));
}

TEST(SnakeTest, WrapTopLeft)
{
    pair<int, int> pos = {0, 0};
    auto next = get_next_head(pos, 'u');
    EXPECT_EQ(next, make_pair(BOARD_SIZE - 1, 0));
    next = get_next_head(pos, 'l');
    EXPECT_EQ(next, make_pair(0, BOARD_SIZE - 1));
}

TEST(SnakeTest, WrapBottomRight)
{
    pair<int, int> pos = {BOARD_SIZE - 1, BOARD_SIZE - 1};
    auto next = get_next_head(pos, 'd');
    EXPECT_EQ(next, make_pair(0, BOARD_SIZE - 1));
    next = get_next_head(pos, 'r');
    EXPECT_EQ(next, make_pair(BOARD_SIZE - 1, 0));
}

TEST(SnakeTest, InvalidDirection)
{
    pair<int, int> pos = {3, 3};
    auto next = get_next_head(pos, 'x');
    EXPECT_EQ(next, pos);
}
