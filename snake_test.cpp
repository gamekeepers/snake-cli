#include "snake.h"
#include <gtest/gtest.h>

char direction = 'r'; // Needed for linking

// ✅ Test next head calculation
TEST(SnakeTest, NextHeadRight)
{
    pair<int, int> pos = {0, 0};
    auto next = get_next_head(pos, 'r');
    EXPECT_EQ(next, make_pair(0, 1));
}

TEST(SnakeTest, NextHeadWrapLeft)
{
    pair<int, int> pos = {0, 0};
    auto next = get_next_head(pos, 'l');
    EXPECT_EQ(next, make_pair(0, BOARD_SIZE - 1));
}

TEST(SnakeTest, NextHeadDown)
{
    pair<int, int> pos = {0, 0};
    auto next = get_next_head(pos, 'd');
    EXPECT_EQ(next, make_pair(1, 0));
}

TEST(SnakeTest, NextHeadUpWrap)
{
    pair<int, int> pos = {0, 0};
    auto next = get_next_head(pos, 'u');
    EXPECT_EQ(next, make_pair(BOARD_SIZE - 1, 0));_