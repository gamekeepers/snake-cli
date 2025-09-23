#include <gtest/gtest.h>
#include "snake.h"

// ---------- get_next_head ----------
TEST(SnakeBehaviour, NextHeadRight) {
    pair<int, int> current = {5, 9};
    EXPECT_EQ(Game::get_next_head(current, 'r'), make_pair(5, 0));
}
TEST(SnakeBehaviour, NextHeadLeft) {
    pair<int, int> current = {5, 0};
    EXPECT_EQ(Game::get_next_head(current, 'l'), make_pair(5, 9));
}
TEST(SnakeBehaviour, NextHeadUp) {
    pair<int, int> current = {0, 5};
    EXPECT_EQ(Game::get_next_head(current, 'u'), make_pair(9, 5));
}
TEST(SnakeBehaviour, NextHeadDown) {
    pair<int, int> current = {9, 5};
    EXPECT_EQ(Game::get_next_head(current, 'd'), make_pair(0, 5));
}

// ---------- Snake class ----------
TEST(SnakeClass, MovementAndGrowth) {
    Snake s;
    auto h = s.head();
    auto next = Game::get_next_head(h, 'r');
    s.move(next, false);
    EXPECT_EQ(s.head(), next);

    auto growNext = Game::get_next_head(next, 'r');
    s.move(growNext, true);
    EXPECT_EQ(s.getBody().size(), 3);
}

// ---------- Food class ----------
TEST(FoodClass, FoodAndPoisonNotOnSnake) {
    Snake s;
    Food f;
    f.spawn(s.getBody());
    EXPECT_NE(f.getFood(), s.head());
    EXPECT_NE(f.getPoison(), f.getFood());
}
