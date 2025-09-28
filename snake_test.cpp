#include <gtest/gtest.h>
#include "snake.h"

TEST(SnakeBehaviour, NextHeadRight) {
    EXPECT_EQ(get_next_head({5, 5}, 'r'), make_pair(5, 6));
    EXPECT_EQ(get_next_head({5, 9}, 'r'), make_pair(5, 0));
}

TEST(SnakeBehaviour, NextHeadLeft) {
    EXPECT_EQ(get_next_head({5, 5}, 'l'), make_pair(5, 4));
    EXPECT_EQ(get_next_head({5, 0}, 'l'), make_pair(5, 9));
}

TEST(SnakeBehaviour, NextHeadUp) {
    EXPECT_EQ(get_next_head({5, 5}, 'u'), make_pair(4, 5));
    EXPECT_EQ(get_next_head({0, 5}, 'u'), make_pair(9, 5));
}

TEST(SnakeBehaviour, NextHeadDown) {
    EXPECT_EQ(get_next_head({5, 5}, 'd'), make_pair(6, 5));
    EXPECT_EQ(get_next_head({9, 5}, 'd'), make_pair(0, 5));
}