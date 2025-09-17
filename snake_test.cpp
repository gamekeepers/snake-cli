#include <gtest/gtest.h>
#include "snake.h"

TEST(SnakeBehaviour, NextHeadRight) {
    pair<int, int> current = {5, 9};
    EXPECT_EQ(get_next_head(current, 'r'), make_pair(5, 0)); // wrap
}

TEST(SnakeBehaviour, NextHeadLeft) {
    pair<int, int> current = {5, 0};
    EXPECT_EQ(get_next_head(current, 'l'), make_pair(5, 9)); // wrap
}

TEST(SnakeBehaviour, NextHeadUp) {
    pair<int, int> current = {0, 5};
    EXPECT_EQ(get_next_head(current, 'u'), make_pair(9, 5)); // wrap
}

TEST(SnakeBehaviour, NextHeadDown) {
    pair<int, int> current = {9, 5};
    EXPECT_EQ(get_next_head(current, 'd'), make_pair(0, 5)); // wrap
}
